/*-----------------------------------------------------------------------------
 * This code was developed with assistance from OpenAI's ChatGPT
 * and is published under the GPL 3.0 license.
 * Author: Piotr Rzeszut
-----------------------------------------------------------------------------*/

#include <Arduino.h>

//-----------------------------------------------------------------------------
// LED
//-----------------------------------------------------------------------------
#include <FastLED.h>

#define NUM_LEDS 1
#define DATA_PIN 10
#define BRIGHTNESS 32

CRGB leds[NUM_LEDS];

//-----------------------------------------------------------------------------
// WiFi
//-----------------------------------------------------------------------------
#include <WiFi.h>

const char* ssid     = "ESP32-Access-Point";
const char* password = "123456789";

WiFiServer server(4321);

//-----------------------------------------------------------------------------
// Serial buffer
//-----------------------------------------------------------------------------
#define BUFFER_SIZE 512
uint8_t buffer[BUFFER_SIZE];
uint32_t bufferCnt = 0;

//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------
void setup()
{
  FastLED.addLeds<WS2811, DATA_PIN, RGB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  memset8(leds, 0, NUM_LEDS * sizeof(CRGB));
  FastLED.show();
  Serial.begin(115200);  // USB-CDC serial
  //                                RX  TX
  Serial0.begin(115200, SERIAL_8N1, 20, 21);  // Serial0
  Serial1.begin(115200, SERIAL_8N1,  1,  0);  // Serial1 - by default does not print boot messages
  Serial1.setRxBufferSize(128);
  Serial1.setTxBufferSize(128);
  
  leds[0].setRGB(255, 255, 255);
  FastLED.show();

  WiFi.softAP(ssid, password);
  server.begin();
  leds[0].setRGB(0, 0, 255);
  FastLED.show();
}

void loop()
{
  WiFiClient client = server.available();
  if (client)
  {
    leds[0].setRGB(0, 255, 0);
    FastLED.show();
    while (client.connected())
    {
      if (client.available())
      {
        Serial1.write(client.read());
      }
      if (Serial1.available())
      {
        while (Serial1.available())
        {
          buffer[bufferCnt++] = Serial1.read();
          if (bufferCnt >= BUFFER_SIZE)
          {
            break;
          }
        }
        client.write(buffer, bufferCnt);
        bufferCnt = 0;
      }
    }
    client.stop();
    leds[0].setRGB(255, 0, 0);
    FastLED.show();
  }
}
