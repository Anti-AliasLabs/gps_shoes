#include <EEPROMex.h>

#include "Arduino.h"

int addressLat;
int addressLon;

String inputString = "";
boolean stringComplete = false;

float homeLat;
float homeLon;

void setup() {
  Serial.begin( 9600 );

  inputString.reserve( 200 );

  addressLat = EEPROM.getAddress( sizeof( long ) );
  addressLon = EEPROM.getAddress( sizeof( long ) *2 );

  //writeLongitude( 361234 );
  //writeLatitude( 934321 );

  homeLon = readLongitude();
  homeLat = readLatitude();

}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    if( inputString == "connected\n") {
      Serial.println("shoe"); 
    }
    else {
      parseUpdatedLocation();
      Serial.println(homeLat);
      Serial.println(homeLon);
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    } 
  }
}

void parseUpdatedLocation() {
  int comma = inputString.indexOf( ',' );

  if( comma > 0 ) {
    String tempLat = inputString.substring( 0, comma );
    String tempLon = inputString.substring( comma+1, inputString.length()-1 ); 
    Serial.println(tempLat);
    Serial.println(tempLon);
    
    char latChar[8];
    tempLat.toCharArray(latChar, tempLat.length());   

    unsigned long tempLatInt = atol(latChar);
    int tempLonInt = tempLon.toInt();
    
    Serial.println(tempLatInt);
    Serial.println(tempLonInt);

    writeLatitude( tempLatInt );
    writeLongitude( tempLonInt );

    homeLat = readLatitude();
    homeLon = readLongitude();
  }
}

float readLongitude() {
  return  EEPROM.readInt( addressLon )/ 100000.0;
}

float readLatitude() {
  return EEPROM.readLong( addressLat ) /100000.0;
}

void writeLongitude( int ln ) {
  EEPROM.writeInt( addressLon, ln );
}

void writeLatitude( int lt ) {
  EEPROM.writeLong( addressLat, lt );
}




