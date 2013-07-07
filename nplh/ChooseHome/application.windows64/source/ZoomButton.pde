import de.fhpotsdam.unfolding.*;
import de.fhpotsdam.unfolding.geo.*;
import de.fhpotsdam.unfolding.utils.*;

class ZoomButton
{
  de.fhpotsdam.unfolding.Map map;
  
  int xPos, yPos, mPos;
  int buttonSize;
  
  boolean zoom;
  
  color unclicked = color(50);
  color hovered = color(100);
  color clicked = color(150);

  ZoomButton(de.fhpotsdam.unfolding.Map m, int x, int y, boolean direction)
  {
    xPos = x;
    yPos = y;
    buttonSize = 30;

    mPos = 0;
    
    zoom = direction;
    
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

    rect(xPos, yPos, buttonSize, buttonSize);
    textSize(22);
    fill(230);
    
    if(zoom)
    {
      text("+", xPos+8, yPos+23);
    } else {
      text("-", xPos+10, yPos+22);
    }
  }

  void update()
  {
    mPos = 0;
    if (mouseX > xPos &&
        mouseX < xPos + buttonSize &&
        mouseY > yPos &&
        mouseY < yPos + buttonSize) {
      mPos = 1;
      if (mousePressed) {
        mPos = 2;
      }
    }
  } 
}
