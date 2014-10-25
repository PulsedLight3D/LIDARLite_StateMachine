LIDAR-Lite-DSS-I2C-Library-State-Machine
========================================

This sketch demonstrates the various abilities of the 
LIDAR-Lite Sensor using a simple State Machine. 


## Dependencies
1. Arduino Uno compatible Board
2. USB Cable
3. LIDAR-Lite Sensor
4. The 'Arduino I2C Master Library' from DSS Circuits: http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

## Installation
1. Download Repository and open with Arduino IDE
2. Install DSS "Arduino I2C Master Library" - Information about installing libraries can be found:  
http://arduino.cc/en/Guide/Libraries

## Usage

1. Connect and Arduino and upload code
2. Open the serial monitor
3. Submit commands via the serial monitor

## Commands
All commands start with a single letter, some commands also require one or two numerical values (0-255), for these commands simply type the letter, the first value and then the second value. 

Command|Description
:---|:---
m | read distance
'n' + '2-9' | read average distance (ex. 'n2' will read average distance for two readings)
'v' + '0-3' | read raw velocity and set scaling register (ex. 'v0' will read velocity at 0.1m/s
'w' + '0-3' + '2-9'  | read average velocity and set scaling register (ex. 'w34' will read velocity at 1m/s and average 4 readings)

*More information about velocity scaling is available in the LIDAR-Lite Operating
Manual on page 22:*
http://pulsedlight3d.com/pl3d/wp-content/uploads/2014/10/LIDAR-Lite-Operating-Manual-PRELIM.pdf

*More information about LIDAR-Lite
Learn more about LIDAR-Lite and PulsedLight:*
http://pulsedlight3d.com
