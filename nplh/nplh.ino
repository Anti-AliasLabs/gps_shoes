

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

SoftwareSerial softSerial(3, 4); // RX, TX

const int gpsIn = 1;
const int heelPin = 2;

int centreLED = 5;
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

float homeLat = 37.6922, homeLon = 97.3372;

//---------------------------------------------------------------------------------
// setup - called once at startup
//---------------------------------------------------------------------------------
void setup()
{
  Serial.begin(57600);
  softSerial.begin(9600);

  // set up interrupt for heel clicks
  pinMode(heelPin, INPUT); 
  digitalWrite(heelPin, HIGH);
  attachInterrupt(0, heelClicked, CHANGE);

  // set up LED display
  for (int i=0; i<ledsLen; i++){
    pinMode(ledPins[i], OUTPUT); 
  }

  // Initialise the IO and ISR
  vw_set_ptt_inverted(true); // Required for DR3100
  vw_setup(2000);	 // Bits per sec

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
    softSerial.println("resetting counter");
  }

  // heels have been clicked
  if ( currCount >= triggerNum && (timeLimit-initDetect) )
  {
    // print the results to the serial monitor:
    softSerial.print("starting directions " );  
    softSerial.println(currCount);
    currCount = 0;
    goHome = 1;
  }

  // we are showing directions
  if ( goHome ){
    // Every second we print an update
    while ( millis() - loopStart < 1000)
    {
      if (feedgps())
        newdata = true;
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
    softSerial.print("Current Latitude: ");
    softSerial.print(flat);
    softSerial.print(" Current Longitude: ");
    softSerial.print(flon);

    // keep track of where we are once we've started 
    // navigating home (to keep track of distance travelled
    // once we start going home)
    if ( goHome && startLat == 0.0 && startLon == 0.0) {
      startLat = flat;
      startLon = flon;
    }

    // update directions
    int total_dist = TinyGPS::distance_between(startLat, startLon, homeLat, homeLon) / 1000;
    int dist_travelled = TinyGPS::distance_between(startLat, startLon, flat, flon) / 1000;
    float current_course = gps.f_course(); 
    float course_to_home = TinyGPS::course_to(flat, flon, homeLat, homeLon);

    // send distance to right shoe for lights
    int d = dist_travelled/total_dist * 6.0;
    sendDistanceToRight(d);
    Serial.print(" Current Course: ");
    Serial.print(TinyGPS::cardinal(gps.f_course()));
    Serial.print(" ");
    Serial.print(current_course);
    Serial.print(" ");

    
    Serial.print(" To go ");
    Serial.print(dist_travelled);
    Serial.print(" ");
    Serial.print(d);
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
  Serial.println("click");
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
  // turn off all LEDs
  for (int i=0; i<ledsLen; i++){
    digitalWrite(ledPins[i], LOW); 
  }

  //given a heading and a direction, calculate which of 8
  // LEDs should light up (valued 0-7)
  float relativeAngle;
  int ledOn;

  // calculate relative angle 
  relativeAngle = c-h;

  // ensure relative angle is in the range 0-360
  relativeAngle = modulo360((int)relativeAngle);

  // calculate LED based upon borders of (x*45)+22.5 degrees for x = (0...7) 
  ledOn = floor(modulo360(relativeAngle+22.5)/45);

  digitalWrite(centreLED, LOW);
  delay(100);
  digitalWrite(centreLED, HIGH);
  digitalWrite(ledPins[ledOn], HIGH);
  softSerial.println(ledOn);
}

//---------------------------------------------------------------------------------
// modulo360: simple modulo 360. Negative numbers are set within
// the 0-360 range which is not done by built in Processing modulo
//---------------------------------------------------------------------------------
float modulo360(int val)
{
  // ifval is < 0, set it within 0-360
  while (val < 0)
  {
    val = val+360;
  }

  // if val is > 360, set within 0-360
  val = val % 360;

  return val;
}

//---------------------------------------------------------------------------------
// sendDistanceToRight - sends an int between 1 and 6 to right shoe
//---------------------------------------------------------------------------------
void sendDistanceToRight(int dist)
{
  uint8_t msg[] = {
    dist    };

  vw_send(msg, 1);
  vw_wait_tx(); // Wait until the whole message is gone
}










