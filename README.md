LIDAR-Lite-DSS-I2C-Library-State-Machine
========================================

This sketch demonstrates the various abilities of the 
LIDAR-Lite Sensor using a simple State Machine. 

## Comands
All commands start with a single letter, some commands also require one or two numerical values (0-255), for these commands simply type the letter, a space, the first value, a space then the second value. 

Command|Short Name|Description
:---|:--|:---
i |Initialize|Initialize Sensor
s | Single Reading|Take single reading
r XX | Read Register| Read register, ex. 'r 12' reads register 12
w XX XX | Write Register|Write to register, ex. 'w 18 19' writes '19' to register '18'

## Dependencies
1. Arduino Uno compatible Board
2. USB Cable
3. LIDAR-Lite Sensor
4. The 'Arduino I2C Master Library' from DSS Circuits: http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

## Installation
Information about installing libraries can be found:  
http://arduino.cc/en/Guide/Libraries

## More information about LIDAR-Lite
Learn more about LIDAR-Lite and PulsedLight:  
http://pulsedlight3d.com
