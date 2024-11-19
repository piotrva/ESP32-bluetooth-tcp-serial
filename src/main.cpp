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
// Bluetooth Serial
//-----------------------------------------------------------------------------
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_TX   "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_RX   "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

BLECharacteristic *txCharacteristic;
BLECharacteristic *rxCharacteristic;

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            Serial.println("Received data: " + String(value.c_str()));
            // Echo the received data back as a notification (optional)
            pCharacteristic->setValue(value);
            pCharacteristic->notify();
        }
    }
};

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* server) {
        Serial.println("Client connected");
        leds[0].setRGB(0, 0, 255);  // Blue for connection
        FastLED.show();
    }

    void onDisconnect(BLEServer* server) {
        Serial.println("Client disconnected");
        leds[0].setRGB(255, 0, 0);  // Red for disconnection
        FastLED.show();
        server->startAdvertising();  // Restart advertising
    }
};

void setupBLE(void)
{
  BLEDevice::init("ESP32-C3-BLE");

  BLEServer *server = BLEDevice::createServer();
  server->setCallbacks(new MyServerCallbacks()); // Set connection callbacks

  BLEService *service = server->createService(SERVICE_UUID);

  rxCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_RX,
      BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY
  );
  rxCharacteristic->addDescriptor(new BLE2902());
  rxCharacteristic->setCallbacks(new MyCallbacks());

  txCharacteristic = service->createCharacteristic(
      CHARACTERISTIC_TX,
      BLECharacteristic::PROPERTY_NOTIFY
  );
  txCharacteristic->addDescriptor(new BLE2902());

  service->start();
  BLEAdvertising *advertising = BLEDevice::getAdvertising();
  advertising->addServiceUUID(SERVICE_UUID);
  advertising->start();

  Serial.println("BLE server started, waiting for connections...");
  leds[0].setRGB(0, 255, 0);  // Green for advertising
  FastLED.show();
}

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
  
  leds[0].setRGB(0, 255, 0);
  FastLED.show();
  setupBLE();
}

void loop()
{
  // Example: Sending data to the client
    static int counter = 0;
    txCharacteristic->setValue(String(counter++).c_str());
    txCharacteristic->notify(); // Notify client of new data
    delay(1000);
}
