/*
LIDAR-LITE DSS I2C LIBRARY STATE MACHINE

This sketch demonstrates the various abilities of the 
LIDAR-Lite Sensor using a simple State Machine. 

It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 

You can find more information about installing libraries here:
http://arduino.cc/en/Guide/Libraries

NOTE: Be sure to set 'Newline' in drop down box at the bottom of the monitor
window instead of the default 'No line ending

*/

// ************************************Version****************************************
#define    SKETCHVERSION      "0.1"	// Version number of sketch...is printed in debug mode.

// ***********************************Includes****************************************
#include <I2C.h>	// Faster & Smaller Library without the I2C timeout bug. [Replaces <Wire.h>] http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library

// **********************************Definitions**************************************
#define    MULTIPLEXER_ADDRESS 0x70			// Default I2C Address of Multiplexer.
#define    LIDARLite_ADDRESS   0x62			// Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00			// Register to write to initiate ranging.
#define    MeasureValue        0x04			// Value to initiate ranging.
#define    RegisterStatus      0x01			// Register for status - Control Register #1: [0:Busy, 1:Ref Overflow, 2:Sig Overflow, 3:Signal Not Valid, 4:Secondary Return, 5:Velocity Complete, 6:External Trigger Complete, 7:Eye Safe]
#define    RegisterHighLowB    0x8f			// Register to get both High and Low bytes in 1 call.
#define    RegisterModes       0x04			// Register for Modes.
#define    RegisterModeQ       B01111010	// Register value to turn off Correlation: 01111010
#define    RegisterUpperByte   0x0f			// Register for upper 8 bits of ranging data.
#define    RegisterLowerByte   0x10			// Register for lower 8 bits of ranging data.
#define    SensorDisabledVal   205

// ***********************************Variables***************************************

byte        vHighByte = 0x00;		// Stores high byte from ranging.
byte        vLowByte = 0x00;		// Stored low byte from ranging.
byte        statusByte = 0x00;		// Stores Device Status.
boolean     s1ON = true;			// Stores the state of Sensor 1 on the I2C bus.
boolean     i2cON = true;			// Stores the state of the I2C bus.
byte        i2cBuffer[1];

// ******************************Structures and Objects*******************************
struct Lidar_Readings{  // Structure to Store Readings.
	int              sensor_1_read;
	unsigned int     sensor_1_readtime;
	int              sensor_2_read;
	unsigned int     sensor_2_readtime;
} lidar_reads;

// ******************************State Machine_Variables*******************************

int n = 0;
unsigned int integerValue=0;	// The interger being created via serial read, Max value is 65535
char incomingByte;				// The byte currently being read by State Machine
uint8_t letterValue = 0;		// The letter for the command	
unsigned int intOne = 0;		// The first integer in the command
unsigned int intTwo = 0;		// The second integer in the command
int runStateMachine = 0;		// If flag = 1, run StateMachine

// *********************************Arduino Methods***********************************
void setup(){  
	Serial.begin(115200); 	// Opens serial connection at 9600 baud.
	I2c.begin();			// Opens & Joins the i2c bus as master.
	delay(100); 			// Waits to make sure everything is powered up before sending or receiving data
	I2c.timeOut(50);		// Sets a timeout to ensure no 'locking-up' of sketch if I2C communication fails.
	printUserManual();
}

void loop(){
	if (Serial.available() > 0)
	{
		while(1) {  // force into a loop until 'n' is received
			incomingByte = Serial.read();
			if(incomingByte == 10)
			{
				n++; 
				runStateMachine = 1;
			}
			else if(incomingByte == 32)
			{
     			n++;
			}
			else if (incomingByte > 57)
			{
				letterValue = incomingByte;
     		}
     		else
     		{
				if(incomingByte > 47)
				{
					if(incomingByte < 58)
					{
						integerValue *= 10;  // shift left 1 decimal place
						// convert ASCII to integer, add, and shift left 1 decimal place
						integerValue = ((incomingByte - 48) + integerValue);
					}
				}
			}
    		if (incomingByte == -1)continue;  // if no characters are in the buffer read() returns -1 
			if( n > 0 )
			{
				if(integerValue > 0)
				{
					if(intOne == 0)
					{
						intOne = integerValue;
					}
					else if(intTwo == 0)
					{
						intTwo = integerValue;
					}              
					integerValue = 0;              
				}
				n=0; 
			}
			if(runStateMachine == 1)
			{
				stateMachine(letterValue,intOne,intTwo);
			}
		}
	} 
}
void stateMachine(uint8_t switchCaseVar,int firstValue, int secondValue){
	letterValue = 0;
	intOne = 0;
	intTwo = 0;
	n=0;  
	runStateMachine = 0;
	switch(switchCaseVar){
		case 'm': case 'M':  
			Serial.print(F("// AQUIRE "));
			Serial.print(firstValue);
			Serial.println(F(" DISTANCE READINGS"));                        
			for(int i = 0; i < firstValue; i++)
			{
				getRange();  // Calls a function to get range in cm from all Sensors.
				Serial.print(F("Sensor reads: "));
				Serial.print(lidar_reads.sensor_1_read);
				Serial.print(F("cm at "));
				Serial.println(lidar_reads.sensor_1_readtime);
			}
			Serial.println(F("// END \n"));
			break;
		case 's': case 'S': // Run a Single Reading.
			Serial.println(F("// AQUIRE SINGLE DISTANCE READING"));
			getRange();  // Calls a function to get range in cm from all Sensors.
			Serial.print(F("Byte Readings: "));
			Serial.print(F("H: "));
			Serial.print(vHighByte);
			Serial.print(F("  L: "));
			Serial.print(vLowByte);
			Serial.print(F(" / DISTANCE READINGS: "));
			Serial.print(lidar_reads.sensor_1_read);
			Serial.print(F("cm at "));
			Serial.println(lidar_reads.sensor_1_readtime); 	
			Serial.println(F(""));
			break;
		case 'i': case 'I':
			runSensorInit();
			break;
		case 'r': case 'R':
			Serial.print(F("// READ REGISTER "));
			Serial.println(firstValue);
			if(s1ON)
			{
				readAndPrintRegisterByte(firstValue);
			}
			else
			{
				Serial.println("// ERROR: Please initialize sensor with 'i'");
			}
			Serial.println(F(""));
			break; 
		case 'w': case 'W':
			Serial.print(F("// WRITE "));
			Serial.print(secondValue);
			Serial.print(F(" TO REGISTER "));
			Serial.println(firstValue);
			if(s1ON)
			{
				uint8_t firstOne = uint8_t(firstValue);
				uint8_t secondOne = uint8_t(secondValue);
				writeAndPrintRegisterByte(firstOne,secondOne);
			}else{
				Serial.println("// ERROR: Please initialize sensor with 'i'");
			}
			Serial.println(F(""));
			break;
		default:  
			Serial.write(switchCaseVar);
			Serial.print('\t');
			Serial.print(firstValue);
			Serial.print('\t');
			Serial.println(secondValue);       
	}

}

