/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include <CurieBLE.h>

#define BLE_LOCAL_NAME "qri_imu_bcast"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_ACCEL_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_GYRO_UUID  "30ab0001-b5a3-0f93-e0a9-e50e24dc3993"

typedef struct sensing_accel {
    uint32_t stamp;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} sensing_accel_t;

typedef struct sensing_gyro {
    uint32_t stamp;
    uint16_t x;
    uint16_t y;
    uint16_t z;
} sensing_gyro_t;


#define SENSOR_ACCEL_MAX_LEN 12
#define SENSOR_GYRO_MAX_LEN 12
BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService imuService(SENSOR_UUID); 
BLECharacteristic accelChar(SENSOR_ACCEL_UUID,BLERead | BLENotify,SENSOR_ACCEL_MAX_LEN);
BLECharacteristic gyroChar(SENSOR_GYRO_UUID,BLERead | BLENotify,SENSOR_GYRO_MAX_LEN);

#define LED 13

void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(LED, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName(BLE_LOCAL_NAME);
  blePeripheral.setAdvertisedServiceUuid(imuService.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(imuService);
  blePeripheral.addAttribute(accelChar);
  blePeripheral.addAttribute(gyroChar);

  // begin advertising BLE service:
  blePeripheral.begin();
  
  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      //TODO:update the imu data to server
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}

