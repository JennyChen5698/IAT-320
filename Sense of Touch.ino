#include <Adafruit_CircuitPlayground.h>

int vals[8] = {3,2,0,1,6,9,10,12};
float pitches[8] = {261.626, 293.665, 329.628, 349.288, 391.955, 440.0, 493.883, 523.251};

void setup() {
  Serial.begin(9600);
  CircuitPlayground.begin();
}

void loop() {
  for (int i=0; i<8; i++){
    int x = (CircuitPlayground.readCap(vals[i]));
    if (x > 600){
      CircuitPlayground.playTone(pitches[i], 100);
      CircuitPlayground.setPixelColor(i,249, 168, 0);
    }
    else{
      CircuitPlayground.setPixelColor(i,0,0,0);
    }
  }

  delay(100);

}
