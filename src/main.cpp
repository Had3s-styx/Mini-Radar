#include <Arduino.h>        // grabs the arduino dictionary so the board knows serial and delay
#include <Servo.h>         

// Names set for the pins the wires are plugged into
const int TRIG_PIN = 9;     // sets the trig pin variable to 9
const int ECHO_PIN = 10;    // sets the echo pin variable to 10
const int SERVO_PIN = 6;    // sets the servo pin variable to 6
// MAX DISTANCE HC SR04
const float MAX_DISTANCE_CM = 450;
const long MAX_ECHO_US = MAX_DISTANCE_CM * 2 / 0.0343;

Servo radarServo;

void setup() // Runs one time when the board turns on.
{
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);      // Trig pin SENDS signlas out.
  pinMode(ECHO_PIN, INPUT);       // Echo Pin LISTENS for signals.
  radarServo.attach(SERVO_PIN);   // attaching the servo to the pin defined 6
}

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(10);

  long duration = pulseIn(ECHO_PIN, HIGH, MAX_ECHO_US);

  if (duration == 0)
  {
    return MAX_DISTANCE_CM * 0.393701; // converts the return to inches
  }

  float distanceInch = (duration * 0.0343 / 2) * 0.393701;  

  return distanceInch;
}
void loop() 
{
  for (int angle = 0; angle <=180; angle += 2)
  {
    radarServo.write(angle);
    delay(15);
    
    float distance = getDistance();
    Serial.print("Angle, Distance: ");
    Serial.print(angle); 
    Serial.print(" Degrees, ");
    Serial.print(distance);
    Serial.println(" inches");
    delay(10);
  }
  for (int angle = 180; angle >= 0; angle -= 2)
  {
    radarServo.write(angle);
    delay(15);

    float distance = getDistance();
    Serial.print("Angle, Distance: ");
    Serial.print(angle); 
    Serial.print(" Degrees, ");
    Serial.print(distance);
    Serial.println(" inches");
    delay(10);
  }
}