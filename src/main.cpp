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

  //Step 3
  Serial.print("Echo time (microseconds): "); // writes the label but stays on the same line.
  Serial.println(duration);                   // Write the number then jumps to the next line
  delay(500);                                 // delays half a second so the screen doesnt flood

}