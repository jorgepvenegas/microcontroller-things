/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.
 
  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).
 
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!
 
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 *************************************************************************/
 
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

// ST7789 TFT module connections
#define TFT_DC    D1     // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST   D2     // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS    D8     // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)
// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

//Enter your wifi credentials
const char* ssid = "yourssid";  
const char* password =  "ssidpassword";

//Enter your mqtt server configurations
const char* mqttServer = "ip";    //Enter Your mqttServer address
const int mqttPort = 1883;       //Port number
const char* mqttUser = "username"; //User
const char* mqttPassword = "password"; //Password

WiFiClient espClient;
PubSubClient client(espClient);

void MQTTcallback(char* topic, byte* payload, unsigned int length) {
 
  char* tempTopic = "livingroom/sensor/livingroom_temperature/state";
  char* humidityTopic = "livingroom/sensor/livingroom_humidity/state";

  String message;
  for (int i = 0; i < length; i++) {
    message = message + (char)payload[i];  //Conver *byte to String
  }

  Serial.print("Topic: ");
  Serial.println(topic);
  tft.setTextColor(ST77XX_BLACK);

  Serial.print("Message: ");
  Serial.print(message);
  if(strcmp(topic, tempTopic) == 0) {
    tft.setTextSize(3);
    tft.setCursor(5, 5);
    tft.println("Temperature");
    tft.setCursor(5, 35);
    tft.fillRect(0, 35, tft.width(), 50, ST77XX_WHITE);
    tft.setTextSize(5);
    tft.setTextColor(ST77XX_RED);
    tft.print(message);
    tft.print(" C");
    Serial.println();
    Serial.println("-----------------------");  
  }
  if(strcmp(topic, humidityTopic) == 0) {
    tft.setTextSize(3);
    tft.setCursor(5, 90);
    tft.println("Humidity");
    tft.setCursor(5, 125);
    tft.fillRect(0, 125, tft.width(), 50, ST77XX_WHITE);
    tft.setTextSize(5);
    tft.setTextColor(ST77XX_RED);
    tft.print(message);
    tft.print(" %");
    Serial.println();
    Serial.println("-----------------------");  
  }
}

void setup(void) {
  Serial.begin(9600);
  Serial.print(F("Hello! ST77xx TFT Test"));
 
  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
  delay(500);
 
  tft.fillScreen(ST77XX_BLACK);

  WiFi.begin(ssid, password);
  tft.setTextSize(2);
  
  // Print WiFi connection status on TFT Screen
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting WiFi... ");
    tft.setTextColor(ST77XX_RED);
    tft.setCursor(0, 0);
    tft.print("Connecting WiFi: ");
  }

  tft.setTextColor(ST77XX_GREEN);
  tft.println("Ok!");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(MQTTcallback);

  // Print MQTT connection status on TFT Screen
  while (!client.connected()) {
    tft.setTextColor(ST77XX_RED);
    Serial.println("Connecting MQTT...");
    tft.setCursor(0, 60);
    tft.print("Connecting MQTT: ");
    delay(1000);
    if (client.connect("some-unique-client-id", mqttUser, mqttPassword )) {
 
      Serial.println("Client connected.");  
 
    } else {
      Serial.print("Client failed: ");
      Serial.println(client.state());
      delay(2000);
 
    }
  }
  tft.setTextColor(ST77XX_GREEN);
  tft.println("Ok!");
  delay(2000);

  client.subscribe("livingroom/sensor/livingroom_temperature/state");
  client.subscribe("livingroom/sensor/livingroom_humidity/state");
  tft.fillScreen(ST77XX_WHITE);
}

 
void loop() {
  client.loop();
}
