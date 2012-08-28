/* 
 No Place Like Home
 Becky Stewart
 Codasign
 becky@codasign.com
 July 2012
 
 Serial port reads in GPS data (via XBee) and writes out
 instructions to remote XBee controlling LEDS.
 
 Serial on 0 and 1
 LEDs 
 Reed switch on 10
 Compass on A4 and A5
 
 
 */

#include <TinyGPS.h>
#include <Wire.h>
#include <PinChangeInt.h>

const int heelPin = 5;

const unsigned long timeLimit = 5000;
const int triggerNum = 1;


int ledPins[] = {
  6, 7, 8, 9};

int ledsLen = 4;

unsigned long initDetect;
int currCount = 0;

int goHome = 0;

int HMC6352SlaveAddress = 0x42;
int HMC6352ReadAddress = 0x41; //"A" in hex, A command is: 

int headingValue;
float startLat = 0.0;
float startLon = 0.0;

uint8_t latest_interrupted_pin;

TinyGPS gps;

static const float LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

//---------------------------------------------------------------------------------
// setup - called once at startup
//---------------------------------------------------------------------------------
void setup()
{
  Serial.begin(57600);

  // "The Wire library uses 7 bit addresses throughout. 
  //If you have a datasheet or sample code that uses 8 bit address, 
  //you'll want to drop the low bit (i.e. shift the value one bit to the right), 
  //yielding an address between 0 and 127."
  HMC6352SlaveAddress = HMC6352SlaveAddress >> 1; // I know 0x42 is less than 127, but this is still required
  Wire.begin();

  // set up interrupt for heel clicks
  pinMode(heelPin, INPUT); 
  digitalWrite(heelPin, HIGH);
  PCintPort::attachInterrupt(heelPin, &heelClicked, RISING);

  // set up LED display
  for (int i=0; i<ledsLen; i++){
    pinMode(ledPins[i], OUTPUT); 
  }
}

//---------------------------------------------------------------------------------
// loop - called continuously while powered
//---------------------------------------------------------------------------------
void loop()
{
  bool newdata = false;
  unsigned long start = millis();

  // if it's been longer than the time limit
  // reset the counter
  if ( currCount > 0 && (millis()-initDetect) > timeLimit )
  {
    currCount = 0; 
    Serial.println("resetting counter");
  }

  // heels have been clicked
  if ( currCount >= triggerNum && (timeLimit-initDetect) )
  {
    // print the results to the serial monitor:
    Serial.print("knock knock " );  
    Serial.println(currCount);
    currCount = 0;
    goHome = 1;
  }
  if ( goHome ){
    // Every second we print an update
    while (millis() - start < 1000)
    {
      if (feedgps())
        newdata = true;
    }
    updateGPS(gps);
  }
  /*else {
    spinLights();
  }*/
}

//---------------------------------------------------------------------------------
// gpsdump - reads in GPS data
//---------------------------------------------------------------------------------
static void updateGPS(TinyGPS &gps)
{
  float flat, flon;
  unsigned long age, date, time, chars = 0;
  unsigned short sentences = 0, failed = 0;
  unsigned long invalid = 0;


  gps.f_get_position(&flat, &flon, &age);

  // if GPS isn't receiving a signal, just flash lights
  if(TinyGPS::GPS_INVALID_F_ANGLE == flat || TinyGPS::GPS_INVALID_F_ANGLE == flon) {
    Serial.println("invalid position"); 
    spinLights();
  }
  // if receiving a signal, go ahead and compute location and distance
  else{
    Serial.print("Current Latitude: ");
    Serial.print(flat);
    Serial.print(" Current Longitude: ");
    Serial.print(flon);

    // keep track of where we are once we've started 
    // navigating home (to keep track of distance travelled
    // once we start going home)
    if ( goHome && startLat == 0.0 && startLon == 0.0) {
      startLat = flat;
      startLon = flon;
    }

    // get current course
    const char *str = TinyGPS::cardinal(gps.f_course());
    updateLightsHeading(str);
    //updateLightsHeading( readCompass() );
    
    int dist_so_far = TinyGPS::distance_between(startLat, startLon, flat, flon) / 1000;
    int dist_to_go = TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000;
    int course_to_home = TinyGPS::course_to(flat, flon, 51.508131, -0.128002);
    
    Serial.print(" Current Course: ");
    Serial.print(" ");
    Serial.print(gps.f_course());
    Serial.print(" ");
    Serial.println(str);
    
    Serial.print(" So far travelled ");
    Serial.print(dist_so_far);
    Serial.print(" To go ");
    Serial.print(dist_to_go);
    Serial.println(course_to_home);
    
  }
}


