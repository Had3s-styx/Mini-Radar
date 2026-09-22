// Had3s-Styx
// Mini Radar using a servo, ultrasonic radar, uno 43 and oled display
// Running
#include <Arduino.h>        // grabs the arduino dictionary so the board knows serial and delay
#include <Servo.h>          // Servo Lib
#include <U8g2lib.h>        // Display Lib

U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

// Names set for the pins the wires are plugged into
const int TRIG_PIN = 9;     // sets the trig pin variable to 9
const int ECHO_PIN = 10;    // sets the echo pin variable to 10
const int SERVO_PIN = 6;    // sets the servo pin variable to 6
const float MAX_RANGE_IN = 48.0;  // sets the max range defined on the display

// What the sweep found: one slot per angle, holding how many screen dots out
// from the center it was detected. 0 means nothing was detected at that angle. :AI ASSISTED
byte radius[91];   // 91 numbered slots, one for each servo angle (0, 2, 4 ... 180) :AI ASSISTED

// The dots that actually get drawn: ONE per object, worked out from radius[] :AI ASSISTED
const int MAX_DOTS = 12;   // the most dots the screen will ever show at once :AI ASSISTED
byte dotX[MAX_DOTS];       // x position (across) of each dot to draw :AI ASSISTED
byte dotY[MAX_DOTS];       // y position (down) of each dot to draw :AI ASSISTED
int dotCount = 0;          // how many dots there are right now :AI ASSISTED
int nearestDots = 0;       // distance to the closest object in screen dots (0 = none) :AI ASSISTED

const int SAME_OBJECT_DOTS = 6;   // neighbors within this many dots of each other count as one object :AI ASSISTED
const int MIN_HITS = 2;           // a group needs this many readings or it is ignored as noise :AI ASSISTED

Servo radarServo;

void setup() // Runs one time when the board turns on.
{
  display.begin();                // wakes the display

  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);      // Trig pin SENDS signlas out.
  pinMode(ECHO_PIN, INPUT);       // Echo Pin LISTENS for signals.
  radarServo.attach(SERVO_PIN);   // attaching the servo to the pin defined 6
}

void recordDot(int angle, float distance)
{
  int slot = angle / 2;

  if (distance > 0 && distance <= MAX_RANGE_IN)
  {
    // turn inches into screen dots (48 in = 60 dots) and save it in this angle's slot :AI ASSISTED
    radius[slot] = distance / MAX_RANGE_IN * 60;
  }
  else
  {
    radius[slot] = 0;   // clear the slot so an old dot disappears :AI ASSISTED
  }
}

// Turns everything the sweep found into ONE dot per object.
// One object shows up at several neighboring angles because the sensor hears
// a wide cone, so we group neighbors that are about the same distance away
// and draw a single dot in the middle of each group. :AI ASSISTED
void buildDots()     // makes one dot per object from the saved readings :AI ASSISTED
{
  dotCount = 0;      // start with no dots :AI ASSISTED
  nearestDots = 0;   // start with no closest object :AI ASSISTED
  int i = 0;         // i is the slot we are looking at, starting at angle 0 :AI ASSISTED

  while (i <= 90)         // keep going until all 91 slots have been looked at :AI ASSISTED
  {
    if (radius[i] == 0)   // empty slot: nothing was detected at this angle :AI ASSISTED
    {
      i++;                // move on to the next slot :AI ASSISTED
      continue;           // skip the rest and go back to the top of the while :AI ASSISTED
    }

    // found something, so start a group here :AI ASSISTED
    int first = i;           // the first slot in this group :AI ASSISTED
    int last = i;            // the last slot in this group so far :AI ASSISTED
    int total = radius[i];   // running sum of the distances, so we can average them later :AI ASSISTED
    int hits = 1;            // how many readings are in this group :AI ASSISTED

    // keep adding neighbors that are about the same distance away.
    // It may skip one empty angle, since single readings get lost sometimes. :AI ASSISTED
    int j = i + 1;            // j looks ahead at the slots after the start of the group :AI ASSISTED
    // keep looking ahead, but stop after more than one empty slot in a row :AI ASSISTED
    while (j <= 90 && j - last <= 2)
    {
      if (radius[j] > 0)      // this slot detected something :AI ASSISTED
      {
        // is it much closer or farther than the last one? :AI ASSISTED
        if (abs(radius[j] - radius[last]) > SAME_OBJECT_DOTS)
        {
          break;              // yes, so it is a different object: stop growing this group :AI ASSISTED
        }
        last = j;             // no, it is the same object: the group now ends here :AI ASSISTED
        total += radius[j];   // add its distance to the running sum :AI ASSISTED
        hits++;               // count one more reading in the group :AI ASSISTED
      }
      j++;                    // look at the next slot :AI ASSISTED
    }

    // one lonely reading is probably noise, so only keep groups with company :AI ASSISTED
    // keep the group only if it is big enough and there is room for another dot :AI ASSISTED
    if (hits >= MIN_HITS && dotCount < MAX_DOTS)
    {
      float r = total / hits;                    // average distance of the group, in dots :AI ASSISTED
      float rad = (first + last) * PI / 180.0;   // middle angle of the group, in radians :AI ASSISTED
      dotX[dotCount] = 64 + r * cos(rad);        // the dot's x: how far sideways from the center :AI ASSISTED
      // the dot's y: how far up from the bottom (minus, since y counts down) :AI ASSISTED
      dotY[dotCount] = 63 - r * sin(rad);
      dotCount++;                                // we now have one more dot :AI ASSISTED

      if (nearestDots == 0 || r < nearestDots)   // first dot found, or closer than the closest so far? :AI ASSISTED
      {
        nearestDots = r;                         // yes, so remember it as the closest :AI ASSISTED
      }
    }

    i = last + 1;   // carry on with the slot after this group :AI ASSISTED
  }
}

