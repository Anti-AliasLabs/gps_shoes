import processing.opengl.*;
import codeanticode.glgraphics.*;
import de.fhpotsdam.unfolding.*;
import de.fhpotsdam.unfolding.geo.*;
import de.fhpotsdam.unfolding.utils.*;

de.fhpotsdam.unfolding.Map map;

UploadButton button;
ZoomButton zoomIn;
ZoomButton zoomOut;

PFont instructionsFont;

int boxX = 20;
int boxY = 20;
int boxWidth = 250;
int boxHeight = 300;


void setup() {
  size(1000, 800, GLConstants.GLGRAPHICS);

  map = new de.fhpotsdam.unfolding.Map(this);
  map.zoomAndPanTo(new Location(51.508131f, -0.128002f), 12);

  button = new UploadButton(map, boxX+10, boxY+boxHeight-60);
  zoomIn = new ZoomButton(map, boxX+boxWidth/2-25, boxY+100, true);
  zoomOut = new ZoomButton(map, boxX+boxWidth/2-25, boxY+150, false);

  instructionsFont = loadFont("AppleGothic-48.vlw");
  textFont(instructionsFont, 24);

  MapUtils.createDefaultEventDispatcher(this, map);
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

  // draw buttons
  zoomIn.update();
  zoomIn.display();

  zoomOut.update();
  zoomOut.display();

  button.update();
  button.display();
}

void drawInstructions() {

  noStroke();
  fill(255, 190);
  rect(boxX, boxY, boxWidth, boxHeight);

  stroke(0);
  fill(0);
  text("Your Title Here", boxX+40, boxY+30);
  textSize(24);
  //text("+  to zoom in", boxX+20, boxY+100);
  //text("-  to zoom out", boxX+20, boxY+130);

  //text("click on map", boxX+20, boxY+180);
  //text("to select destination", boxX+20, boxY+210);
}

