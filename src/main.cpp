#include <Arduino.h>        // grabs the arduino dictionary so the board knows serial and delay
#include <Servo.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Names set for the pins the wires are plugged into
const int TRIG_PIN = 9;     // sets the trig pin variable to 9
const int ECHO_PIN = 10;    // sets the echo pin variable to 10
const int SERVO_PIN = 6;    // sets the servo pin variable to 6

Servo radarServo;

void setup() // Runs one time when the board turns on.
{
  display.begin();
  display.firstPage();
  do 
  {
    display.drawCircle(64, 63, 60, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    display.drawCircle(64, 63, 40, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    display.drawCircle(64, 63, 20, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    display.drawHLine(4, 63, 121);

  } while (display.nextPage());

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

  long duration = pulseIn(ECHO_PIN, HIGH, 26239);

  if (duration == 0)
  {
    return -1;
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
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);
    delay(100);
  }
  for (int angle = 180; angle >= 0; angle -= 2)
  {
    radarServo.write(angle);
    delay(15);

    float distance = getDistance();
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);
    delay(100);
  }
}