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
#include "CurieIMU.h"

#define BLE_LOCAL_NAME "qri_imu_bcast"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_ACCEL_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_GYRO_UUID  "30ab0001-b5a3-0f93-e0a9-e50e24dc3993"

typedef struct sensing_accel {
    uint32_t stamp;
    int16_t x;
    int16_t y;
    int16_t z;
} sensing_accel_t;

typedef struct sensing_gyro {
    int32_t stamp;
    int16_t x;
    int16_t y;
    int16_t z;
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

  CurieIMU.begin();
  CurieIMU.autoCalibrateGyroOffset();

  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 0);
}

int last_led_state=0;
int ax, ay, az;         // accelerometer values
int gx, gy, gz;         // gyrometer values
sensing_accel_t st_accel;
sensing_gyro_t  st_gyro;
int count=0;
void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    digitalWrite(LED,HIGH);
    count = 0;
    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      //TODO:update the imu data to server
      CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);
      count++;
      st_accel.stamp = count;
      st_accel.x = (int16_t)ax;
      st_accel.y = (int16_t)ay;
      st_accel.z = (int16_t)az;
      
      st_gyro.stamp = count;
      st_gyro.x = (int16_t)gx;
      st_gyro.y = (int16_t)gy;
      st_gyro.z = (int16_t)gz;

      unsigned char * pst_data = (unsigned char*)(&st_accel);
      unsigned short data_len = sizeof(st_accel);
      accelChar.setValue(pst_data,data_len);
      
      pst_data = (unsigned char*)(&st_gyro);
      data_len = sizeof(st_gyro);
      gyroChar.setValue(pst_data,data_len);
      delay(30);
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }else{
    last_led_state=!last_led_state;
    digitalWrite(LED,last_led_state);
    delay(500);
    count = 0;
  }
}

