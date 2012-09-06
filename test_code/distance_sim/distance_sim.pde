void setup()
{
 size(200, 250); 
}

void draw()
{
  background(255);
  for(int i=0; i<5; i++)
  {
   rect(width/2-10, i*35 + 40, 20, 20); 
  }
}
