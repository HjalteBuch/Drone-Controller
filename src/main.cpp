#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include <AsyncUDP.h>
#include <WiFi.h>
#include <string>

MPU6050 mpu6050(Wire);

AsyncUDP udp;
//wifi name
const char *ssid = "TELLO-598E42";
const char *password = "";

//Pin numbers bliver defineret her
const int joystickXPin = 2;
const int joystickYPin = 36;
const int potentiometerPin = 32;
const int buttonPinWhite = 23;
const int buttonPinBlue = 5;

//til potentiometeret
int prevalue = 0;
const int tollerance = 2;

int rollDeadzone;
int pitchDeadzone;
int yawDeadzone;

//takeoff / land check
boolean inAir = false;

int takeofflandcheck = 0;

void sendmsg(String msg){
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
  Serial.println(msg);
}

void command()
{
  sendmsg("command");
}

//virker kun første gang
//hvis dronen bliver disconnected, kan den ikke connecte til dronen igen.
void connect()
{
  if (digitalRead(buttonPinBlue) == LOW)
  {
    Serial.println("Connecting...");
    WiFi.begin(ssid, password);
    if (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      Serial.println("WiFi Failed");
      return;
    }
    if (WiFi.waitForConnectResult() == WL_CONNECTED)
    {
      command();
      Serial.println("Connected");
    }
    delay(1000);
  }
}

void setup()
{
  Serial.begin(9600);
  pinMode(joystickXPin, INPUT);
  pinMode(joystickYPin, INPUT);
  pinMode(potentiometerPin, INPUT);
  pinMode(buttonPinWhite, INPUT_PULLUP);
  pinMode(buttonPinBlue, INPUT_PULLUP);

  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);


  mpu6050.update();
  rollDeadzone = mpu6050.getAngleX();
  pitchDeadzone = mpu6050.getAngleY();
  yawDeadzone = analogRead(joystickXPin);

  WiFi.mode(WIFI_STA);

  Serial.println("ready!");
}

void takeoff()
{
  sendmsg("takeoff");
}

void land()
{
  sendmsg("land");
}

void takeoffLand()
{
  while (digitalRead(buttonPinWhite) == LOW)
  {
    if (!inAir)
    {
      takeoff();
      inAir = true;
      Serial.println("Takeoff");
      delay(1000);
      break;
    }
    if (inAir)
    {
      land();
      inAir = false;
      Serial.println("Land");
      delay(1000);
      break;
    }
  }
}


int prev = analogRead(potentiometerPin);
int upAndDown()
{
  int height = analogRead(potentiometerPin)/45;

  if (height > prevalue + tollerance || height < prevalue - tollerance)
  {
    return height;
    prevalue = height;
  }
  return 0;
}


int roll(){
  mpu6050.update();
  int roll = mpu6050.getAngleX();
  if(roll > rollDeadzone + 10 || roll < rollDeadzone - 10){
    return roll;
  }
  return 0;
}


int pitch(){
  mpu6050.update();
  int pitch = mpu6050.getAngleY();
  if(pitch < pitchDeadzone - 10 || pitch > pitchDeadzone + 10){
    return pitch;
  }
  return 0;
}

int yaw(){
  int yaw = analogRead(joystickXPin);
  if(yaw < 0){
    yaw = yaw/-45;
  } else{
    yaw = yaw /45;
  }
  if(yaw > yawDeadzone + 2 || yaw < yawDeadzone - 2){
    return yaw;
  }
  return 0;
}

void rc(){
  int a = roll();
  int b = pitch();
  int c = upAndDown();
  int d = yaw();
  String msg0 = " " + a;
  String msg1 = " " + b;
  String msg2 = " " + c;
  String msg3 = " " + d;
  String msg = "rc"+msg0+msg1+msg2+msg3;
  sendmsg(msg);
}

void loop()
{
  connect();
  takeoffLand();
  rc();
  
}
