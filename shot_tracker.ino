/*
  Basketball Shot Tracker
  ------------------------
  Detects when a ball passes through the hoop using an ultrasonic
  sensor, beeps a buzzer as feedback, and logs each made shot
  (shot number + time since power-on) to a CSV file on an SD card.

  Wiring:
    HC-SR04   VCC->5V (breadboard rail)   GND->GND   Trig->D9   Echo->D2
    Buzzer    +->D8     -->GND
    SD card   VCC->5V (breadboard rail)   GND->GND   MISO->D12  MOSI->D11  SCK->D13  CS->D10

    Note: HC-SR04 VCC and SD module VCC share the Arduino's single 5V pin
    via a breadboard power rail (one wire from Arduino 5V to the rail,
    then one wire each from the sensor and SD module into that same rail).
*/

#include <SPI.h>
#include <SD.h>

// ---- Pin setup ----
const int trigPin = 9;
const int echoPin = 2;
const int buzzerPin = 8;
const int chipSelect = 10;

// ---- Detection settings (tune these during calibration) ----
const int shotDistanceThreshold = 15;   // cm - distance counts as "ball detected" below this
const unsigned long minTimeBetweenShots = 600; // ms - prevents double-counting one pass

// ---- State tracking ----
int shotCount = 0;
bool ballCurrentlyDetected = false;
unsigned long lastShotTime = 0;

File dataFile;

void setup() {
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card failed or not present. Check wiring/card.");
    // Flash buzzer 3 times to signal SD error even with no computer attached
    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzerPin, HIGH);
      delay(150);
      digitalWrite(buzzerPin, LOW);
      delay(150);
    }
    while (1); // stop here if SD card isn't working
  }
  Serial.println("SD card ready.");

  // Write a header line to a new session file
  dataFile = SD.open("session.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.println("shot_number,time_ms,time_min");
    dataFile.close();
  }

  // Two short beeps = system ready
  beep(100);
  delay(100);
  beep(100);
}

void loop() {
  int distance = getDistanceCM();

  bool ballDetectedNow = (distance > 0 && distance < shotDistanceThreshold);

  // Rising edge: ball just entered range (and enough time has passed since last shot)
  if (ballDetectedNow && !ballCurrentlyDetected) {
    unsigned long now = millis();
    if (now - lastShotTime > minTimeBetweenShots) {
      shotCount++;
      lastShotTime = now;
      logShot(shotCount, now);
      beep(120);
      Serial.print("Shot #");
      Serial.print(shotCount);
      Serial.print(" at ");
      Serial.print(now / 1000.0);
      Serial.println(" sec");
    }
  }

  ballCurrentlyDetected = ballDetectedNow;

  delay(20); // small delay for sensor stability
}

// Measures distance in cm using the HC-SR04
int getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  if (duration == 0) return -1; // no echo received

  int distance = duration * 0.034 / 2;
  return distance;
}

// Writes one shot entry to the CSV file on the SD card
void logShot(int count, unsigned long timeMs) {
  dataFile = SD.open("session.csv", FILE_WRITE);
  if (dataFile) {
    dataFile.print(count);
    dataFile.print(",");
    dataFile.print(timeMs);
    dataFile.print(",");
    dataFile.println(timeMs / 60000.0, 2); // time in minutes, 2 decimal places
    dataFile.close();
  } else {
    Serial.println("Error opening session.csv");
  }
}

// Simple buzzer beep helper
void beep(int durationMs) {
  digitalWrite(buzzerPin, HIGH);
  delay(durationMs);
  digitalWrite(buzzerPin, LOW);
}
