LIDARLite_StateMachine
========================================

This Arduino sketch demonstrates the various abilities of the 
LIDAR-Lite Sensor using a simple State Machine. 


## Dependencies
1. Arduino Uno compatible Board
2. Arduino IDE (1.0.5 or newer)
3. USB Cable
4. LIDAR-Lite Sensor
5. The 'Arduino I2C Master Library' from DSS Circuits: http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

## Installation
1. Download Repository and open with Arduino IDE
2. Install DSS "Arduino I2C Master Library" - Information about installing libraries can be found:  
http://arduino.cc/en/Guide/Libraries

## Arduino Setup

![arduino-screenshot](http://pulsedlight3d.net/assets.pl3d/arduino-setup.png)

### LIDARLite I2C Connection Setup
LIDARLite Cable | Arduino Pins
:---|:---
5V | 5V
PWR EN | _(Unused)_
MODE | _(Unused)_
SCL | SCL (topmost pin on the right hand side of Arduino)
SDA | SDA (second pin from the top on the right hand side of Arduino)
GND | GND

## Basic Usage

1. [Connect an Arduino](#arduino-setup) and upload code
2. Open the serial monitor (Make sure baud rate is set to 9600)
3. Submit commands via the serial monitor

###  Commands
All commands start with a single letter, some commands also require one or two numerical values (0-255), for these commands simply type the letter, the first value and then the second value. 

Command|Description
:---|:---
m | read distance
'n' + '2-9' | read average distance (ex. 'n2' will read average distance for two readings)
'v' + '0-3' | read raw velocity and set scaling register (ex. 'v0' will read velocity at 0.1m/s
'w' + '0-3' + '2-9'  | read average velocity and [set scaling register](#references) (ex. 'w34' will read velocity at 1m/s and average 4 readings)
Any other key | stop reading

---

# Detailed Usage

## When you first launch the serial monitor

As soon as you open the serial monitor, the User Manual will print. 

*NOTE: Sometimes if you've just uploaded the code to the Arduino, there will be "junk" output in the serial monitor - you can safely ignore this.*

## Reading Distance

To read distance, simply type "m" into the top input box of the serial monitor and press Enter. A long string of numbers numbers will print one after the other. These are the raw distance read out of the sensor in centimeters. 

![x](http://pulsedlight3d.net/assets.pl3d/ll-m.jpg)

## Stop Reading / Show User Manual

While a process is running, if you press any key a-z, A-Z, 0-9 (any key besides m, n, v, w) the process will stop and the User Manual will print. 

![x](http://pulsedlight3d.net/assets.pl3d/ll-x.jpg)


## Reading Velocity

To read velocity type "v" followed by a number 0-3 and press Enter. The numerical value represents the "Velocity Scaling" value. What this means is that the sensor can measure velocity in 4 different quantities over different amounts of time (see the table below):

Number Value | Measurement Period (msec) | Velocity Scaling (meters/sec)
:---| :---|:---
0 (default) | 100|0.1 m/s
1| 40|0.25 m/s
2|20|0.5 m/s
3|10|1.0 m/s

Based on your application the sensor can read more detailed readings over longer periods of time, or less detailed readings more quickly. As you experiment with different numerical values following the "v" you'll notice that the values fill the serial monitor more quickly or slowly depending. 

*NOTE: sometimes the text output between the "====" does not reflect the values you have entered (the response from the serial monitor can be slow). If this is the case you can simply type the command into the monitor again, or increase the baud rate in the Arduino program (and upload) and then adjust the baud rate on the serial monitor to reflect the change you made.*

![x](http://pulsedlight3d.net/assets.pl3d/ll-v3.jpg)

## Reading Average Velocity
To read average velocity type "w", followed by 0-3 for the velocity scaling value, and then followed by a value 2-9 representing the number of values to average. 

![x](http://pulsedlight3d.net/assets.pl3d/ll-w04.jpg)

## Reading Average Distance

To read average distance type "n", followed by a value 2-9 representing the number of values to average. 

![x](http://pulsedlight3d.net/assets.pl3d/ll-n8.jpg)




## References 

Learn more about LIDAR-Lite and PulsedLight:*
http://pulsedlight3d.com





