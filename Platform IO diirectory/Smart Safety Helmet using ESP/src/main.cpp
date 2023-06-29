#include <ESP8266WiFi.h>
#include <PubSubClient.h>

/**
 * CO326 - Group Project
 * Smart Safety Helmet
 * Group 11
 *      - Ariyawansha P.H.J.U. (E/18/028)
 *      - Kasthuripitiya K.A.I.M. (E/18/173)
 *      - Ranasinghe S.M.T.S.C. (E/18/285)
*/

// WIFI Status (Confidental)
const char* ssid = "JUA Xperia";
const char* password = "Jeewantha13";


// MQTT Brocker Credentials
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* tempTopic = "CO326/G11/Tempurature";
const char* gasTopic = "CO326/G11/Gas";
const char* stopTopic = "CO326/G11/Stop";
const char* soundTopic = "CO326/G11/Sound";
const char* buzzTopic = "CO326/G11/Buzzer";
///////////////////////////////////////////////////////////////

// Defining User ID (Area|Post|No)
#define UID 410101

// Defining sensor input pins
#define THERMISTER A0
#define GASSENSOR 12 // d6
#define SOUNDSENSOR 13 // d7
#define BUZZER 14 // D8

// Defining returning values of the sensor
String UOP_CO_326_E18_11_tempurature;
float UOP_CO_326_E18_11_tempuratureVal;
String UOP_CO_326_E18_11_gas;
bool UOP_CO_326_E18_11_gasVal;
String UOP_CO_326_E18_11_sound;
bool UOP_CO_326_E18_11_soundVal;

// Defining input values for actuators
String UOP_CO_326_E18_11_buzzer;

// Iniitialing WiFi CLient
WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;

///////////////// WiFi Module //////////////////////////////////
// Function to the coneting Wifi
void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

////////////////////// MQTT Module //////////////////////////////////
// Call back function to get messages
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

// Function to reconnect with the MQTT
void reconnect() {  
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESPClient")) {
      Serial.println("connected");
      client.subscribe(stopTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

////////////////////////// Time Module /////////////////////////////////////////
// Function to get current time
String time(){
  time_t now = time(nullptr);
  struct tm *timeinfo;
  timeinfo = localtime(&now);

  String time = String(timeinfo->tm_hour) + ":" + String(timeinfo->tm_min) + ":" + String(timeinfo->tm_sec);

  return time;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////// SENSORS ///////////////////////////////////////////////
/////////////// Thermometer ///////////////////////////////////////////////////
float Thermometer(){
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

  UOP_CO_326_E18_11_tempurature = String(UID) + "|" + String(curTime) + "|" + String(T);

  // Give data to the broker that the buzzer is activated
  // snprintf (msg3, MSG_BUFFER_SIZE, "%f", T);
  Serial.print("Publish message: ");
  Serial.println(UOP_CO_326_E18_11_tempurature);
  client.publish(tempTopic, UOP_CO_326_E18_11_tempurature.c_str());

  return T;
}

////////////////////////////// Gas Sensor ///////////////////////////////////////
bool GasSensor(){
  // getting the value from the gassensor
  int gas = digitalRead(GASSENSOR);
  // Getting the time that occured
  String curTime = time();

  // Sending the safe or unsafe status
  if (gas){
    UOP_CO_326_E18_11_gas = String(UID) + "|" + String(curTime) + "|" + String("SAFE");

    // Give data to the broker that the buzzer is activated
    // snprintf (msg3, MSG_BUFFER_SIZE, "%f", T);
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_gas);
    client.publish(gasTopic, UOP_CO_326_E18_11_gas.c_str());

    return 0;
  }
  else{
    UOP_CO_326_E18_11_gas = String(UID) + "|" + String(curTime) + "|" + String("UNSAFE");

    // Give data to the broker that the buzzer is activated
    // snprintf (msg3, MSG_BUFFER_SIZE, "%f", T);
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_gas);
    client.publish(gasTopic, UOP_CO_326_E18_11_gas.c_str());

    return 1;
  }
}

////////////////////////////////////// Sound Sensor ///////////////////////////////////////
bool SoundSensor(){
  // getting the value from the gassensor
  int sound = digitalRead(SOUNDSENSOR);
  // Getting the time that occured
  String curTime = time();

  // Sending the safe or unsafe status
  if (sound){
    UOP_CO_326_E18_11_sound = String(UID) + "|" + String(curTime) + "|" + String("SAFE");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_sound);
    client.publish(soundTopic, UOP_CO_326_E18_11_sound.c_str());

    return 0;
  }
  else{
    UOP_CO_326_E18_11_sound = String(UID) + "|" + String(curTime) + "|" + String("UNSAFE");
    
    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_sound);
    client.publish(soundTopic, UOP_CO_326_E18_11_sound.c_str());

    return 1;
  }
}

///////////////////////////////////////////////////////////////////////////////
//////////////////////// ACTUATOR ///////////////////////////////////////////////
//////////////// BUZZER /////////////////////////////////////////////////////////
// Function for buzzer
void Buzzer(String input){

  if (input == "Gas"){
    tone(BUZZER, 350, 1000);
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("GasBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());
  }
  else if (input == "Temp"){
    tone(BUZZER, 450, 1000);
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("TempBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());
  }
  else if (input == "Sound"){
    tone(BUZZER, 550, 1000);
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("SoundBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());
  }
}


void setup() {
// Sensor Pin Defining
  pinMode(THERMISTER, INPUT);
  pinMode(GASSENSOR, INPUT);
  pinMode(SOUNDSENSOR, INPUT);

  Serial.begin(115200);

  // Connect with wifi and MQTT
  setup_wifi();

  // Initialize the RTC
  configTime(19800, 0, "pool.ntp.org"); // Configure NTP server

  // Wait for the time to be synchronized
  while (!time(nullptr)) {
    delay(1000);
    Serial.println("Waiting for time synchronization...");
  }

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  // client.subscribe(topic);
}



//////////////////////////// PLC ///////////////////////////////////////////
void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  while()

  if (now - lastMsg > 2000) {
    lastMsg = now;
    UOP_CO_326_E18_11_tempuratureVal = Thermometer();
    delay(500);
    UOP_CO_326_E18_11_gasVal = GasSensor();
    delay(500);
    UOP_CO_326_E18_11_soundVal = SoundSensor();
    delay(500);

    if (UOP_CO_326_E18_11_tempuratureVal>40){
        Buzzer("Temp");
    }
    else if (UOP_CO_326_E18_11_gasVal){
        Buzzer("Gas");
    }
    else if (UOP_CO_326_E18_11_soundVal){
        Buzzer("Sound");
    }
  }
}