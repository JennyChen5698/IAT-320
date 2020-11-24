//Built on top of http://learn.adafruit.com/led-ampli-tie

#include <Adafruit_NeoPixel.h>

#define PIN   6
#define pixelcount     60
#define delayval       10
#define brightval      20

#include <Adafruit_CircuitPlayground.h>

#include <Wire.h>
#include <SPI.h>
#include <math.h>


Adafruit_NeoPixel pixels = Adafruit_NeoPixel(pixelcount, PIN, NEO_GRB + NEO_KHZ800);


#define MIC_PIN         A4  // Microphone is attached to this analog pin (A4 for circuit playground)
#define SAMPLE_WINDOW   10  // Sample window for average level
#define PEAK_HANG       24  // Time of pause before peak dot falls
#define PEAK_FALL        4  // Rate of falling peak dot
#define INPUT_FLOOR     10  // Lower range of analogRead input
#define INPUT_CEILING  200  // Max range of analogRead input, the lower the value the more sensitive (1023 = max)


byte peak = 5;        // Peak level of column; used for falling dots
byte Cpeak = 16;
unsigned int sample;
byte dotCount = 0;     //Frame counter for peak dot
byte dotHangCount = 0; //Frame counter for holding peak dot
byte CdotCount = 0;     //Frame counter for peak dot
byte CdotHangCount = 0; //Frame counter for holding peak dot

float fscale(float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve);

void setup() 
{
  CircuitPlayground.begin();
  
  Serial.begin(9600);

  pixels.begin(); // This initializes the NeoPixel library.
  pixels.show();

}

void loop() 
{
  //int numPixels = CircuitPlayground.strip.numPixels();
 
  int xnumPixels = 60;
  int xxnumPixels = 10;
  
  unsigned long startMillis= millis();  // Start of sample window
  float peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1023;
  unsigned int c, y, Cc, Cy;

  // collect data for length of sample window (in mS)
  while (millis() - startMillis < SAMPLE_WINDOW)
  {
    sample = analogRead(MIC_PIN);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
 
  //Serial.println(peakToPeak); 

  //Fill the strip with rainbow gradient
  for (int g=0;g<=xxnumPixels-1;g++){
    CircuitPlayground.strip.setPixelColor(g,Wheel(map(g,0,xnumPixels-1,30,150)));
  }
   for (int i=0;i<=xnumPixels-1;i++){
                     pixels.setPixelColor(i,Wheel(map(i,0,xnumPixels-1,30,150)));
                     
  }

  //Scale the input logarithmically instead of linearly
  c = fscale(INPUT_FLOOR, INPUT_CEILING, xnumPixels, 0, peakToPeak, 2);
  Cc = fscale(INPUT_FLOOR, INPUT_CEILING, xxnumPixels, 0, peakToPeak, 2);
  
  // Turn off pixels that are below volume threshold.
  if(c < peak) {
    peak = c;        // Keep dot on top
    dotHangCount = 0;    // make the dot hang before falling
  }
  if (c <= xnumPixels) { // Fill partial column with off pixels
    drawLine(xnumPixels, xnumPixels-c, CircuitPlayground.strip.Color(0, 0, 0));
  }

  if(Cc < Cpeak) {
    Cpeak = Cc;        // Keep dot on top
    CdotHangCount = 0;    // make the dot hang before falling
  }
  if (Cc <= xxnumPixels) { // Fill partial column with off pixels
    CdrawLine(xxnumPixels, xxnumPixels-Cc, CircuitPlayground.strip.Color(0, 0, 0));
  }


  // Set the peak dot to match the rainbow gradient
  y = xnumPixels - peak;
  Cy = xxnumPixels - Cpeak;
  Serial.println(y);
  
  CircuitPlayground.strip.setPixelColor(Cy-1,Wheel(map(Cy,0,xxnumPixels-1,30,150)));
  CircuitPlayground.strip.show();

                    pixels.setPixelColor(y-1,Wheel(map(y,0,xnumPixels-1,30,150)));
                    pixels.show();


  delay(10);
  
  // Frame based peak dot animation
  if(dotHangCount > PEAK_HANG) { //Peak pause length
    if(++dotCount >= PEAK_FALL) { //Fall rate 
      peak++;
      dotCount = 0;
    }
  } 
  else {
    dotHangCount++; 
  }



 if(CdotHangCount > PEAK_HANG) { //Peak pause length
    if(++CdotCount >= PEAK_FALL) { //Fall rate 
      Cpeak++;
      CdotCount = 0;
    }
  } 
  else {
    CdotHangCount++; 
  }

}

//Used to draw a line between two points of a given color
void drawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
 //   CircuitPlayground.strip.setPixelColor(i, c);
                     pixels.setPixelColor(i, c);                
  }
}
void CdrawLine(uint8_t from, uint8_t to, uint32_t c) {
  uint8_t fromTemp;
  if (from > to) {
    fromTemp = from;
    from = to;
    to = fromTemp;
  }
  for(int i=from; i<=to; i++){
    CircuitPlayground.strip.setPixelColor(i, c);

  }
}

float fscale( float originalMin, float originalMax, float newBegin, float
newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale - this seems more intuitive - postive numbers give more weight to high end on output 
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){ 
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd; 
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine 
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {   
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange); 
  }

  return rangedValue;
}


// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
    return CircuitPlayground.strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } 
  else if(WheelPos < 170) {
    WheelPos -= 85;
    return CircuitPlayground.strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } 
  else {
    WheelPos -= 170;
    return CircuitPlayground.strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
