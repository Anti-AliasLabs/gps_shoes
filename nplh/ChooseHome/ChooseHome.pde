import de.fhpotsdam.unfolding.mapdisplay.*;
import de.fhpotsdam.unfolding.utils.*;
import de.fhpotsdam.unfolding.marker.*;
import de.fhpotsdam.unfolding.tiles.*;
import de.fhpotsdam.unfolding.interactions.*;
import de.fhpotsdam.unfolding.*;
import de.fhpotsdam.unfolding.core.*;
import de.fhpotsdam.unfolding.geo.*;
import de.fhpotsdam.unfolding.events.*;
import de.fhpotsdam.utils.*;
import de.fhpotsdam.unfolding.providers.*;

import processing.opengl.*;
import codeanticode.glgraphics.*;
import guicomponents.*;

import com.francisli.processing.http.*;

de.fhpotsdam.unfolding.Map map;

boolean responseProcessed = false;

PImage pointerImage;
PImage logo;

PFont instructionsFont;

int boxX = 20;
int boxY = 20;
int boxWidth = 280;
int boxHeight = 150;

GTextField postcodeBox;
GButton searchButton, uploadButton, inButton, outButton;
String postcode = "";
Location marker = new Location(-1.0, -1.0);
int markerX = -1;
int markerY = -1;

float longitude = -1;
float latitude = -1;

void setup() {
  size(1000, 800, GLConstants.GLGRAPHICS);

  // look and fonts
  instructionsFont = loadFont("Baskerville-36.vlw");
  textFont(instructionsFont, 24);

  GComponent.globalColor = GCScheme.getColor(this, GCScheme.GREY_SCHEME);
  GComponent.globalFont = GFont.getFont(this, "Baskerville", 20);
  postcodeBox = new GTextField(this, "", boxX+10, boxY+100, 100, 15, true);
  searchButton = new GButton(this, "Search", boxX+120, boxY+100, 90, 30);
  searchButton.tag = "search";

  /*inButton = new GButton(this, "+", boxX+240, boxY+70, 30, 30);
  inButton.tag = "zoom in";
  outButton = new GButton(this, "-", boxX+240, boxY+110, 30, 30);
  outButton.tag = "zoom out";*/

  uploadButton = new GButton(this, "Upload to Shoes", boxX+10, boxY+boxHeight, boxWidth-20, 40);
  uploadButton.tag = "upload";

  //map = new de.fhpotsdam.unfolding.Map(this, new Google.GoogleMapProvider());
  map = new de.fhpotsdam.unfolding.Map(this);
  //map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);
  map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);

  MapUtils.createDefaultEventDispatcher(this, map);

  // load in images
  pointerImage = loadImage("pointershoe.png");
  logo = loadImage("logoshoe.gif");
}

void draw() {
  //draw map
  background(0);
  map.draw();


  // draw cursor
  if (marker.x == -1) {
    image(pointerImage, mouseX-20, mouseY-37, 40, 75);
  } 
  else {
    float[] curr = map.getScreenPositionFromLocation(marker);
    image(pointerImage, curr[0]-20, curr[1]-37, 40, 75);
  }

  // draw instructions
  drawInstructions();
}

void drawInstructions() {

  noStroke();
  fill(255, 190);
  // title rect
  rect(boxX, boxY, boxWidth, boxHeight*0.3);

  // interface rect
  rect(boxX, boxY+boxHeight*0.4, boxWidth, boxHeight);

  stroke(0);
  fill(0);
  //text("Your Title Here", boxX+40, boxY+30);
  image(logo, boxX+9, boxY+8, 265, 28);
  textSize(20);

  text("Post Code Search", boxX+10, boxY+90);
  //text("+  to zoom in", boxX+20, boxY+100);
  //text("-  to zoom out", boxX+20, boxY+130);

  //text("click on map", boxX+20, boxY+180);
  //text("to select destination", boxX+20, boxY+210);
}

void mouseClicked() {
  if (mouseX > boxX+boxWidth &&
    mouseY > boxY+boxHeight)
  {
    markerX = mouseX;
    markerY = mouseY;
  }

  marker = map.getLocationFromScreenPosition((float)mouseX, (float)mouseY);
}


void handleTextFieldEvents(GTextField tfield) {
  if (tfield.getEventType() == GTextField.ENTERED)
  {
    postcode = tfield.viewText();
  }
}

void handleButtonEvents(GButton button) {
  if (button.eventType == GButton.CLICKED)
  {
    if (button.tag == "search")
    { 
      println("search");
      getCoordinatesFromString(postcode);
    }
    if (button.tag == "zoom in")
    { 
      println("zoom in");
    }
    if (button.tag == "zoom out")
    {
      println("zoom out");
    }
    if (button.tag == "upload")
    { 
      println("upload");
    }
  }
}

// -------------------------------------------------------------
// getCoordinatesFromString: given a string-based post-code it queries
// Google Maps API for the longitude and latitude. Response is handled
// by the responseReceived function
void getCoordinatesFromString(String location)
{
  // create new client
  HttpClient client = new HttpClient(this, "maps.googleapis.com");

  // instantiate a new HashMap
  HashMap params = new HashMap();

  // pass parameters as key-value pairs
  params.put("address", location);
  params.put("sensor", "true");

  // make the request
  client.GET("/maps/api/geocode/json", params);
}

// -------------------------------------------------------------
// responseReceived: implemented here to handle responses from
// Google API so that the latitude and longitude of a response
// are stored in global varialbles 'longitude' and 'latitude'
void responseReceived(HttpRequest request, HttpResponse response) 
{
  // check for HTTP 200 success code
  if (response.statusCode == 200) {
    JSONObject results = response.getContentAsJSONObject();

    longitude = results.get("results").get(0).get("geometry").get("location").get("lng").floatValue();
    latitude = results.get("results").get(0).get("geometry").get("location").get("lat").floatValue();

    printLongitudeAndLatitude();
  } 
  else {
    // output the entire response as a string
    println(response.getContentAsString());
  }
}

// -------------------------------------------------------------
// printLongitudeAndLatitude: print out co-ordinates
void printLongitudeAndLatitude()
{
  println(latitude + ", " + longitude); 
  marker.x = latitude;
  marker.y = longitude;
}

