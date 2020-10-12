#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include <AsyncUDP.h>
#include <WiFi.h>

MPU6050 mpu6050(Wire);

AsyncUDP udp;
//wifi name
const char * ssid = "TELLO-598E42";
const char * password = "";

const int joystickXPin = 2;
const int joystickYPin = 4;
const int potentiometerPin = 15;
const int buttonPinWhite = 23;
const int buttonPinBlue = 21;

boolean inAir = false;

int takeofflandcheck = 0;

void command(){
  String msg = "command";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
}

void setup() {
  Serial.begin(9600);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(buttonPinWhite, INPUT_PULLUP);
  pinMode(buttonPinBlue, INPUT_PULLUP);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);


  WiFi.mode(WIFI_STA);

  command();
  Serial.println("ready!");
}

void connect(){
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("Connected");
}

void takeoff(){
  String msg = "takeoff";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
}

void land(){
  String msg = "land";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
}

void loop() {

  if(digitalRead(buttonPinBlue) == LOW){
    Serial.println("Connecting...");
    connect();
    delay(1000);

  }
  
  while(digitalRead(buttonPinWhite) == LOW){
    if(!inAir){
      takeoff();
      inAir = true;
      Serial.println("Takeoff");
      delay(1000);
      break;
    }
    if(inAir){
     land();
     inAir = false;
      Serial.println("Land");
      delay(1000);
      break;
    }
  }
}
