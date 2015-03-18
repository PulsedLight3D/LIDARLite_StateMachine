/* 

CONTINUOUS READ STATE MACHINE FOR LIDAR-LITE

This sketch demonstrates distance, raw velocity and smoothed velocity readings.
It also demonstrates the velocity scaling feature. 

USAGE: 

'm' = read distance
'n' + '2-9' = read average distance
'v' + '0-3' = read raw velocity and set scaling register 
'w' + '0-3' + '2-9' = read average velocity and set scaling register 
Type any other key to stop

Examples:
- 'm' will read distance
- 'n2' will read average distance for two readings
- "v0" will read velocity at 0.1m/s
- 'w34' will read velocity at 1m/s and average 4 readings

------------------------------------------

More information about velocity scaling is available in the LIDAR-Lite Operating
Manual on page 22: 
http://pulsedlight3d.com/pl3d/wp-content/uploads/2014/10/LIDAR-Lite-Operating-Manual-PRELIM.pdf

It utilizes the 'Arduino I2C Master Library' from DSS Circuits:
http://www.dsscircuits.com/index.php/articles/66-arduino-i2c-master-library 

You can find more information about installing libraries here:
http://arduino.cc/en/Guide/Libraries


*/

#include <I2C.h>

// Global Variables
char LIDARLite_ADDRESS = 0x62; // LIDAR-Lite I2C Address


void setup(){
  Serial.begin(9600); //Opens serial connection at 9600 baud.     
  I2c.begin(); // Opens & joins the irc bus as master
  delay(100); // Waits to make sure everything is powered up before sending or receiving data  
  I2c.timeOut(50); // Sets a timeout to ensure no locking up of sketch if I2C communication fails
}

void loop(){
  smRunStateMachine(); // Run the State Machine that controls actions based on user input
}




/* ==========================================================================================================================================
Basic read and write functions for LIDAR-Lite, waits for success message (0 or ACK) before proceeding
=============================================================================================================================================*/

// Write a register and wait until it responds with success
void llWriteAndWait(char myAddress, char myValue){
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.write(LIDARLite_ADDRESS,myAddress, myValue); // Write to LIDAR-Lite Address with Value
    delay(2); // Wait 2 ms to prevent overpolling
  }
}

// Read 1-2 bytes from a register and wait until it responds with sucess
byte llReadAndWait(char myAddress, int numOfBytes, byte arrayToSave[2]){
  uint8_t nackack = 100; // Setup variable to hold ACK/NACK resopnses     
  while (nackack != 0){ // While NACK keep going (i.e. continue polling until sucess message (ACK) is received )
    nackack = I2c.read(LIDARLite_ADDRESS,myAddress, numOfBytes, arrayToSave); // Read 1-2 Bytes from LIDAR-Lite Address and store in array
    delay(2); // Wait 2 ms to prevent overpolling
  }
  return arrayToSave[2]; // Return array for use in other functions
}



/* ==========================================================================================================================================
Get 2-byte distance from sensor and combine into single 16-bit int
=============================================================================================================================================*/

int llGetDistance(){
  llWriteAndWait(0x00,0x04); // Write 0x04 to register 0x00 to start getting distance readings
  byte myArray[2]; // array to store bytes from read function
  llReadAndWait(0x8f,2,myArray); // Read 2 bytes from 0x8f
  int distance = (myArray[0] << 8) + myArray[1];  // Shift high byte [0] 8 to the left and add low byte [1] to create 16-bit int
  return(distance);
}

/* ==========================================================================================================================================
Get raw velocity readings from sensor and convert to signed int
=============================================================================================================================================*/

int llGetVelocity(){
  llWriteAndWait(0x00,0x04); // Write 0x04 to register 0x00 to start getting distance readings
  llWriteAndWait(0x04,0x80); // Write 0x80 to 0x04 to switch on velocity mode 
  byte myArray[1]; // Array to store bytes from read function
  llReadAndWait(0x09,1,myArray); // Read 1 byte from register 0x09 to get velocity measurement 
  return((int)((char)myArray[0])); // Convert 1 byte to char and then to int to get signed int value for velocity measurement
}


/* ==========================================================================================================================================
Average readings from velocity and distance
int numberOfReadings - the number of readings you want to average (0-9 are possible, 2-9 are reccomended)
=============================================================================================================================================*/


int llGetDistanceAverage(int numberOfReadings){ 
  if(numberOfReadings < 2){
    numberOfReadings = 2; // If the number of readings to be taken is less than 2, default to 2 readings
  }
  int sum = 0; // Variable to store sum
  for(int i = 0; i < numberOfReadings; i++){ 
      sum = sum + llGetDistance(); // Add up all of the readings
  }
  sum = sum/numberOfReadings; // Divide the total by the number of readings to get the average
  return(sum);
}

