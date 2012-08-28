int firstLED = 9;
int lastLED = 13;
int currBlink = 9;
int home = 0;

void setup()
{
  // start with everything off
  for(int i=firstLED; i<=lastLED; i++)
  {
    pinMode(i, OUTPUT); 
    turnOffAll();
  }

}

void loop() 
{
  // read in test signal
  int sensorValue = analogRead(A0);
  // convert to 5 point scale
  int position = map(sensorValue, 0, 1023, 0, 6);

  updatePosition(position);

  // blink status light if not home
  if(!home)
  {
    // status light for blinking
    blinkLocation();
  }
}

void updatePosition(int p)
{
  turnOffAll();
  switch(p)
  {
  case 6:
    home = 1;
  case 5:
    digitalWrite(firstLED+4, HIGH);
  case 4:
    digitalWrite(firstLED+3, HIGH);
  case 3:
    digitalWrite(firstLED+2, HIGH);
  case 2:
    digitalWrite(firstLED+1, HIGH);
  case 1:
    digitalWrite(firstLED, HIGH);
  } 
  currBlink = firstLED + p - 1;
}

void blinkLocation()
{
  digitalWrite(currBlink, HIGH);
  delay(600);
  digitalWrite(currBlink, LOW);
  delay(600); 
}

void turnOffAll()
{
  for(int i=firstLED; i<=lastLED; i++)
  { 
    digitalWrite(i, LOW);
  } 
}





