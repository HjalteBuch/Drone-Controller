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
const int joystickYPin = 36;
const int potentiometerPin = 32;
const int buttonPinWhite = 23;
const int buttonPinBlue = 5;



boolean inAir = false;

int takeofflandcheck = 0;

void command(){
  String msg = "command";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
}

//virker kun første gang
//hvis dronen bliver disconnected, kan den ikke connecte til dronen igen.
void connect(){
  Serial.println("Connecting...");
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Failed");
    return;
  }
  if (WiFi.waitForConnectResult() == WL_CONNECTED) {
    command();
    Serial.println("Connected");
  }
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

  Serial.println("ready!");
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


//når potentiometer står stille køre den skiftevis "descending" og "ascending", men den fatter mens man drejer
int prev = analogRead(potentiometerPin);
void upAndDown(){
  int ascend = analogRead(potentiometerPin) / 9 + 20;

  if(ascend < prev-2){
    String msg = "down" + ascend;
    udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
    Serial.println("descending");
  }

  if(ascend > prev+2){
    String msg = "up" + ascend;
    udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
    Serial.println("ascending");
  }
  
  prev = ascend;
}

void loop() {

  if(digitalRead(buttonPinBlue) == LOW){
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
  upAndDown();

}
