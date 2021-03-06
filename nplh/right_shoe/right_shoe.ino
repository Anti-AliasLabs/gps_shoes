// receiver
//
// Receiver for right shoe of GPS shoes
// rf receiver in pin 11

#include <VirtualWire.h>

// first LED with PWM fading needs to be in 3 (or same port)
int leds[] = {
  3, 17, 16, 15, 14};
int numLEDs = 5;
int currBlink = 0;

int j =0;
int fadeValue = 0;
int fadeDirection = 5;

void setup()
{
    Serial.begin(9600);	// Debugging only
    Serial.println("setup");
    
    for( int i=0; i<numLEDs; i++)
    {
      pinMode(leds[i], OUTPUT);
    }

    // Initialise the IO and ISR
    vw_set_ptt_inverted(true); // Required for DR3100
    vw_setup(2000);	 // Bits per sec

    vw_rx_start();       // Start the receiver PLL running
}

void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

    if (vw_get_message(buf, &buflen)) // Non-blocking
    {
	int i;

        digitalWrite(13, true); // Flash a light to show received good message
	// Message with a good checksum received, dump it.
	Serial.print("Got: ");
	
	for (i = 0; i < buflen; i++)
	{
            currBlink = buf[i];
            Serial.print("received: ");
	    Serial.println(currBlink);
	}

        digitalWrite(13, false);
    }
    
    distance_lights(currBlink);
    
    //Serial.println("test");
    //delay(1000);
}


void distance_lights(int pos)
{ 
  //Serial.print("blinking position: ");
  //Serial.println(pos);
  //Serial.print(" ");
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


    delay(50);
    //Serial.println(fadeValue);
  }
}
