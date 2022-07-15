#include <DHT.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//Selected pin
#define DHTPIN 2
#define pumpPin 0
#define DHTTYPE DHT22
#define SensorPin 34
#define SensorRain 35

//Replace the next variables with your SSID/Password combination
const char* ssid = "****";
const char* password = "****";

//Mosquitto server
const char* mqtt_server = "test.mosquitto.org";

//Declare used variable
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int sensorValue;
int detection;
float hum;
float temp;
long rssi;
DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  //Initialise pin mode
  Serial.begin(115200);
  dht.begin();
  pinMode(SensorPin, INPUT);
  pinMode(SensorRain, INPUT);
  pinMode(pumpPin, OUTPUT);
  delay(10);
  //Connect with network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);  
}

//Function for call pump manually 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '0') {
    digitalWrite(pumpPin, LOW);
    Serial.println("Pump On");
  } 
  if ((char)payload[0] == '1') {
    digitalWrite(pumpPin, HIGH);
    Serial.println("Pump Off");
  }
}

//Function for reconnect client (Mosquitto)
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    }else{
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}

void loop(){
  //Test if client is connected
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    delay(2000);
    
    //Read and publish sensorValue (Soil moisture sensor)  
    sensorValue = digitalRead(SensorPin);
    if(sensorValue == HIGH){
      sensorValue = 10;
    }else{
      sensorValue = 100;
    }
    char senString[8];
    dtostrf(sensorValue, 5, 2, senString);
    client.publish("esp32/Soil", senString);
    
    //Read and publish humidity (Humidity Sensor ==> DHT22) 
    hum = dht.readHumidity();
    char humString[8];
    dtostrf(hum, 5, 2, humString);
    client.publish("esp32/humidity", humString);
    
    //Read and publish temperature (Temperature Sensor ==> DHT22)
    temp= dht.readTemperature();
    char tempString[8];
    dtostrf(temp, 5, 2, tempString);
    client.publish("esp32/temperature", tempString);
    
    //Read and publish rainfull sensor (Rainful Sensor)
    detection=digitalRead(SensorRain);
    if(detection == HIGH){
      detection = 10;
    }else{
      detection = 100;
    }
    char detString[8];
    dtostrf(detection, 5, 2, detString);
    client.publish("esp32/rain", detString);
    
    //Display value
    Serial.print("Soil: ");
    Serial.print(senString);
    Serial.print("%, Humidity: ");
    Serial.print(humString);
    Serial.print(" %, Temp: ");
    Serial.print(tempString);
    Serial.print(" Celsius, Pluie: ");
    Serial.println(detString);

    //Read and publish RSSI
    rssi=WiFi.RSSI();
    char RssiString[8];
    dtostrf(rssi, 5, 2, RssiString);
    Serial.print("RSSI: ");
    Serial.println(rssi);
    client.publish("esp32/rssi", RssiString);
  }
}
