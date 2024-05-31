// Bluetooth.cpp
#include "Bluetooth.h"
#include <Arduino.h>

int clutch_pin = 15;


#define AP_SERVICE_UUID   "ab0828b1-198e-4351-b779-901fa0e0371e"
#define UUID_RECEIVE      "4ac8a682-9736-4e5d-932b-e9b31405049c"

BLEService *Bluetooth::pService = nullptr;


//all ota stuff
#define OTA_SERVICE_UUID  "c8659210-af91-4ad3-a995-a58d6fd26145"
#define UUID_FW           "c8659211-af91-4ad3-a995-a58d6fd26145"
#define UUID_VERSION      "c8659212-af91-4ad3-a995-a58d6fd26145"

BLECharacteristic *Bluetooth::Version_Characteristic = nullptr;
BLECharacteristic *Bluetooth::FW_Characteristic = nullptr;

BLEService *Bluetooth::pOTAService = nullptr;

#define FULL_PACKET 512
#define CHARPOS_UPDATE_FLAG 5

esp_ota_handle_t otaHandler = 0;

bool updateFlag = false;
bool readyFlag = false;
int bytesReceived = 0;
int timesWritten = 0;

//end ota


#define LED 2



bool Bluetooth::deviceConnected = false;
bool Bluetooth::oldDeviceConnected = false;

BLEServer *Bluetooth::pServer = nullptr;  // Initialize the static member variable


class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    Bluetooth::deviceConnected = true;
    Serial.println("connected");
    digitalWrite(LED, HIGH);
  };

  void onDisconnect(BLEServer *pServer) {
    Bluetooth::deviceConnected = false;
    Serial.println("Disconnected");
    digitalWrite(LED, LOW);
    delay(1000);
    pServer->getAdvertising()->start();
    Motor::set(0, 0);
  }
};




class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    //if you get an error, you didnt install the esp32 boards manager. Go to tools->boards manager and search for esp32. Install the esp32 by Espressif Systems.
    std::string rxValue = pCharacteristic->getValue();

    if (rxValue.length() > 0) {
      String rxString = rxValue.c_str();
      Serial.print("Received Value: ");
      Serial.println(rxString);

      //do not change because there is old hardware.
      if (rxString.startsWith("motor")) {
        rxString.remove(0, 5);
        int motor_speed = rxString.toInt();
        int motor_direction = 1;
        if (motor_speed < 0) {
          motor_speed *= -1;
          motor_direction = 0;
        }
        Motor::set(motor_speed, motor_direction);
        return;
      }

      int splitpoint = rxString.indexOf(",");
      String key = rxString.substring(0, splitpoint);
      String value = rxString.substring(splitpoint+1);


      if (key == "apOn") {
        if(value == "0") digitalWrite(clutch_pin, LOW);
        else digitalWrite(clutch_pin, HIGH);; 
      }


    }
  }
};

class otaCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {

    std::string rxData = pCharacteristic->getValue();

      Serial.print("packet receiced size:");
  Serial.print(rxData.length());
  Serial.print(" value: ");
  Serial.println(rxData.c_str());


    if (!updateFlag) { //If it's the first packet of OTA since bootup, begin OTA
      Serial.println("BeginOTA");
      esp_ota_begin(esp_ota_get_next_update_partition(NULL), OTA_SIZE_UNKNOWN, &otaHandler);
      updateFlag = true;
    }
    if (rxData.length() > 0){
      esp_ota_write(otaHandler, rxData.c_str(), rxData.length());
      if (rxData.length() != FULL_PACKET)
      {
        esp_ota_end(otaHandler);
        Serial.println("EndOTA");
        if (ESP_OK == esp_ota_set_boot_partition(esp_ota_get_next_update_partition(NULL))) {
          delay(2000);
          esp_restart();
        }
        else {
          Serial.println("Upload Error");
        }
      }
    }
    

    uint8_t txData[5] = {1, 2, 3, 4, 5};
    //delay(1000);
    pCharacteristic->setValue((uint8_t*)txData, 5);
    pCharacteristic->notify();
  }
};


void Bluetooth::notify(BLECharacteristic *characteristic, String value) {
  static char charBuff[15];
  value.toCharArray(charBuff, 15);
  characteristic->setValue(charBuff);
  characteristic->notify();
}

void Bluetooth::setup() {

    pinMode(clutch_pin, OUTPUT);
    digitalWrite(clutch_pin, LOW);


  BLEDevice::init("AutoPilot");
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  pService = pServer->createService(AP_SERVICE_UUID);




  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(UUID_RECEIVE, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pRxCharacteristic->setCallbacks(new MyCallbacks());

  pService->start();


  //OTA
  pOTAService = pServer->createService(OTA_SERVICE_UUID);

  Version_Characteristic = pOTAService->createCharacteristic(UUID_VERSION, BLECharacteristic::PROPERTY_READ);
  Version_Characteristic->addDescriptor(new BLE2902());

  FW_Characteristic = pOTAService->createCharacteristic(UUID_FW,BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE| BLECharacteristic::PROPERTY_WRITE_NR);
  FW_Characteristic->addDescriptor(new BLE2902());
  FW_Characteristic->setCallbacks(new otaCallback());

  pOTAService->start();



  pServer->getAdvertising()->addServiceUUID(OTA_SERVICE_UUID);

  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  uint8_t firmwareVersion[1] = {FIRMWARE_VERSION};
  Version_Characteristic->setValue((uint8_t*)firmwareVersion, 1);

    pinMode(LED, OUTPUT);


}

void Bluetooth::loop() {

  if (!deviceConnected && oldDeviceConnected) {
    delay(500);

    pServer->startAdvertising();
    Serial.println("start advertising");
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
    Serial.println("Connecting...");
  }
}




