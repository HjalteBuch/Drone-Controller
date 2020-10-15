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
const int joystickXPin = 2;
const int joystickYPin = 36;
const int potentiometerPin = 32;
const int buttonPinWhite = 23;
const int buttonPinBlue = 5;

//til potentiometeret
int prevalue = 0;
const int tollerance = 250;

//takeoff / land check
boolean inAir = false;

int takeofflandcheck = 0;

void command()
{
  String msg = "command";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
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

  WiFi.mode(WIFI_STA);

  Serial.println("ready!");
}

void takeoff()
{
  String msg = "takeoff";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
}

void land()
{
  String msg = "land";
  udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
              IPAddress(192, 168, 10, 1), 8889);
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
int prev = analogRead(potentiometerPin);
void upAndDown()
{
  int height = analogRead(potentiometerPin);

  if (height > prevalue + tollerance)
  {
    String msg = "up 20";
    udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
                IPAddress(192, 168, 10, 1), 8889);
                prevalue = height;
                Serial.println(msg);
  }

  if (height < prevalue - tollerance)
  {
    String msg = "down 20";
    udp.writeTo((const uint8_t *)msg.c_str(), msg.length(),
                IPAddress(192, 168, 10, 1), 8889);
                prevalue = height;
                Serial.println(msg);
  }
}

void loop()
{
  connect();
  takeoffLand();
  upAndDown();
}
