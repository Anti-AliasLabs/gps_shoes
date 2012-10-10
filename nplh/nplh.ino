

/* 
 No Place Like Home
 Becky Stewart
 Codasign
 becky@codasign.com
 July 2012
 
 Serial port reads in GPS data and writes out
 instructions to remote XBee controlling LEDS.
 
 
 
 */

#include <TinyGPS.h>
#include <VirtualWire.h>
#include <SoftwareSerial.h>

const int gpsIn = 1;
const int heelPin = 2;

int centreLED = 9;
int ledPins[] = {
  6, 13, 14, 15, 16, 17, 8, 7};

int ledsLen = 8;
/*
 N  6
 NE 13
 E  14
 SE 15
 S  16
 SW 17
 W  8
 NW 7
 */

int fadeValue = 0;
int fadeDirection = 2;

unsigned long lastGPSfeed;

const unsigned long timeLimit = 5000;
const int triggerNum = 3;

unsigned long initDetect;
int currCount = 0;

int goHome = 0;


int headingValue;
float startLat = 0.0;
float startLon = 0.0;

TinyGPS gps;

static const float LONDON_LAT = 51.508131, LONDON_LON = -0.128002;

//---------------------------------------------------------------------------------
// setup - called once at startup
//---------------------------------------------------------------------------------
void setup()
{
  Serial.begin(57600);

  // set up interrupt for heel clicks
  pinMode(heelPin, INPUT); 
  digitalWrite(heelPin, HIGH);
  attachInterrupt(0, heelClicked, CHANGE);

  // set up LED display
  for (int i=0; i<ledsLen; i++){
    pinMode(ledPins[i], OUTPUT); 
  }
  
  // initialise GPS reading timer
  lastGPSfeed = millis();
}

//---------------------------------------------------------------------------------
// loop - called continuously while powered
//---------------------------------------------------------------------------------
void loop()
{
  bool newdata = false;
  unsigned long loopStart = millis();

  // if it's been longer than the time limit
  // reset the counter
  if ( currCount > 0 && (loopStart-initDetect) > timeLimit )
  {
    currCount = 0; 
    Serial.println("resetting counter");
  }

  // heels have been clicked
  if ( currCount >= triggerNum && (timeLimit-initDetect) )
  {
    // print the results to the serial monitor:
    Serial.print("starting directions " );  
    Serial.println(currCount);
    currCount = 0;
    goHome = 1;
  }

  // we are showing directions
  if ( goHome ){
    // Every second we print an update
    if ( (loopStart - lastGPSfeed) > 1000)
    {
      if (feedgps())
        newdata = true;
       
       lastGPSfeed = millis();
    }
    // spin lights if we don't have a signal
    // show directions if we do
    updateGPS(gps);
  }
  else {
    standby();
  }
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
    // get current course
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
    //updateLightsHeading( readCompass() );

    // update directions
    int dist_so_far = TinyGPS::distance_between(startLat, startLon, flat, flon) / 1000;
    int dist_to_go = TinyGPS::distance_between(flat, flon, LONDON_LAT, LONDON_LON) / 1000;
    float current_course = gps.f_course();
    float course_to_home = TinyGPS::course_to(flat, flon, 51.508131, -0.128002);

    Serial.print(" Current Course: ");
    Serial.print(" ");
    Serial.print(current_course);
    Serial.print(" ");

    Serial.print(" So far travelled ");
    Serial.print(dist_so_far);
    Serial.print(" To go ");
    Serial.print(dist_to_go);
    Serial.print(" ");
    Serial.println(course_to_home);
    const char *str = TinyGPS::cardinal(course_to_home);
    updateLightsHeading(current_course, course_to_home);

  }
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
  currCount++;

  // if this is the first knock
  // note the time
  if ( currCount == 1 )
  {
    initDetect = millis(); 
  }
}

//---------------------------------------------------------------------------------
// standby - standby power light
//---------------------------------------------------------------------------------
void standby(){
  // fade centre from min to max in increments of 5 points:
  analogWrite(centreLED, fadeValue);  

  fadeValue += fadeDirection;
  if(fadeValue > 253 || fadeValue < 2)
    fadeDirection = -1*fadeDirection;  


  delay(30);
}

//---------------------------------------------------------------------------------
// spinLights - spinning light pattern
//---------------------------------------------------------------------------------
void spinLights()
{
  digitalWrite(centreLED, LOW);
  for (int i=0; i<ledsLen; i++){
    digitalWrite(ledPins[i], HIGH); 
    delay(200);
    digitalWrite(ledPins[i], LOW); 
  }
}

//---------------------------------------------------------------------------------
// updateLightsHeading - update light indicating north
//---------------------------------------------------------------------------------
void updateLightsHeading(float c, float h)
{
  
  digitalWrite(ledPins[0], HIGH);
  digitalWrite(ledPins[3], HIGH);
  digitalWrite(ledPins[5], HIGH);
  digitalWrite(ledPins[7], HIGH);
  /*float w = 180/ledsLen;
  int ledOn;

  if ( c-h > (360-w) || c-h < w ){
    ledOn = 0;
  }
  else {
    float f_ledOn = (c-h-w)/(360.0-w) * float(ledsLen);
    ledOn = round(f_ledOn);
    Serial.print(f_ledOn);
    Serial.print(" ");
  }

  //int ledOn = float(heading)/360.0 * ledsLen;
  Serial.println(ledOn);



  for (int i=0; i<ledsLen; i++){
    digitalWrite(ledPins[i], LOW); 
  }
  digitalWrite(ledPins[ledOn], HIGH);
  */
}











