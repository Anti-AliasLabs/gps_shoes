int centre = 10;
int leds[] = {
  8, 9, 6, 2, 5, 4, 7, 3};

/*
  N  8
 NE 9
 E  6
 SE 2
 S  5
 SW 4
 W  7
 NW 3
 */

void setup() {
  for(int i=0; i<8; i++)
  {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }
  pinMode(centre, OUTPUT);
  digitalWrite(centre, HIGH);

  Serial.begin(4800);
}


int j=0, k=0;
int fadeValue = 0;
int fadeDirection = 2;

void loop() {
  int potValue = analogRead(A3);

  int pos = map(potValue, 0, 1023, 0, 9);
  
  Serial.println(pos);

  // turn off ring lights lights
  for(int i=0; i<8; i++)
  {
    digitalWrite(leds[i], LOW);
  }
  
  // show direction display
  if(pos < 8)
  {
    digitalWrite(leds[(pos+3)%8], HIGH);
  } 
  // show no GPS signal display
  else {

    digitalWrite(leds[j], HIGH);
    k++;
    if(k>30){
      k=0;
      j++;
    }

    if(j>7) j = 0;
  }
  
  // fade centre from min to max in increments of 5 points:
  analogWrite(centre, fadeValue);  

  fadeValue += fadeDirection;
  if(fadeValue > 253 || fadeValue < 2)
    fadeDirection = -1*fadeDirection;  


  delay(30);

}






