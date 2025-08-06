#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define RGB_PIN 48
#define NPIXELS 1
#define NEOPXL_BRIGHTNESS 100

Adafruit_NeoPixel pixels(NPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);

int delay_ms = 20; 

void setup(){
  Serial.begin(115200);
  pixels.begin();
}

void loop(){
  
  for (long H=0; H < 5*16<<16; H+=256) {
    //pixels.clear();
    pixels.rainbow(H,1,255,NEOPXL_BRIGHTNESS);
    pixels.show();
    delay(delay_ms); 
  }
} 

