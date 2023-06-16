#include <Arduino.h>
#include <ESP8266WiFi.h>

// WIFI Status (Confidental)
const char *ssid = "JUA Xperia";
const char *password = "Jeewantha13";

// Iniitialing WiFi CLient
WiFiClient client;

// Defining User ID (Area|Post|No)
#define UID 010101

// Defining sensor input pins
#define THERMISTER A0

// Defining returning values of the sensor
String UOP_CO_326_E18_11_tempurature;

// Defining the seperate functions for sensors and actuators
String Thermometer();

// Funtion for get time
String time();
// Fuction for connect  with wifii
void connectToWiFi();


void setup() {
  Serial.begin(9600);

  // Connect to the WIFI
  // Connect to Wi-Fi
  connectToWiFi();

  // Initialize the RTC
  configTime(0, 0, "pool.ntp.org"); // Configure NTP server

  // Wait for the time to be synchronized
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time synchronization...");
  }
}

void loop() {
  // Getting tempurature and printing the output of it
  String UOP_CO_326_E18_11_tempurature = Thermometer();

  Serial.println(UOP_CO_326_E18_11_tempurature);
  delay(1000);
}

String Thermometer(){
  // Getting the raw sensor details (Voltage)
  int Vo = analogRead(THERMISTER);
  String curTime = time();
  // Calculate voltage
  float v = Vo * 5.0 / 1024;
  float Rt = 10 * v / (5 - v);

  // Getting tempurature in Kelvin 
  float tempK = 1 / (log(Rt / 10) / 3950 + 1 / (273.15 + 25));

  // Getting the tempurature in Celsuius
  float T = tempK - 293.15 + 18;

  String temp = String(010101) + "|" + String(curTime) + "|" + String(T);

  return temp;
}

String time(){
  time_t now = time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);

  String time = String(timeinfo->tm_hour) + ":" + String(timeinfo->tm_min) + ":" + String(timeinfo->tm_sec);

  return time;
}


// Function to the coneting Wifi
void connectToWiFi() {
//Connect to WiFi Network
   Serial.println();
   Serial.println();
   Serial.print("Connecting to WiFi");
   Serial.println("...");
   WiFi.begin(ssid, password);
   int retries = 0;
while ((WiFi.status() != WL_CONNECTED) && (retries < 15)) {
   retries++;
   delay(500);
   Serial.print(".");
}
if (retries > 14) {
    Serial.println(F("WiFi connection FAILED"));
}
if (WiFi.status() == WL_CONNECTED) {
    Serial.println(F("WiFi connected!"));
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}
    Serial.println(F("Setup ready"));
}