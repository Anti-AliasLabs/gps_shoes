import de.fhpotsdam.unfolding.*;
import de.fhpotsdam.unfolding.geo.*;
import de.fhpotsdam.unfolding.utils.*;

class UploadButton
{
  de.fhpotsdam.unfolding.Map map;

  int xPos, yPos, mPos;
  int markerPlaced, markerX, markerY;
  
  int buttonWidth, buttonHeight;
  color unclicked = color(50);
  color hovered = color(100);
  color clicked = color(150);

  UploadButton(de.fhpotsdam.unfolding.Map m, int x, int y)
  {
    // size and placement
    xPos = x;
    yPos = y;
    buttonWidth = 230;
    buttonHeight = 50;

    mPos = 0; // unclicked button
    
    // marker variables
    markerPlaced = 0;
    
    map = m;
  }

  void display()
  {
    noStroke();
    switch(mPos) {
    case 0:
      fill(unclicked);
      break;
    case 1:
      fill(hovered);
      break;
    case 2:
      fill(clicked);
      break;
    }

    rect(xPos, yPos, buttonWidth, buttonHeight);
    textSize(22);
    fill(230);
    text("Upload to Shoe", xPos + 30, yPos+30);
  }

  void update()
  {
    mPos = 0; // button unclicked
    if (mouseX > xPos &&
        mouseX < xPos + buttonWidth &&
        mouseY > yPos &&
        mouseY < yPos + buttonHeight) {
      mPos = 1; // button hovered over
      if (mousePressed) {
        mPos = 2; // button clicked
      }
    } else {
      // mouse clicked but
    }
  }
}

