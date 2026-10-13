[README.md](https://github.com/user-attachments/files/33360910/README.md)
# basketball-shot-tracker
Arduino-based shot tracking device for basketball practices for my foundation. 
# Basketball Shot Tracker

An Arduino-based device that automatically detects and logs made shots during basketball practice, built to help identify when team shooting performance peaks and declines due to fatigue.

## What it does

- Mounts behind the basketball hoop
- Uses an ultrasonic distance sensor to detect when a ball passes through the net
- Beeps as feedback on each made shot
- Logs every shot with a timestamp to a CSV file on an SD card
- Runs untethered on battery power during practice
- After practice, the data can be pulled and analyzed to find peak shooting times and fatigue-related drop-off points

## Hardware

- Elegoo Uno R3 (Arduino-compatible microcontroller)
- HC-SR04 ultrasonic distance sensor
- Active piezo buzzer
- MicroSD card module + microSD card
- 9V battery + barrel jack adapter
- Breadboard (for shared power distribution)

## Wiring

| Component | Pin |
|---|---|
| HC-SR04 Trig | D9 |
| HC-SR04 Echo | D2 |
| Buzzer (+) | D8 |
| SD module MISO | D12 |
| SD module MOSI | D11 |
| SD module SCK | D13 |
| SD module CS | D10 |
| HC-SR04 VCC, SD module VCC | 5V (shared via breadboard rail) |
| All GND | Arduino GND |

## How it works

The Arduino continuously measures distance with the ultrasonic sensor. When a ball passes through the hoop, the distance reading drops sharply for a moment then returns to normal. The code detects this dip as a "shot made" event, counts it, beeps the buzzer, and writes the shot number and timestamp to `session.csv` on the SD card.

## Output data

Each session is logged as a CSV with:
- `shot_number`
- `time_ms` (milliseconds since power-on)
- `time_min` (minutes since power-on, for easier analysis)

This data can be opened in Excel or Google Sheets to chart shots over time and identify fatigue patterns.

## Why I built this

I coach and play basketball, and wanted a way to measure team shooting performance objectively during practice rather than relying on guesswork for when players need a break. This project combines hardware (sensor-based detection) and data logging to give coaches real data on performance trends over the course of a practice.

## Future improvements

- Bluetooth data transfer (HC-05 module) instead of pulling the SD card manually
- A simple web dashboard to visualize shot data automatically
- Rule-based substitution alerts when shooting performance drops below a threshold
