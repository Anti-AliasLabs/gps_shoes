

void setup()
{
  float heading;
  float direction;
  int LED;
  
  heading = 0;
  direction = 15;
  
  LED = anglesToLED(heading, direction);

  println("LED: " + LED);
}


// ---------------------------------------------------------
// anglesToLED: given a heading and a direction, calculate which of 8
// LEDs should light up (valued 0-7)
int anglesToLED(float heading, float direction)
{
  float relativeAngle;
  int LED;

  // calculate relative angle 
  relativeAngle = direction-heading;

  // ensure relative angle is in the range 0-360
  relativeAngle = modulo360(relativeAngle);
 
  // calculate LED based upon borders of (x*45)+22.5 degrees for x = (0...7) 
  LED = floor(modulo360(relativeAngle+22.5)/45);

  return LED;
}

// ---------------------------------------------------------
// modulo360: simple modulo 360. Negative numbers are set within
// the 0-360 range which is not done by built in Processing modulo
float modulo360(float val)
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
