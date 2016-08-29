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
#include "task.h"
#include "internal/ble_client.h"
#define BLE_DISCONNECT_REASON_LOCAL_TERMINATION 0x16
#define BLE_LOCAL_NAME "qri_imu_bcast"

#define SENSOR_UUID       "00ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_IMU_UUID "20ab0001-b5a3-0f93-e0a9-e50e24dc3993"
#define SENSOR_BEAT_UUID "30ab0001-b5a3-0f93-e0a9-e50e24dc3993"

enum BLE_TASK_ID {
  BLE_TASK_BEAT=0,
  BLE_TASK_IMU=1,
  BLE_TASK_NUM
};
Task g_bleTask[BLE_TASK_NUM];

enum NONE_BLE_TASK_ID {
  NONE_BLE_TASK_LED=0,
  NONE_BLE_TASK_NUM
};
Task g_noBleTask[NONE_BLE_TASK_NUM];


#define SENSOR_IMU_MAX_LEN 17
BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)
BLEService imuService(SENSOR_UUID); 
BLECharacteristic imuChar(SENSOR_IMU_UUID,BLERead | BLENotify,SENSOR_IMU_MAX_LEN);
BLEUnsignedCharCharacteristic beatChar(SENSOR_BEAT_UUID,BLERead | BLEWrite);


#define TASK_INTERVAL_IMU 30000
typedef struct sensing_imu {
    uint32_t stamp;
    int16_t ax;
    int16_t ay;
    int16_t az;
    int16_t gx;
    int16_t gy;
    int16_t gz;
} sensing_imu_t;
int ax, ay, az;         // accelerometer values
int gx, gy, gz;         // gyrometer values
sensing_imu_t g_stImu;
int g_imuFrameCount=0;
void taskBleUpdateIMUReset(){
  g_imuFrameCount = 0;
}
void taskBleUpdateIMURun(){
    CurieIMU.readMotionSensor(ax, ay, az, gx, gy, gz);
    g_imuFrameCount++;
    g_stImu.stamp = g_imuFrameCount;
    g_stImu.ax = (int16_t)ax;
    g_stImu.ay = (int16_t)ay;
    g_stImu.az = (int16_t)az;
    g_stImu.gx = (int16_t)gx;
    g_stImu.gy = (int16_t)gy;
    g_stImu.gz = (int16_t)gz;
    unsigned char * pst_data = (unsigned char*)(&g_stImu);
    unsigned short data_len = sizeof(g_stImu);
    imuChar.setValue(pst_data,data_len); 
}

#define TASK_INTERVAL_BEAT 1000000 //1000ms
#define TASK_BEAT_COUNTER_RESET 5
int g_bleBeatCounter=TASK_INTERVAL_BEAT;

void taskBleUpdateBeatReset(){
  g_bleBeatCounter = TASK_BEAT_COUNTER_RESET;
}
void taskBleUpdateBeatRun(){
  if(beatChar.written()){
    //currently do not check the value.
    g_bleBeatCounter = TASK_BEAT_COUNTER_RESET;
  }
  //update the beat
  g_bleBeatCounter = g_bleBeatCounter-1;
 if(g_bleBeatCounter<0 ){
    //Serial.println("Ble connection timeout. Disconnect.");
    g_bleBeatCounter = TASK_BEAT_COUNTER_RESET;
     //ble_client_gap_disconnect(BLE_DISCONNECT_REASON_LOCAL_TERMINATION);
     //delay(300);
  }
}

#define TASK_INTERVAL_LED 500000 //500ms
#define LED 13
int g_ledLastState=0;
void taskLedReset(){
  g_ledLastState = 0;
}
void taskLedRun(){
  g_ledLastState = !g_ledLastState;
  digitalWrite(LED,g_ledLastState);
}
void TaskNoneBleReset()
{
  g_noBleTask[NONE_BLE_TASK_LED].reset();
}
void TaskNoneBleRun(unsigned long us)
{
  g_noBleTask[NONE_BLE_TASK_LED].trigger(us);
}
void TaskBleReset()
{
  g_bleTask[BLE_TASK_BEAT].reset();
  g_bleTask[BLE_TASK_IMU].reset();
}
void TaskBleRun(unsigned long us)
{
  g_bleTask[BLE_TASK_BEAT].trigger(us);
  g_bleTask[BLE_TASK_IMU].trigger(us);
}


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
  blePeripheral.addAttribute(beatChar);

  // begin advertising BLE service:
  blePeripheral.begin();
  
  Serial.println("BLE LED Peripheral");

  CurieIMU.begin();
  CurieIMU.autoCalibrateGyroOffset();

  CurieIMU.autoCalibrateAccelerometerOffset(X_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Y_AXIS, 0);
  CurieIMU.autoCalibrateAccelerometerOffset(Z_AXIS, 0);

  //init ble task
  g_bleTask[BLE_TASK_BEAT].init(TASK_INTERVAL_BEAT,taskBleUpdateBeatRun,taskBleUpdateBeatReset);
  g_bleTask[BLE_TASK_IMU].init(TASK_INTERVAL_IMU,taskBleUpdateIMURun,taskBleUpdateIMUReset);
  
  //init no ble task
  g_noBleTask[NONE_BLE_TASK_LED].init(TASK_INTERVAL_LED,taskLedRun,taskLedReset);

  //reset all
  TaskNoneBleReset();
  TaskBleReset();
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    digitalWrite(LED,HIGH);
    //reset ble tasks
    TaskBleReset();
    // while the central is still connected to peripheral:
    while (central.connected()) {
      // if the remote device wrote to the characteristic,
      // use the value to control the LED:
      //TODO:update the imu data to server
      unsigned long us = micros();
      TaskBleRun(us);
      delayMicroseconds(500);
    }

    //reset the no ble tasks
    TaskNoneBleReset();
    
    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }else{
    unsigned long us = micros();
    TaskNoneBleRun(us);
  }
  delay(5);
}

