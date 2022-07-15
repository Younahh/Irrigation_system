#include <DHT.h>
#include <WiFi.h>

//Selected pin
#define DHTPIN 2
#define pumpPin 0
#define DHTTYPE DHT22
#define SensorPin 34
#define SensorRain 35

//Replace the next variables with your SSID/Password combination
const char* ssid = "****";
const char* password = "****";

//Declare used variable
int sensorValue;
int detection;
float hum;
float temp;
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  //Initialise pin mode
  Serial.begin(115200);
  dht.begin();
  pinMode(SensorPin, INPUT);
  pinMode(SensorRain, INPUT);
  pinMode(pumpPin, OUTPUT);
  delay(10);
  //Connect with network
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
}

void loop() {
  //Read sensor value
  sensorValue = digitalRead(SensorPin);
  if(sensorValue == HIGH){
    sensorValue = 10;
  }else{
    sensorValue = 100;
  }
  //Function for automate irrigation
  /*if(sensorValue == HIGH){
    digitalWrite(pumpPin, LOW);
    Serial.print("Pump on");
  }else{
    digitalWrite(pumpPin, HIGH);
    Serial.print("Pump off");
  }*/
  hum = dht.readHumidity();
  temp= dht.readTemperature();
  detection=digitalRead(SensorRain);
  if(detection == HIGH){
    detection = 10;
  }else{
    detection = 100;
  }
  //Display value
  Serial.print("Soil: ");
  Serial.print(sensorValue);
  Serial.print("%, Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.print(" Celsius, Pluie: ");
  Serial.println(detection);
  delay(2000);
}