//---------------------------------------------------------------------------------
// readCompass - returns the current heading in degrees
//---------------------------------------------------------------------------------
int readCompass()
{
  //"Get Data. Compensate and Calculate New Heading"
  Wire.beginTransmission(HMC6352SlaveAddress);
  Wire.write(HMC6352ReadAddress);              // The "Get Data" command
  Wire.endTransmission();
  //time delays required by HMC6352 upon receipt of the command
  //Get Data. Compensate and Calculate New Heading : 6ms
  delay(6);

  Wire.requestFrom(HMC6352SlaveAddress, 2); //get the two data bytes, MSB and LSB

  //"The heading output data will be the value in tenths of degrees
  //from zero to 3599 and provided in binary format over the two bytes."
  byte MSB = Wire.read();
  byte LSB = Wire.read();

  float headingSum = (MSB << 8) + LSB; //(MSB / LSB sum)
  float headingInt = headingSum / 10; 

  Serial.print(headingInt);
  Serial.println(" degrees");

  //delay(100);
  return headingInt;
}


//---------------------------------------------------------------------------------
// feedgps - returns true if GPS data available
//---------------------------------------------------------------------------------
static bool feedgps()
{
  while (Serial.available())
  {
    if (gps.encode(Serial.read()))
      return true;
  }
  return false;
}

//---------------------------------------------------------------------------------
// heelClicked - increments counter for heel clicks
//---------------------------------------------------------------------------------
void heelClicked() {
  latest_interrupted_pin = PCintPort::arduinoPin;
  // if interrupt was on pin we're watching
  if (latest_interrupted_pin == heelPin){
    currCount++;

    // if this is the first knock
    // note the time
    if ( currCount == 1 )
    {
      initDetect = millis(); 
    }
  }
}


//---------------------------------------------------------------------------------
// spinLights - spinning light pattern
//---------------------------------------------------------------------------------
void spinLights()
{
  for (int i=0; i<ledsLen; i++){
    digitalWrite(ledPins[i], HIGH); 
    delay(300);
    digitalWrite(ledPins[i], LOW); 
  }
}

//---------------------------------------------------------------------------------
// updateLightsHeading - update light indicating north
//---------------------------------------------------------------------------------
/*void updateLightsHeading(int heading)
 {
 int w = 180/ledsLen;
 int ledOn;
 
 if ( heading > (360-w) || heading < w ){
 ledOn = 0;
 }
 else {
 float f_ledOn = float(heading-w)/(360.0-w) * ledsLen;
 ledOn = round(f_ledOn);
 }
 
 //int ledOn = float(heading)/360.0 * ledsLen;
 Serial.println(ledOn);
 
 for (int i=0; i<ledsLen; i++){
 digitalWrite(ledPins[i], LOW); 
 }
 digitalWrite(ledPins[ledOn], HIGH);
 }*/
//---------------------------------------------------------------------------------
// updateLightsHeading - update light indicating north
//---------------------------------------------------------------------------------
void updateLightsHeading(const char *headingString)
{
  String  N = "N";
  String  NE = "NE";
  String  E = "E";

  if (N.equals(headingString))
  {
    Serial.println("-----north");
  }
  if (NE.equals(headingString))
  {
    Serial.println("-----north");
  }
  if (E.equals(headingString))
  {
    Serial.println("-----north");
  }

}





