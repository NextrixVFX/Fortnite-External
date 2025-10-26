#include <Mouse.h>

void setup()
{
  Serial.begin(9600);
  Mouse.begin();
}

void loop()
{
  if (!Serial.available())
    return;

  String s = Serial.readStringUntil('\n');
  s.trim();
  s.toLowerCase();

  if (s.startsWith("move"))
  {
    int firstSpace = s.indexOf(' ');
    int secondSpace = s.indexOf(' ', firstSpace + 1);

    if (firstSpace > 0 && secondSpace > firstSpace)
    {
      int dx = s.substring(firstSpace + 1, secondSpace).toInt();
      int dy = s.substring(secondSpace + 1).toInt();
      
      Serial.println("Moving to [XY]: ");
      Serial.println(dx);
      Serial.println(dy);

      Mouse.move(dx, dy, 0); // moves relative to current position
    } 
  }
  else if (s == "lclick")
  {
    Mouse.click(MOUSE_LEFT);
  } 
  else if (s == "rclick")
  {
    Mouse.click(MOUSE_RIGHT);
  } 
  else if (s == "lpress")
  {
    Mouse.press(MOUSE_LEFT);
  } 
  else if (s == "rpress")
  {
    Mouse.press(MOUSE_RIGHT);
  } 
  else if (s == "lrelease")
  {
    Mouse.release(MOUSE_LEFT);
  } 
  else if (s == "rrelease")
  {
    Mouse.release(MOUSE_RIGHT);
  }
  else 
  {
    Serial.println("Invalid Input:\t");
    Serial.print(s);
  }
  
}