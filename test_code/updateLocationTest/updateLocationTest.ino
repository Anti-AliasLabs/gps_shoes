#include <EEPROMex.h>

#include "Arduino.h"

int addressLat;
int addressLon;
int addressLatSign;
int addressLonSign;

String inputString = "";
boolean stringComplete = false;

float homeLat;
float homeLon;

void setup() {
  Serial.begin( 9600 );

  inputString.reserve( 200 );

  addressLat = EEPROM.getAddress( sizeof( long ) );
  addressLon = EEPROM.getAddress( sizeof( long ) );
  addressLatSign = EEPROM.getAddress( sizeof( int ) );
  addressLonSign = EEPROM.getAddress( sizeof( int )  );

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
    long tempLatInt;
    int latSign;
    long tempLonInt;
    int lonSign;

    // latitude
    char latChar[5];
    if( inputString[0] == '-') {
      String tempLat = inputString.substring( 1, comma );
      Serial.println("negative latitude"); 
      tempLat.toCharArray(latChar, tempLat.length() );
    } 
    else {
      String tempLat = inputString.substring( 0, comma );
      tempLat.toCharArray(latChar, tempLat.length() );  
      tempLatInt = atol(latChar);
    }

    // longitude
    char lonChar[5];
    if (inputString[comma+1] == '-') {
      String tempLon = inputString.substring( comma+2, inputString.length()-1 ); 
      tempLon.toCharArray(lonChar, tempLon.length() );
      tempLonInt = atol(lonChar);
      tempLonInt = tempLonInt;
      lonSign = 2;
    } 
    else {
      String tempLon = inputString.substring( comma+1, inputString.length()-1 ); 
      tempLon.toCharArray(lonChar, tempLon.length() );
      tempLonInt = atol(lonChar);
      lonSign = 1;
    }


    Serial.println(tempLatInt);
    Serial.println(tempLonInt);

    writeLatitude( tempLatInt );
    writeLongitude( tempLonInt, lonSign );

    homeLat = readLatitude();
    homeLon = readLongitude();
  }
}

float readLongitude() {
  float rLon =  EEPROM.readLong( addressLon )/ 100.0;
  int rSign = EEPROM.readInt( addressLonSign );
  if ( rSign == 2 )
    rLon = rLon * -1;
  return rLon;
}

float readLatitude() {
  return EEPROM.readLong( addressLat ) /100.0;
}

void writeLongitude( long ln, int sn ) {
  EEPROM.writeLong( addressLon, ln );
  EEPROM.writeInt( addressLonSign, sn );
}

void writeLatitude( long lt ) {
  EEPROM.writeLong( addressLat, lt );
}







