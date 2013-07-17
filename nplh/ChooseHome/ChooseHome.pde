import processing.serial.*;

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
int boxHeight = 200;

// communication with shoe
boolean uploading = false;
boolean shoeConnected = false;
Serial shoePort;
String portName = "";


GTextField postcodeBox;
GImageButton searchButton, inButton, outButton, uploadButton;

String postcode = "";
Location marker = new Location(-1.0, -1.0);
int markerX = -1;
int markerY = -1;

int zoomLevel = 12;

float longitude = -1;
float latitude = -1;

void setup() {
  size(1000, 800, GLConstants.GLGRAPHICS);

  // look and fonts
  instructionsFont = loadFont("Baskerville-36.vlw");
  textFont(instructionsFont, 24);

  GComponent.globalColor = GCScheme.getColor(this, GCScheme.GREY_SCHEME);
  GComponent.globalFont = GFont.getFont(this, "Baskerville", 20);
  postcodeBox = new GTextField(this, "", boxX+10, boxY+180, 100, 15, true);

  String[] search_files = {
    "search_off.png", "search_over.png", "search_down.png"
  };
  searchButton = new GImageButton(this, null, search_files, boxX+120, boxY+180);
  searchButton.tag = "search";

  String[] in_files = {
    "zoom_in_off.png", "zoom_in_over.png", "zoom_in_down.png"
  };
  inButton = new GImageButton(this, null, in_files, boxX+240, boxY+140);
  inButton.tag = "zoom in";

  String[] out_files = {
    "zoom_out_off.png", "zoom_out_over.png", "zoom_out_down.png"
  };
  outButton = new GImageButton(this, null, out_files, boxX+240, boxY+180);
  outButton.tag = "zoom out";

  //uploadButton = new GButton(this, "Upload to Shoes", boxX+10, boxY+boxHeight, boxWidth-20, 40);
  String[] upload_files = {
    "upload_off.png", "upload_over.png", "upload_down.png"
  };
  uploadButton = new GImageButton(this, null, upload_files, boxX+10, boxY+boxHeight+30);
  uploadButton.tag = "upload";

  map = new de.fhpotsdam.unfolding.Map(this, new OpenStreetMap.OpenStreetMapProvider() );
  //map = new de.fhpotsdam.unfolding.Map(this, new Google.GoogleMapProvider());
  //map = new de.fhpotsdam.unfolding.Map(this);
  //map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);
  map.zoomAndPanTo(new Location(51.508131f, -0.128002f), zoomLevel);

  MapUtils.createDefaultEventDispatcher(this, map);

  // load in images
  pointerImage = loadImage("pointershoe.png");
  logo = loadImage("logoshoe.gif");
}
// -------------------------------------------------------------
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

  // if uploading
  if ( uploading )
  {
    // draw upload status
    drawUploadStatus();
  }
}
// -------------------------------------------------------------
void drawInstructions() {

  noStroke();
  fill(255, 190);
  // title rect
  rect(boxX, boxY, boxWidth, boxHeight*0.3);

  // interface rect
  rect(boxX, boxY+boxHeight*0.4, boxWidth, boxHeight);

  stroke(0);
  fill(0);
  image(logo, boxX+9, boxY+8, 265, 28);
  textSize(20);

  text("Post Code Search", boxX+10, boxY+170);

  text("Click on map or type post code", boxX+10, boxY+110);
  text("to select destination.", boxX+10, boxY+133);
}
// -------------------------------------------------------------
void mouseClicked() {
  if (mouseX > boxX+boxWidth &&
    mouseY > boxY+boxHeight)
  {
    markerX = mouseX;
    markerY = mouseY;

    marker = map.getLocationFromScreenPosition((float)mouseX, (float)mouseY);
  }
}

// -------------------------------------------------------------
void handleTextFieldEvents(GTextField tfield) {
  if (tfield.getEventType() == GTextField.ENTERED)
  {
    postcode = tfield.viewText();
  }
}
// -------------------------------------------------------------
void handleImageButtonEvents(GImageButton button) {
  if (button.eventType == GImageButton.CLICKED)
  {
    if (button.tag == "search")
    { 
      println("search");
      getCoordinatesFromString(postcode);
    }
    if (button.tag == "zoom in")
    { 
      zoomLevel++;
      map.zoomToLevel(zoomLevel);
      println("zoom in");
    }
    if (button.tag == "zoom out")
    {
      zoomLevel--;
      map.zoomToLevel(zoomLevel);
      println("zoom out");
    }
    if (button.tag == "upload")
    { 
      println("upload");
      uploadToShoe();
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
// -------------------------------------------------------------
void uploadToShoe() 
{
  uploading = true;
  portName = Serial.list()[0];
  println( portName );
  shoePort = new Serial(this, portName, 9600);
  shoePort.bufferUntil('\n');
}
// -------------------------------------------------------------
void drawUploadStatus() 
{
  noStroke();
  fill( 255 );
  rect(width/2-130, height/2-50, 260, 100 );

  if ( shoeConnected )
  {
    fill(0);
    text("Connected to shoe", width/2-80, height/2 );
    println(latitude);
    println(longitude);

    int intLat = (int) ( latitude * 100000);
    int intLon = (int) ( longitude * 100000);
    
    

    String sendLat = Integer.toString( intLat );
    String sendLon = Integer.toString( intLon );
    
    if( intLon%100 < 1 ) {
      if( intLon < 0 ) {
        sendLon = "-00" + sendLon.substring(1, sendLon.length());
      }
    }
    
    println(sendLat);
    println(sendLon);
    println( "---------");
    
    // compensate for negative sign
    if ( intLat < 0 ) {
      shoePort.write( sendLat.substring(0, 6) );
    } 
    else {
      shoePort.write( sendLat.substring(0, 5) );
    }
    shoePort.write(',');
    if ( intLon < 0 ) {
      shoePort.write(sendLon.substring(0, 6));
    } 
    else {
      shoePort.write(sendLon.substring(0, 5));
    }
    shoePort.write('\n');
    uploading = false;
  } 
  else
  {
    fill(0);
    text("Connecting to shoe...", width/2-80, height/2 );
    // send test message to see if shoe is connected
    shoePort.write("connected\n");
  }
}

void serialEvent( Serial p )
{
  String inString = p.readStringUntil('\n');
  inString = inString.replace('\n', ' ');
  String trimmedString = trim(inString);
  println(inString);

  if ( trimmedString.equals("shoe") )
    shoeConnected = true;
}

