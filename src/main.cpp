#include <Arduino.h>

void setup() 
{
  Serial.begin(9600);
}

void loop() 
{
  Serial.println("Hello From the Uno");
  delay(1000);
}