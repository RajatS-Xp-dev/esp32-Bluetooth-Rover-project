# esp32-Bluetooth-Rover-project
Arduino firmware for an ESP32-powered Bluetooth RC rover using an L293D motor module (2wd)+ pins connections and power distribution.
Connections and Pins:
BatteryPositive- VIN+ of L293D
BatteryNegative/ground- VIN- of L293D
Two DC Gear Motors- connected resp to the two-two Motor output pins, on L293D
ESP32 Power supply-
5V of L293D to VIN pin of ESP
Commom GND with GND pin of ESP
Inputs pins (L293D)-
IN1, IN2-ESP pin 26&27 resp
IN3, IN4-ESP pin 12&13 resp

Bluetooth Mobile Application-
Car Driving interface on Arduino Bluetooth Control App

