
/*  Knock Three Times
 Becky Stewart
 July 2012
 
 Unipolar digital hall effect sensor is used to 
 determine when a magnet has come into close
 contact with the circuit 3 times within a 
 set time period.
 
 Circuit:
 SS443A Hall effect sensor, pin 1 Vcc, pin 2 ground, 
 10k resistor between pin 3 and Vcc with readings from sensor 
 on pin 3 of sensor read into pin 5 of Arduino
 
 
 */
const int pinIn = 10;
const unsigned long timeLimit = 5000;
const int triggerNum = 3;

unsigned long initDetect;
int currCount = 0;
int prevMagnetState = 1;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

void loop() {
  // read in value to determine if magnet is present
  // 0 is present, 1 is away
  int magnetAway = digitalRead(pinIn);                    

  // determine if magnet is near
  // and only is it recently came near
  if ( !magnetAway && magnetAway != prevMagnetState )
  {
    currCount++;

    // if this is the first detection of the magnet
    // note the time
    if ( currCount == 1 )
    {
      initDetect = millis(); 
    }
  }
  // if it's been longer than the time limit
  // reset the counter
  if ( currCount > 0 && (millis()-initDetect) > timeLimit )
  {
    currCount = 0; 
    Serial.println("resetting counter");
  }

  if ( currCount >= triggerNum && (timeLimit-initDetect) )
  {
    // print the results to the serial monitor:
    Serial.println("magnet is near " );  
    currCount = 0;
  } 

  prevMagnetState = magnetAway;

  // wait 2 milliseconds before the next loop
  // for the analog-to-digital converter to settle
  // after the last reading:
  delay(2);                     
}