float getDistance()  // called by the sweep of the servo to get the distance reading from the ultrasonic
{
  digitalWrite(TRIG_PIN, LOW);                      // turns off the sensor
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, HIGH);                     // turns on the sensor
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);                      // turns off the sensor
  delayMicroseconds(10);

  long duration = pulseIn(ECHO_PIN, HIGH, 26239);   // measures the response 

  if (duration == 0)    // returns -1 if the reading is past teh hardware defined range
  {
    return -1;
  }

  float distanceInch = (duration * 0.0343 / 2) * 0.393701;    // converts the cm distance to inches

  return distanceInch;
}

void drawRadar(int angle)         
{
  // "static" means these are remembered between calls, instead of being
  // forgotten every time the function ends. We use them to know which way
  // the sweep is moving, so the tail can trail BEHIND the line. :AI ASSISTED
  static int lastAngle = 0;                // the angle from the last call, remembered between calls :AI ASSISTED
  static int direction = 1;                // which way the sweep is moving: 1 = up, -1 = down :AI ASSISTED
  if (angle > lastAngle) direction = 1;    // the angle went up, so the sweep is moving up :AI ASSISTED
  if (angle < lastAngle) direction = -1;   // the angle went down, so the sweep is moving down :AI ASSISTED
  lastAngle = angle;                       // save this angle for the next call :AI ASSISTED

  // The sweep line, pointing at the servo's angle :AI ASSISTED
   float rad = angle * PI / 180.0;
  int x = 64 + 60 * cos(rad);
  int y = 63 - 60 * sin(rad);

  // The tail: two faint dotted lines 4 and 8 degrees behind the sweep line.
  // Their angles are worked out here so the trig isn't repeated for every strip. :AI ASSISTED
  float tailCos[2];                                     // sideways share of each tail line's angle :AI ASSISTED
  float tailSin[2];                                     // upward share of each tail line's angle :AI ASSISTED
  // true if that tail line is on the radar and should be drawn :AI ASSISTED
  bool tailOn[2];
  for (int t = 0; t < 2; t++)                           // do this for both tail lines :AI ASSISTED
  {
    // 4 degrees behind the sweep line, then 8 degrees behind :AI ASSISTED
    int tailAngle = angle - direction * 4 * (t + 1);
    tailOn[t] = (tailAngle >= 0 && tailAngle <= 180);   // only draw it if it is inside the half circle :AI ASSISTED
    float tailRad = tailAngle * PI / 180.0;             // degrees to radians :AI ASSISTED
    tailCos[t] = cos(tailRad);                          // save the sideways share :AI ASSISTED
    tailSin[t] = sin(tailRad);                          // save the upward share :AI ASSISTED
  }

  // Short tick marks on the outer arc every 30 degrees (30, 60, 90, 120, 150) :AI ASSISTED
  int tickX1[5];                                  // outer end of each degree tick, x :AI ASSISTED
  int tickY1[5];                                  // outer end of each degree tick, y :AI ASSISTED
  int tickX2[5];                                  // inner end of each degree tick, x :AI ASSISTED
  int tickY2[5];                                  // inner end of each degree tick, y :AI ASSISTED
  for (int t = 0; t < 5; t++)                     // one tick for each of 30, 60, 90, 120 and 150 degrees :AI ASSISTED
  {
    float tickRad = (30 + t * 30) * PI / 180.0;   // this tick's angle in radians :AI ASSISTED
    tickX1[t] = 64 + 60 * cos(tickRad);           // outer end is 60 dots from the center, x :AI ASSISTED
    tickY1[t] = 63 - 60 * sin(tickRad);           // outer end, y :AI ASSISTED
    tickX2[t] = 64 + 55 * cos(tickRad);           // inner end is 55 dots from the center, x :AI ASSISTED
    tickY2[t] = 63 - 55 * sin(tickRad);           // inner end, y :AI ASSISTED
  }

  // work out the dots BEFORE drawing, since drawing runs 8 times :AI ASSISTED
  buildDots();
  int nearestInches = nearestDots * MAX_RANGE_IN / 60;   // turn dots back into inches for the readout :AI ASSISTED

  display.firstPage();
  do
  {
    // The frame: one outer arc and a baseline. Nothing else, to keep it clean. :AI ASSISTED
    display.drawCircle(64, 63, 60, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);
    display.drawHLine(4, 63, 121);

    for (int t = 0; t < 5; t++)   // draw the 5 degree ticks :AI ASSISTED
    {
      // draw one tick :AI ASSISTED
      display.drawLine(tickX1[t], tickY1[t], tickX2[t], tickY2[t]);
    }

    display.drawVLine(44, 60, 3);    // small vertical tick 20 dots left of center (16 inches) :AI ASSISTED
    display.drawVLine(84, 60, 3);    // small vertical tick 20 dots right of center (16 inches) :AI ASSISTED
    display.drawVLine(24, 60, 3);    // small vertical tick 40 dots left of center (32 inches) :AI ASSISTED
    display.drawVLine(104, 60, 3);   // small vertical tick 40 dots right of center (32 inches) :AI ASSISTED

    // the sensor: a small filled half-circle at the center :AI ASSISTED
    display.drawDisc(64, 63, 3, U8G2_DRAW_UPPER_RIGHT | U8G2_DRAW_UPPER_LEFT);

    for (int t = 0; t < 2; t++)            // dotted tail: do both tail lines :AI ASSISTED
    {
      if (tailOn[t])                       // only if this tail line is on the radar :AI ASSISTED
      {
        for (int k = 8; k <= 58; k += 5)   // walk outward from 8 to 58 dots, one pixel every 5 dots :AI ASSISTED
        {
          // light one pixel along the tail line :AI ASSISTED
          display.drawPixel(64 + k * tailCos[t], 63 - k * tailSin[t]);
        }
      }
    }

    display.drawLine(64, 63, x, y);

    for (int i = 0; i < dotCount; i++)           // draw every dot we found :AI ASSISTED
    {
      display.drawDisc(dotX[i], dotY[i], 1);     // small filled dot in the middle, 1 dot in radius :AI ASSISTED
      display.drawCircle(dotX[i], dotY[i], 3);   // thin ring around it, 3 dots in radius :AI ASSISTED
    }

    if (nearestDots > 0)                   // only show the readout if something was detected :AI ASSISTED
    {
      display.setFont(u8g2_font_4x6_tr);   // pick a tiny font, 4 dots wide and 6 tall :AI ASSISTED
      display.setCursor(0, 6);             // start the text in the top-left corner :AI ASSISTED
      display.print(nearestInches);        // print the number of inches :AI ASSISTED
      display.print("in");                 // print the unit after it :AI ASSISTED
    }
  }
  while (display.nextPage());
}

void loop() // main loop 
{
  for (int angle = 0; angle <=180; angle += 2)    // starts the servo at 0 degrees and increments the angle by 2 until == 180
  {
    // moves the servo and waits
    radarServo.write(angle);           
    delay(15);
    // gets the distance and records a dot
    float distance = getDistance();
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);
    recordDot(angle, distance);
    drawRadar(angle);
  }
  for (int angle = 180; angle >= 0; angle -= 2)   // starts the servo at 180 and sweeps till 0 
  {
    // moves the servo and waits
    radarServo.write(angle);
    delay(15);
    // gets the distance and records a dot
    float distance = getDistance();
    Serial.print(angle);
    Serial.print(",");
    Serial.println(distance);
    recordDot(angle, distance);
    drawRadar(angle);
  }
}