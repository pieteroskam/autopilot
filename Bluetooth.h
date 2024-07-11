// Bluetooth.h
#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "Arduino.h"
#include "Motor.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include "esp_ota_ops.h"

#define FIRMWARE_VERSION 2


class Bluetooth {
public:
  static void setup();
  static void loop();
  static void notify(BLECharacteristic *characteristic, String value);
  static bool deviceConnected;

  static BLECharacteristic *pRxCharacteristic;

private:
  static BLEServer *pServer;
  static BLEService *pService;

//ota
  static BLEService *pOTAService;
  static BLECharacteristic * Version_Characteristic;
  static BLECharacteristic * FW_Characteristic;

  static bool oldDeviceConnected;
};



#endif