// *******************************Private Functions***********************************
void readAndPrintRegisterByte(uint8_t reg)
{
	while (I2c.read((uint8_t)LIDARLite_ADDRESS, reg ,(uint8_t)1) != 0); // Request 1 byte from LIDAR-Lite 
	while (I2c.available() < 1);  // Wait for data
	byte myByte = I2c.receive();  // Get Data
	Serial.print(F("Register "));
	Serial.print(reg);
	Serial.print(F(" : "));
	Serial.print(bitRead(myByte,7));
	Serial.print(bitRead(myByte,6));
	Serial.print(bitRead(myByte,5));
	Serial.print(bitRead(myByte,4));
	Serial.print(bitRead(myByte,3));
	Serial.print(bitRead(myByte,2));
	Serial.print(bitRead(myByte,1));
	Serial.print(bitRead(myByte,0));
	Serial.print(", 0x");
	Serial.println(myByte, HEX);
}
void writeAndPrintRegisterByte(uint8_t reg, uint8_t regByte){
	I2c.write((uint8_t)LIDARLite_ADDRESS,(uint8_t)reg,(uint8_t)regByte);
	Serial.print("Write successful: ");
	readAndPrintRegisterByte(reg);
}

// ***************This function gets/stores range from the LIDAR-Lite*****************
void getRange(){
	int o = 100;
	if (s1ON)
	{
		o = 100;
		while (o != 0)
		{
			o =
			I2c.write((uint8_t)LIDARLite_ADDRESS,
				(uint8_t)RegisterMeasure,       // Register to initiate ranging
				(uint8_t)MeasureValue);         // Value to initiate ranging
			if (o != 0)
			{
				delay(2);
			}
		}

		delay(4);  // Delay for quickest read time
		lidar_reads.sensor_1_readtime = millis(); 	// Store Timestamp

		//// *** START OF TWO BYTE READ CODE ***
		byte HLByte[2];
		o = 100;
		while (o != 0)
		{
			// Call the register for both high and Low byte
			o = I2c.read((uint8_t)LIDARLite_ADDRESS,(uint8_t)RegisterHighLowB, 2,HLByte);
			if (o != 0)
			{
				delay(2);
			}
		}
		vHighByte = HLByte[0]; // Get high byte
		vLowByte = HLByte[1]; // Get low byte
		//// *** END OF TWO BYTE READ CODE

		// Store Range after putting High/Low byte together.
		lidar_reads.sensor_1_read = (vHighByte << 8) + vLowByte;
	}
	if (!s1ON)
	{
		// If sensor 1 isn't ON we set to a value we watch for in the code...
		lidar_reads.sensor_1_read = SensorDisabledVal; 
	}
}
// **********************This function checks i2c bus for devices***********************
void runSensorInit()
{
	byte tmpData[1];
	// Check Sensor 1:
	if(I2c.read(LIDARLite_ADDRESS, RegisterStatus, 1, tmpData) == 0)
	{
		s1ON = true;
		Serial.println("// SENSOR INITIALIZED \n");
	} 
	else 
	{
		s1ON = false;
		//Put starter values in Struct since sensor is disabled.
		lidar_reads.sensor_1_readtime = 0;
		lidar_reads.sensor_1_read = SensorDisabledVal;
	}
	if(!s1ON)
	{
		I2c.end();
	i2cON = false;
	} 
	else 
	{
		if (!i2cON){
			I2c.begin();
			i2cON = true;
		}
	}
}

void printUserManual(){
	Serial.println(F("\n*********LIDAR-LITE STATE MACHINE USER MANUAL*********"));
	Serial.println(F("================================================================================"));
	Serial.println(F("COMMAND   => DESCRIPTION"));
	Serial.println(F("'i'       => Initilize Sensor"));
	Serial.println(F("'s'       => Take single reading"));
	Serial.println(F("'r XX'    => Read register, ex. 'r 12' reads register 12"));
	Serial.println(F("'w XX XX' => Write to register, ex. 'w 18 19' writes '19' to register '18'"));
	Serial.println(F(""));
	Serial.println(F("NOTES:"));
	Serial.println(F("- Be sure to set 'Newline' in drop down box at the bottom of the monitor"));
	Serial.println(F("  window instead of the default 'No line ending'"));	
	Serial.println(F("================================================================================"));
}
