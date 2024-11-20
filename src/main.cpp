#include <Arduino.h>

//-----------------------------------------------------------------------------
// Bluetooth Serial
//-----------------------------------------------------------------------------
#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

//-----------------------------------------------------------------------------
// Serial hardware
//-----------------------------------------------------------------------------
#define BUFFER_SIZE 512
uint8_t buffer[BUFFER_SIZE];
uint32_t bufferCnt = 0;

#define RXD2 16
#define TXD2 17

#define RXD1 4
#define TXD1 2

//-----------------------------------------------------------------------------
// Setup
//-----------------------------------------------------------------------------
void setup()
{
  Serial.begin(115200);  // USB serial
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // PM3
  Serial2.setRxBufferSize(1024);
  SerialBT.begin("ESP32test");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  if (Serial2.available()) {
    while(Serial2.available())
    {
      buffer[bufferCnt++] = Serial2.read();
      if(bufferCnt >= BUFFER_SIZE)
      {
        break;
      }
    }
    SerialBT.write(buffer, bufferCnt);
    bufferCnt = 0;
  }
  if (SerialBT.available()) {
    Serial2.write(SerialBT.read());
  }
}
