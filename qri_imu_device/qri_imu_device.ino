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
#define SENSOR_IMU_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"

typedef struct sensing_imu {
    uint32_t stamp;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} sensing_imu_t;


#define SENSOR_IMU_MAX_LEN 17
BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService imuService(SENSOR_UUID); 
BLECharacteristic imuChar(SENSOR_IMU_UUID,BLERead | BLENotify,SENSOR_IMU_MAX_LEN);

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
  blePeripheral.addAttribute(imuChar);

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
sensing_imu_t st_imu;
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
      st_imu.stamp = count;
      st_imu.ax = (int16_t)ax;
      st_imu.ay = (int16_t)ay;
      st_imu.az = (int16_t)az;
      st_imu.gx = (int16_t)gx;
      st_imu.gy = (int16_t)gy;
      st_imu.gz = (int16_t)gz;

      unsigned char * pst_data = (unsigned char*)(&st_imu);
      unsigned short data_len = sizeof(st_imu);
      imuChar.setValue(pst_data,data_len);
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

