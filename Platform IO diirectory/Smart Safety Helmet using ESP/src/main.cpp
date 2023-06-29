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

const char* tempTopic = "UOP/CO326/E18/11/Temperature";
const char* gasTopic = "UOP/CO326/E18/11/Gas";
const char* stopTopic = "UOP/CO326/E18/11/Stop";
const char* soundTopic = "UOP/CO326/E18/11/Sound";
const char* buzzTopic = "UOP/CO326/E18/11/Buzzer";
///////////////////////////////////////////////////////////////

// Defining User ID (Area|Post|No)
#define UID 410101

// Defining sensor input pins
#define THERMISTER A0
#define GASSENSOR 12 // d6
#define SOUNDSENSOR 13 // d7
#define BUZZER 5 // D8

// Defining returning values of the sensor
String UOP_CO_326_E18_11_tempurature;
float UOP_CO_326_E18_11_tempuratureVal;
String UOP_CO_326_E18_11_gas;
bool UOP_CO_326_E18_11_gasVal;
String UOP_CO_326_E18_11_sound;
bool UOP_CO_326_E18_11_soundVal;

String UOP_CO_326_E18_11_stop;

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
    UOP_CO_326_E18_11_stop = (char)payload[i];
  }

  Serial.println(UOP_CO_326_E18_11_stop);

  if (UOP_CO_326_E18_11_stop == "1"){
    Buzzer("SOS");
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
  int total = 0;
  /* int numSamples = 10;

  // Read and sum multiple samples
  for (int i = 0; i < numSamples; i++) {
    total += analogRead(THERMISTER)*1.5;
    delay(10);
  } */

  String curTime = time();

  // Calculate the average
  float average = analogRead(THERMISTER) * 1.5;

  // Convert the average reading to temperature
  float resistance = 10000.0 / (1023.0 / average - 1.0);  // Assuming a 10k thermistor
  float steinhart = resistance / 10000.0;                // (R/Ro)
  steinhart = log(steinhart);                            // ln(R/Ro)
  steinhart /= 3950.0;                                   // 1/B * ln(R/Ro)
  steinhart += 1.0 / (25.0 + 273.15);                    // + (1/To)
  steinhart = (1.0 / steinhart) + 25;                           // Invert
  steinhart -= 273.15;                                   // Convert to Celsius
 

  UOP_CO_326_E18_11_tempurature = String(UID) + "|" + String(curTime) + "|" + String(steinhart);

  // Give data to the broker that the buzzer is activated
  // snprintf (msg3, MSG_BUFFER_SIZE, "%f", T);
  Serial.print("Publish message: ");
  Serial.println(UOP_CO_326_E18_11_tempurature);
  client.publish(tempTopic, UOP_CO_326_E18_11_tempurature.c_str());

  return steinhart;
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
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("GasBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());

    tone(BUZZER, 2000);
    delay(5000);
  }
  else if (input == "Temp"){
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("TempBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());

    tone(BUZZER, 1000);
    delay(10000);
  }
  else if (input == "Sound"){
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("SoundBuzz");

    // Give data to the broker that the buzzer is activated
    Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str());

    tone(BUZZER, 1500);
    delay(10000);
  }
  else if(input == "Off"){
    noTone(BUZZER);
    // delay(10000);
    // Getting the time that occured
    String curTime = time();

    UOP_CO_326_E18_11_buzzer = String(UID) + "|" + String(curTime) + "|" + String("Off");

    // Give data to the broker that the buzzer is activated
    /* Serial.print("Publish message: ");
    Serial.println(UOP_CO_326_E18_11_buzzer);
    client.publish(buzzTopic, UOP_CO_326_E18_11_buzzer.c_str()); */
  }
  else if (input == "SOS"){
    for (int i=0; i<5; i++){
      tone(BUZZER, 1700);
      delay(1000);
      noTone(BUZZER);
      delay(500);
    }

  }  
}


void setup() {
// Sensor Pin Defining
  pinMode(THERMISTER, INPUT);
  pinMode(GASSENSOR, INPUT);
  pinMode(SOUNDSENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(BUZZER, LOW);

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

  UOP_CO_326_E18_11_stop = "0";

  // while(UOP_CO_326_E18_11_stop != "1"){
  // while (UOP_CO_326_E18_11_stop == "1"){}
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
    if (UOP_CO_326_E18_11_gasVal){
        Buzzer("Gas");
    }
    if (UOP_CO_326_E18_11_soundVal){
        Buzzer("Sound");
    }
    else if(UOP_CO_326_E18_11_tempuratureVal<40 && !UOP_CO_326_E18_11_gasVal && !UOP_CO_326_E18_11_soundVal) {
      Buzzer("Off");
    }
  }

  // Serial.println("Device Stoped by the Admin");
}