#include <PinChangeInt.h>

#define RUPTPIN 5

const unsigned long timeLimit = 5000;
const int triggerNum = 8;

unsigned long initDetect;
int currCount = 0;

uint8_t latest_interrupted_pin;

void knock() {
  latest_interrupted_pin = PCintPort::arduinoPin;
  // if interrupt was on pin we're watching
  if (latest_interrupted_pin == RUPTPIN){
    currCount++;

    // if this is the first knock
    // note the time
    if ( currCount == 1 )
    {
      initDetect = millis(); 
    }
  }
}

void setup() {
  pinMode(RUPTPIN, INPUT); 
  digitalWrite(RUPTPIN, HIGH);
  PCintPort::attachInterrupt(RUPTPIN, &knock, RISING);
  Serial.begin(9600);
  Serial.println("---------------------------------------");
}


void loop() {

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
    Serial.print("knock knock " );  
    Serial.println(currCount);
    currCount = 0;
  } 
}




