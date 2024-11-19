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
#include <NimBLEDevice.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */  
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
  /***************** New - Security handled here ********************
  ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest(){
      Serial.println("Server PassKeyRequest");
      return 123456; 
    }

    bool onConfirmPIN(uint32_t pass_key){
      Serial.print("The passkey YES/NO number: ");Serial.println(pass_key);
      return true; 
    }

    void onAuthenticationComplete(ble_gap_conn_desc desc){
      Serial.println("Starting BLE work!");
    }
  /*******************************************************************/
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++)
          Serial.print(rxValue[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

void setupBLE(void)
{
  // Create the BLE Device
  BLEDevice::init("UART Service");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
                                        CHARACTERISTIC_UUID_TX,
                                    /******* Enum Type NIMBLE_PROPERTY now *******      
                                        BLECharacteristic::PROPERTY_NOTIFY
                                        );
                                    **********************************************/  
                                        NIMBLE_PROPERTY::NOTIFY
                                       );
                                    
  /***************************************************   
   NOTE: DO NOT create a 2902 descriptor 
   it will be created automatically if notifications 
   or indications are enabled on a characteristic.
   
   pCharacteristic->addDescriptor(new BLE2902());
  ****************************************************/                  

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
                                            CHARACTERISTIC_UUID_RX,
                                    /******* Enum Type NIMBLE_PROPERTY now *******       
                                            BLECharacteristic::PROPERTY_WRITE
                                            );
                                    *********************************************/  
                                            NIMBLE_PROPERTY::WRITE
                                            );

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void handleBLE(void)
{
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void txBLE(char c)
{
  if (deviceConnected) {
        pTxCharacteristic->setValue((uint8_t*)&c, 1);
        pTxCharacteristic->notify();
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    }
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
  handleBLE();
  if (Serial.available())
  {
    char c = Serial.read();
    txBLE(c);
  }
}
