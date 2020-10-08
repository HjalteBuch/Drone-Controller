#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

MPU6050 mpu6050(Wire);

int joystickXPin = 2;
int joystickYPin = 4;
int potentiometerPin = 15;

void setup() {
  Serial.begin(9600);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(potentiometerPin, INPUT);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);

}

void loop() {
  Serial.print("Throttle: ");
  Serial.print(analogRead(potentiometerPin));
  
  Serial.print(", Yaw: ");
  Serial.print(analogRead(joystickXPin));

  mpu6050.update();
  Serial.print(", Roll : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print(", Pitch : ");
  Serial.println(mpu6050.getAngleY());
  

}