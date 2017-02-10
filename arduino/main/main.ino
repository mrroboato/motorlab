// main.ino

#include <string.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include "QuickStats.h"

QuickStats stats;
Adafruit_BNO055 bno = Adafruit_BNO055(55);

byte byteRead;
char imuData[20];

int potPin = 1; // Potentiometer input pin.
// SCL = 3; // IMU input pin.
// SDA = 3; // IMU input pin.
int irPin = 2; // Ir proximity sensor input pin.

void setup() {

    if(!bno.begin()) {
      /* There was a problem detecting the BNO055 ... check your connections */
      Serial.println("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
      while(1);
    } 

    sensor_t sensor;
    bno.getSensor(&sensor);
  
    Serial.begin(115200);
}

void loop() {
    int pot = readPotentiometer();
    readImuSensor();
    int ir = readIRProximitySensor();

    sendSensorData(pot, imuData, ir);
    delay(50);
}

void sendSensorData(int pot, char *imu, int ir) {
    char messageBuf[50];
    sprintf(messageBuf, "*Pot:%d,Imu:%s,Ir:%d#", pot, imu, ir);
    Serial.write(messageBuf);
}

int readPotentiometer() {
    return analogRead(potPin); 
}

char *readImuSensor() {
    sensors_event_t event;
    bno.getEvent(&event);

    int orient_x = (int)event.orientation.x;
    int orient_y = (int)event.orientation.y;
    int orient_z = (int)event.orientation.z;
    
//    uint8_t sys, gyro, accel, mag = 0;
//    bno.getCalibration(&sys, &gyro, &accel, &mag);
    
//    sprintf(imuData, "%d;%d;%d;%d;%d;%d;%d", orient_x, orient_y, orient_z, sys, gyro, accel, mag);
      sprintf(imuData, "%d;%d;%d;", orient_x, orient_y, orient_z);
}

int readIRProximitySensor() {
    int n = 20; //window size
    float sig[n];
    
    for (int i=0; i<n; i++){
      sig[i] = analogRead(irPin);
    }
  
    float x = stats.median(sig,n);
    float distance = .00036008*x*x - .28975*x + 68.567;

    return distance;
}

