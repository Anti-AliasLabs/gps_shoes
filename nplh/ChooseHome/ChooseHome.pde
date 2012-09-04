import processing.opengl.*;
import codeanticode.glgraphics.*;
import de.fhpotsdam.unfolding.*;
import de.fhpotsdam.unfolding.geo.*;
import de.fhpotsdam.unfolding.utils.*;
import de.fhpotsdam.unfolding.providers.*;
import guicomponents.*;


de.fhpotsdam.unfolding.Map map;

UploadButton button;
ZoomButton zoomIn;
ZoomButton zoomOut;

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

  inButton = new GButton(this, "+", boxX+240, boxY+70, 30, 30);
  inButton.tag = "zoom in";
  outButton = new GButton(this, "-", boxX+240, boxY+110, 30, 30);
  outButton.tag = "zoom out";

  uploadButton = new GButton(this, "Upload to Shoes", boxX+10, boxY+boxHeight, boxWidth-20, 40);
  uploadButton.tag = "upload";

  //map = new de.fhpotsdam.unfolding.Map(this, new Google.GoogleMapProvider());
  map = new de.fhpotsdam.unfolding.Map(this);
  //map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);
  map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);

  //button = new UploadButton(map, boxX+10, boxY+boxHeight-60);
  zoomIn = new ZoomButton(map, boxX+boxWidth-40, boxY+80, true);
  zoomOut = new ZoomButton(map, boxX+boxWidth-40, boxY+120, false);



  MapUtils.createDefaultEventDispatcher(this, map);

  // load in images
  pointerImage = loadImage("pointershoe.gif");
  logo = loadImage("logoshoe.gif");
}

void draw() {
  //draw map
  background(0);
  map.draw();



  // draw cursor
  fill(20, 100);
  noStroke();
  ellipse(mouseX, mouseY, 40, 40);

  // draw instructions
  drawInstructions();
}

void drawInstructions() {

  noStroke();
  fill(255, 190);
  rect(boxX, boxY, boxWidth, boxHeight*0.3);

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

void handleTextFieldEvents(GTextField tfield) {
  if (tfield.getEventType() == GTextField.ENTERED)
  {
    postcode = tfield.viewText();
  }
}

void handleButtonEvents(GButton button) {
  if (button.eventType == GButton.CLICKED)
  {
    if(button.tag == "search")
    { 
      println("search");
    }
    if(button.tag == "zoom in")
    { 
      println("zoom in");
    }
    if(button.tag == "zoom out")
    {
     println("zoom out"); 
    }
    if(button.tag == "upload")
    { 
      println("upload");
    }
  }
}