int llGetVelocityAverage(int numberOfReadings){ 
  int sum = 0; // Variable to store sum
  for(int i = 0; i < numberOfReadings; i++){ 
      sum = sum + llGetVelocity(); // Add up all of the readings
  }
  sum = sum/numberOfReadings; // Divide the total by the number of readings to get the average
  return(sum);
}


/* ==========================================================================================================================================
SET VELOCITY SCALING VALUES AND RESET REGISTERS BEFORE READING DISTANCE
=============================================================================================================================================*/

void llConfigureRegisters(char myFunction, int velocityScaling, int numberOfReadings){
  unsigned char setScalingValue[] = {0xC8,0x50,0x28,0x14}; // Array of velocity scaling values, see "Velocity Measurment" in operating manual for details
  if(myFunction == 'v' || myFunction == 'w'){
    if(velocityScaling < 4){
      llWriteAndWait(0x68,setScalingValue[velocityScaling]); // Set scaling value based on scaling choice      
    }else{
        llWriteAndWait(0x68,setScalingValue[0]); // If scaling choice is out of array range, use default scaling
    }
  }else if(myFunction == 'm' || myFunction == 'n'){
    llWriteAndWait(0x00,0x00); // reset device to defaults for distance measurment
  }
  smConfigureRegistersPrintStatements(myFunction, velocityScaling, numberOfReadings);
}

/* ==========================================================================================================================================
SERIAL INTERACTION AND DISPLAY FUNCTIONS
=============================================================================================================================================*/

void smPrintFunctionInit(String myString){
  Serial.println("\n=======================================================");
  Serial.println(myString);
  Serial.println("=======================================================\n");
}

String smPrintUserManaual(){
  String myString = "Continuous read state machine for LIDAR-Lite User Manual\n\nUSAGE: \n'm' = read distance\n'n' + '2-9' = read average distance\n'v' + '0-3' = read raw velocity and set scaling register\n'w' + '0-3' + '2-9' = read average velocity and set scaling register \nType any other key to stop\n\nExamples:\n- 'm' will read distance\n- 'n2' will read average distance for two readings\n- 'v0' will read velocity at 0.1m/s\n- 'w34' will read velocity at 1m/s and average 4 readings";
  return myString;
}

// SET VELOCITY SCALING PRINT OUTPUT

void smConfigureRegistersPrintStatements(char myFunction, int velocityScaling, int numberOfReadings){
  String myString;
  if (myFunction == 'v'){myString = "Reading Raw Velocity";}
  else if (myFunction == 'w'){myString = "Reading Velocity Smooth";}
  else if(myFunction == 'm'){myString = "Reading Raw Distance";}
  else if(myFunction == 'n'){myString = "Reading Smooth Distance";}
  if(myFunction == 'v' || myFunction == 'w'){
    if (velocityScaling == 1){
      myString += " @ 0.25 m/s";
    }else if (velocityScaling == 2){
      myString += " @ 0.5 m/s";      
    }else if (velocityScaling == 3){
      myString += " @ 1 m/s";      
    }else{
      myString += " @ 0.1 m/s";      
    }
  }
  if(numberOfReadings != 0){
    myString += " averaging ";
    myString += int(numberOfReadings);
    myString += " readings";
  }
  smPrintFunctionInit(myString);  
}

// GLOBAL VARIABLES FOR STATE MACHINE
int configureFlag = 0;
int i = 0;
char serialArray[3];
char serialRead;

// The state machine
void smRunStateMachine(){
  if(Serial.available() > 0){
    while(Serial.available() > 0){
      if(i==0){
        configureFlag = 0;
      }
      serialRead = Serial.read();
      if(serialRead == 10 || serialRead == 32 ){
      }else{
        serialArray[i] = serialRead;
        i++;  
      }
    }
  }else{
    i = 0;
  }

switch(serialArray[0]){
  case 'v': case 'V':
    if (configureFlag == 0){
      llConfigureRegisters(serialArray[0],int(serialArray[1])-48,0);
      configureFlag++;
    }      
    Serial.println(llGetVelocity());
  break;
  case 'w':case 'W':
    if (configureFlag == 0){
      char scaleChar = Serial.read();
      llConfigureRegisters(serialArray[0],int(serialArray[1])-48,int(serialArray[2])-48);
      configureFlag++;       
    }
    Serial.println(llGetVelocityAverage(int(serialArray[2])-48));
  break;
  case 'm':case 'M':
    if (configureFlag == 0){
      llConfigureRegisters(serialArray[0],0,0); 
      configureFlag++;    
    }
    Serial.println(llGetDistance());
  break;
  case 'n':case 'N':
  if (configureFlag == 0){
      llConfigureRegisters(serialArray[0],0,int(serialArray[1])-48); 
      configureFlag++;    
    }
    Serial.println(llGetDistanceAverage(int(serialArray[1])-48));
  default:
  if (configureFlag == 0){
      smPrintFunctionInit(smPrintUserManaual());
      configureFlag++;    
    }
  }
}
