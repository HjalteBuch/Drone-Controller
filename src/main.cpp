#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

int joystickXPin = 2;
int joystickYPin = 4;

void setup() {
  Serial.begin(9600);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);

}

void loop() {
  Serial.print("X: ");
  Serial.print(analogRead(joystickXPin));
  Serial.print(", Y: ");
  Serial.println(analogRead(joystickYPin));

}