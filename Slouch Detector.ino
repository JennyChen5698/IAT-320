// Circuit Playground Slouch Detector v3
//
// Push button(s) to set a target angle.
// Compute current angle using accelerometer and compare
// to preset slouch angle. Sound alarm if slouching after
// a preset period of time.
//
// Author: Carter Nelson
// MIT License (https://opensource.org/licenses/MIT)


#include <Adafruit_CircuitPlayground.h>

#define SLOUCH_ANGLE        10.0      // allowable slouch angle (deg)
#define SLOUCH_TIME         3000      // allowable slouch time (secs) 
#define GRAVITY             9.80665   // standard gravity (m/s^s)
#define RAD2DEG             52.29578  // convert radians to degrees

float currentAngle;
float targetAngle;
unsigned long slouchStartTime;
bool slouching;

void setup() {
  //Initialize Serial
  Serial.begin(9600);
  
  // Initialize Circuit Playground
  CircuitPlayground.begin();

  // Initialize target angle to zero.
  targetAngle = 0;
}

void loop() {
  // Compute current angle
  currentAngle = RAD2DEG * asin(-CircuitPlayground.motionZ() / 9.81);

  // Set target angle on button press
  if ((CircuitPlayground.leftButton()) || (CircuitPlayground.rightButton())) {
    targetAngle = currentAngle;
    CircuitPlayground.playTone(900,100);
    delay(100);
    CircuitPlayground.playTone(900,100);
    delay(100);
  }
  
  // Check for slouching
  if (currentAngle - targetAngle > SLOUCH_ANGLE) {
    if (!slouching) slouchStartTime = millis();
    slouching = true;
  } else {
    slouching = false;
  }

  // If we are slouching
  if (slouching) {
    // Check how long we've been slouching
    if (millis() - slouchStartTime > SLOUCH_TIME) {
      // Play a tone
      CircuitPlayground.playTone(800, 500);  
      // Light up all LEDs in Red when slouching
      for (int i=0; i<10; i++) {
      CircuitPlayground.setPixelColor(i, 255, 0, 0);
      }
    }     
    // Yellow LEDs when it's not slouching
  delay(200);
    for (int i=0; i<10; i++) {
      CircuitPlayground.setPixelColor(i, 15, 15, 0);
    }
  }
 }
