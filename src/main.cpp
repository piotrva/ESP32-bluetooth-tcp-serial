#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 10
#define BRIGHTNESS 32

CRGB leds[NUM_LEDS];

void setup()
{
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  memset8(leds, 0, NUM_LEDS * sizeof(CRGB));
  FastLED.show();
}

void loop()
{
  delay(1000);
  leds[0].setRGB(0, 0, 255);
  FastLED.show();
  delay(1000);
  leds[0].setRGB(0, 255, 0);
  FastLED.show();
  delay(1000);
  leds[0].setRGB(255, 0, 0);
  FastLED.show();
}
