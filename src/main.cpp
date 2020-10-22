#include <Arduino.h>
#include <Wire.h>
#include <MPU6050_tockn.h>
#include <AsyncUDP.h>
#include <WiFi.h>

MPU6050 mpu6050(Wire);

AsyncUDP udp;
//wifi name
const char *ssid = "TELLO-598E42";
const char *password = "";

//Pin numbers bliver defineret her
const int joystickXPin = 36;
const int potentiometerPin = 32;
const int buttonPinWhite = 23;
const int buttonPinBlue = 5;

//til potentiometeret
int prevalue = 0;
const int tollerance = 250;

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

//når potentiometer står stille køre den skiftevis "descending" og "ascending", men den fatter mens man drejer
void upAndDown()
{
  int height = analogRead(potentiometerPin);

  if (height > prevalue + tollerance)
  {
    sendmsg("up 20");
    prevalue = height;
  }

  if (height < prevalue - tollerance)
  {
    sendmsg("down 20");
    prevalue = height;
  }
}


void direction(){
  mpu6050.update();
  int roll = mpu6050.getAngleX();
  if(roll > rollDeadzone + 10){
    sendmsg("left 20");
  }
  if(roll < rollDeadzone - 10){
    sendmsg("right 20");
  }
  int pitch = mpu6050.getAngleY();
  if(pitch < pitchDeadzone - 10){
    sendmsg("forward 20");
  }
  if(pitch > pitchDeadzone + 10){
    sendmsg("back 20");
  }
  roll = rollDeadzone;
  pitch = pitchDeadzone;
}

void yaw(){
  int yaw = analogRead(joystickXPin);
  if(yaw > yawDeadzone + 100){
    sendmsg("cw 36");
  }
  if(yaw < yawDeadzone - 100){
    sendmsg("ccw 36");
  }
}

void rc(){
  mpu6050.update();
  int roll = mpu6050.getAngleX();
  int pitch = mpu6050.getAngleY();
  int throttle = map(analogRead(potentiometerPin), 0, 4095, -100, 100);
  int yaw = map(analogRead(joystickXPin)+160, 0, 4095, -100, 100);

  Serial.print("Roll: [");
  Serial.print(roll);
  Serial.print("], Pitch: [");
  Serial.print(pitch);
  Serial.print("], Throttle: [");
  Serial.print(throttle);
  Serial.print("], yaw: [");
  Serial.print(yaw);
  Serial.println("]");
  sendmsg("rc " + roll + pitch + throttle + yaw);
}

void loop()
{
  connect();
  takeoffLand();
  //upAndDown();
  //direction();
  //yaw();
  rc();
}
