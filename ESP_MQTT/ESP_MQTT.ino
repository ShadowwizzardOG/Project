#include <Adafruit_BME280.h>
#include <WiFi.h>
 
const char* ssid     = "Rasp"; // ESP32 and ESP8266 uses 2.4GHZ wifi only
const char* password = "WeetIkVeel"; 
 
//MQTT Setup Start
#include <PubSubClient.h>
#define mqtt_server "192.168.59.1"
WiFiClient espClient;
PubSubClient client(espClient);
#define mqttTemp "temp"
#define mqttHum "hum"
//MQTT Setup End
 
Adafruit_BME280 bme; // I2C
 
float temp, hum;
 
void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // begin Wifi connect
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  //end Wifi connect
 
  client.setServer(mqtt_server, 1883);
  
  delay(5000);
  unsigned status;
  status = bme.begin(0x76); 
    if (!status) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1);
    }  
}
 
void getValues() {
 
  temp = bme.readTemperature();
  hum = bme.readHumidity();
  
  Serial.print("BME 1 Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
 
  Serial.print("BME 1 Humidity = ");
  Serial.print(hum);
  Serial.println(" %");
 
  Serial.println();
 
}
 
void reconnect() {
  // Loop until we're reconnected
  int counter = 0;
  while (!client.connected()) {
    if (counter==5){
      ESP.restart();
    }
    counter+=1;
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
   
    if (client.connect("ESP")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
 
void loop() {
  if (!client.connected()){
    reconnect();
  }
  getValues();
 
  client.publish(mqttTemp, String(temp).c_str(),true);
  client.publish(mqttHum, String(hum).c_str(),true);
 
 
  delay(1000);
}
