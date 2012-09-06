int leds[] = {
  9, 7, 6, 5, 4};
int numLEDs = 5;
int currBlink = 0;
int prevPos = 0;

// home green LED is 4
// first red is 8

void setup()
{
  // start with everything off
  for(int i=0; i<=numLEDs; i++)
  {
    pinMode(leds[i], OUTPUT); 
    digitalWrite(leds[i], LOW);
  }

  Serial.begin(9600);
}

int j =0;
int fadeValue = 0;
int fadeDirection = 5;
void loop() 
{
  // read in test signal
  int sensorValue = analogRead(A3);
  // convert to 5 point scale
  int pos = map(sensorValue, 0, 1023, 0, 6);
  Serial.println(pos);
  
  // simulate distance to destination
  if(pos > 0) {
    if(pos < 6){
    for(int i=0; i<=numLEDs-1; i++)
    {
      digitalWrite(leds[i], LOW);
    }

    for(int i=0; i<pos-1; i++){
      digitalWrite(leds[i], HIGH);
    }
    digitalWrite(leds[pos-1], HIGH);
    delay(300);
    digitalWrite(leds[pos-1], LOW);
    delay(300);
    j++;
    if(j>50) j=0;
    }
    else {
      for(int i=0; i<=numLEDs-1; i++)
    {
      digitalWrite(leds[i], HIGH);
    }
    }
  } 
  // simulate standby
  else {
    // fade centre from min to max in increments of 5 points:
    analogWrite(leds[0], fadeValue);  

    fadeValue += fadeDirection;
    if(fadeValue > 250 || fadeValue < 5)
      fadeDirection = -1*fadeDirection;  


    delay(30);
  }

}




