#include <Time.h>

/* http://www.arduino.cc/en/Tutorial/Ping */

const int pingPin = 7;
const int ledPin = 9;

const int DEBUG = 1;

const int maxDist = 255; //Maximum distance in centimeters - Light is OFF at this distance (and brightest just before this distance)
const int minDist = 2; //Minimum distance in centimeters - Light is off at this distance, and ramps up as you go farther away
const int refreshRate = 1; //How often do we update the light brightness? in milliseconds
const int refreshLongRate = 25; //How often do we update the light brightness? in milliseconds

int brightnessLast;
int lastUpdate;
int dark = 1; // 1 for darkness, 0 for lightness

void setup() {
  // initialize serial communication:
  if (DEBUG) {
    Serial.begin(9600);
  }
  pinMode(ledPin, OUTPUT);
}

void loop()
{  
  long duration, inches, cm;
  
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(5000);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5000);
  digitalWrite(pingPin, LOW);

  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);
  
  if ((cm <= maxDist) && (cm >= minDist)) { // if motion is in range
    if (dark) {
      dimIn(cm);
    } else if (((cm > brightnessLast) && ((cm - 50) < brightnessLast)) || ((cm < brightnessLast) && ((cm + 50) > brightnessLast))) {
      // updates the light if it's already on, trying to eliminate noise spikes
      updateLED(cm);
    }
    dark = 0;
  }
  
  if (second(now()) > (lastUpdate + 5)) { // turn off after no activity
    dimOut(brightnessLast);
    dark = 1;
  }
  
  if (DEBUG) { // output to the serial monitor
    Serial.print(inches);
    Serial.print(" inches, ");
    Serial.print(cm);
    Serial.print(" cm, ");
    Serial.print(lastUpdate);
    Serial.print(" update");
    Serial.println();
  }
  
  
  delay(refreshRate);
}

void updateLED(int brightness) {
    analogWrite(ledPin, brightness);
    
  Serial.print(brightness);
  Serial.print(" LED, ");
}


void dimIn(int brightness) { // gradually bring the brightness up
  for(int x = 0; x <= brightness; x++) { 
    analogWrite(ledPin, x);
    delay(refreshLongRate);
    brightnessLast = brightness;
  }
  lastUpdate = second(now());
}

void dimOut(int brightness) { // gradually bring the brightness down
  
  if (DEBUG) {
    Serial.print(brightness);
    Serial.print(" dimness");
  }
  
  for(int x = brightness; x >= 0; x--) {
    analogWrite(ledPin, x);
    delay(refreshLongRate);
    brightnessLast = brightness;
  } 
  lastUpdate = second(now());
}




/* Conversions */

long microsecondsToInches(long microseconds)
{
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}
