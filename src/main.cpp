#include <Arduino.h>        // grabs the arduino dictionary so the board knows serial and delay

// Names set for the pins the wires are plugged into
const int TRIG_PIN = 9;     // sets the trig pin variable to 9
const int ECHO_PIN = 10;    // sets the echo pin variable to 10

void setup() // Runs one time when the board turns on.
{
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);      // Trig pin SENDS signlas out.
  pinMode(ECHO_PIN, INPUT);       // Echo Pin LISTENS for signals.
}

void loop() 
{
  // Step 1
  digitalWrite(TRIG_PIN, LOW);    // Makes sure the Trig Pin starts at a quiet state of 0v
  delayMicroseconds(2);           // Short microsecond delay
  digitalWrite(TRIG_PIN, HIGH);   // HIGH = on, 5V starts the ping 
  delayMicroseconds(10);          // small delay in microseconds
  digitalWrite(TRIG_PIN, LOW);    // Turns off the sensor.

  // Step 2
  // times how long the echo takes to bounce back.
  // pulseIn is like a stopwatch: starts when echo turns on and stops when it turns off
  long duration = pulseIn(ECHO_PIN, HIGH);    

  // Step 3
  // Distance = speed x time
  // sound travels at about 343 meters per second. in centimeters per microsecond thats 0.0343
  // using distance = duration * 0.0343 / 2
  float distance = duration * 0.0343 / 2; // time x speed of sound cut in half for one-way

  // Step 4 : show the distance label, the distance and unit of measurement
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(500);

}