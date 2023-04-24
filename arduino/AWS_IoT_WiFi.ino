#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <NTPClient.h>
#include <Arduino_JSON.h>
#include <ArduinoBearSSL.h>
#include <ArduinoECCX08.h>
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>  // change to #include <WiFi101.h> for MKR1000
#include <Arduino_MKRENV.h>
#include <SeeedOLED.h>
#include <WiFiNINA.h>
#include <SPI.h>
#include <map>
#include <unordered_map>
#include <vector>
#include <algorithm>


#include "arduino_secrets.h"

// Pin definition
#define BUTTON_PIN 0
#define LED1_PIN 1
#define LED2_PIN 2
#define BUZZER_PIN 3

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "1.uk.pool.ntp.org", 0, 60000);

/////// Enter your sensitive data in arduino_secrets.h
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
const char broker[] = SECRET_BROKER;
const char* certificate = SECRET_CERTIFICATE;

WiFiClient wifiClient;                // Used for the TCP socket connection
BearSSLClient sslClient(wifiClient);  // Used for SSL/TLS connection, integrates with ECC508
MqttClient mqttClient(sslClient);

unsigned long lastMillis = 0;

float temperature;
float humidity;
float illuminance;
float pressure;

bool iConnection = true;

// Define the tag IDs for each item
String milkTagIds[] = { "12097297", "11909230", "12104007", "12249255" };
String cokeTagIds[] = { "11910221", "14716460" };
String cheeseTagIds[] = { "14681293", "15037860" };
std::vector<String> twiceTagIds;

StaticJsonDocument<300> tagID;

std::map<std::string, std::map<std::string, float>> item;

int period = 5000;

// UCL logo
static const unsigned char UCLLogo[] PROGMEM = {
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X0F, 0X07, 0X01, 0X07, 0X0F, 0X7F, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0X01, 0X01, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0X09, 0X01, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X01, 0X29, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD,
  0X01, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X3F, 0X9F, 0XCF, 0XE7, 0XF3, 0XFB, 0XF9, 0XFD, 0XFC,
  0XFE, 0XFE, 0XFE, 0XFE, 0X7E, 0X7E, 0XFE, 0XFE, 0XFE, 0XFC, 0XFC, 0XFD, 0XF9, 0XFB, 0XF3, 0XE7,
  0XEF, 0XDF, 0X3F, 0X7F, 0XFF, 0XFF, 0XFF, 0X03, 0X01, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD,
  0XFD, 0X01, 0X01, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0X90, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0X80, 0XBF,
  0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0X00, 0X00, 0XFF, 0XFF, 0X0F, 0XC1, 0XF8, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X1F, 0XC7,
  0XFB, 0XFD, 0XFC, 0XFE, 0XFE, 0XFE, 0XFE, 0XFE, 0XFC, 0XF9, 0XF3, 0XCF, 0XCF, 0XEF, 0XE7, 0XF7,
  0XF3, 0XFB, 0XF9, 0XFC, 0XFE, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0X00, 0XF0, 0XFF, 0XFF, 0X80, 0X1F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XC0, 0X0F,
  0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X3F, 0X9F, 0XDF, 0X9F, 0XBF, 0X3F,
  0X7F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0X00, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F,
  0X7F, 0X7F, 0X7F, 0X7F, 0X70, 0X47, 0X1F, 0X3F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFC, 0XF9,
  0XFB, 0XFB, 0XF3, 0XFB, 0XFB, 0XF9, 0XFE, 0XFF, 0XFF, 0XFF, 0XFF, 0X7F, 0X7F, 0X1F, 0X4F, 0X63,
  0X78, 0X7F, 0X7F, 0X7F, 0X7F, 0X7C, 0X71, 0X67, 0X4F, 0X3F, 0X7F, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFE, 0XFC, 0XF9, 0XFB, 0XFB, 0XFB, 0XFB, 0XF9, 0XFD, 0XFC, 0XFE, 0XFF, 0XFF, 0XFF, 0X7F, 0X7F,
  0X3F, 0X4E, 0X66, 0X70, 0X7D, 0X7F, 0X7F, 0X08, 0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
  0XFF, 0XFE, 0XFC, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0XFD, 0X01, 0X01,
  0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F, 0X7F
};

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);  //RFID serial baud rate
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV Shield!");
    while (1)
      ;
  }
  if (!ECCX08.begin()) {
    Serial.println("No ECCX08 present!");
    while (1)
      ;
  }
  // Set a callback to get the current time
  // used to validate the servers certificate
  ArduinoBearSSL.onGetTime(getTime);

  // Set the ECCX08 slot to use for the private key
  // and the accompanying public certificate for it
  sslClient.setEccSlot(0, certificate);

  // Set the message callback, this function is
  // called when the MQTTClient receives a message
  mqttClient.onMessage(onMessageReceived);

  timeClient.begin();

  ucl_animation();  // Show start screen

  Tag_Json();

  item["milk"]["quantity"] = 4;
  item["coke"]["quantity"] = 2;
  item["cheese"]["quantity"] = 2;
  item["milk"]["price"] = 1.45;
  item["coke"]["price"] = 2.5;
  item["cheese"]["price"] = 2;
  item["milk"]["baditem"] = 0;
  item["coke"]["baditem"] = 0;
  item["cheese"]["baditem"] = 0;
  twiceTagIds.reserve(8);
}

void loop() {
  connectionSetup();
  if (iConnection) {
    iConnection = false;
    shelf_status();
    publishMessage("arduino/Item", Item_Json());
  }
  environment();
  RFID();
  pushButton();
  delay(100);
}

unsigned long getTime() {
  // get the current time from the WiFi module
  return WiFi.getTime();
}

void connectionSetup() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();  // If WiFi is disconnected, connect
  }
  if (!mqttClient.connected()) {
    connectMQTT();  // If MQTT client is disconnected, connect
  }
  mqttClient.poll();  // poll for new MQTT messages and send keep alives
}

void connectWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" ");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(3000);
  }
  Serial.println();

  Serial.println("You're connected to the network");
  Serial.println();
}

void connectMQTT() {
  Serial.print("Attempting to MQTT broker: ");
  Serial.print(broker);
  Serial.println(" ");

  while (!mqttClient.connect(broker, 8883)) {
    // failed, retry
    Serial.print("MQTT connection error: ");
    Serial.println(mqttClient.connectError());
    delay(3000);
  }
  Serial.println();

  Serial.println("You're connected to the MQTT broker");
  Serial.println();

  // subscribe to a topic
  mqttClient.subscribe("client/Command");
}

// template <typename T>
// void publishMessage(String topic, T data, String unit) {
//   Serial.print("Publishing " + topic);
//   Serial.println();

//   // send message, the Print interface can be used to set the message contents
//   mqttClient.beginMessage(topic);
//   mqttClient.print(data);
//   mqttClient.print(unit);
//   mqttClient.endMessage();
// }
void publishMessage(String topic, String json) {
  mqttClient.beginMessage(topic);
  mqttClient.print(json);
  mqttClient.endMessage();
}

void onMessageReceived(int messageSize) {
  Serial.println("Receiving message");
  String topic = mqttClient.messageTopic();
  StaticJsonDocument<256> json;

  deserializeJson(json, mqttClient);

  if (topic == "client/Command") {  
    if (json["light"] == "ON") {
      digitalWrite(LED2_PIN, HIGH);  //Turn On Custmoer light
      Serial.println("light on");   
    } else if (json["light"] == "OFF") {
      digitalWrite(LED2_PIN, LOW);  //Turn On Custmoer light
      Serial.println("light off");
    }
    if (json["buzzer"] == "ON") {
      tone(BUZZER_PIN, 4000);
      Serial.println("buzzer on");
    } else if (json["buzzer"] == "OFF") {
      noTone(BUZZER_PIN);
      Serial.println("buzzer off");
    }
    if (json.containsKey("period")) {
      period = json["period"];
      Serial.println("interval changed to " + (String)period);
    }
    if (json.containsKey("Milk")) {
      item["milk"]["price"] = json["Milk"] != null? json["Milk"]: item["milk"]["price"];
      item["coke"]["price"] = json["Coke"] != null? json["Coke"]: item["coke"]["price"];
      item["cheese"]["price"] = json["Cheese"] != null? json["Cheese"]: item["cheese"]["price"];
      shelf_status();
      publishMessage("arduino/Item", Item_Json());
    }
    if (json["lowpower"] == "ON") {
      WiFi.lowPowerMode();   
      Serial.println("lowpower mode on");
    } else if (json["lowpower"] == "OFF") {
      WiFi.noLowPowerMode();
      Serial.println("lowpower mode off");
    }
  }
}

String Environment_Json() {
  JSONVar MQTTmessage;
  MQTTmessage["Timestamp"] = getTimestamp();
  MQTTmessage["Temperature"] = temperature;
  MQTTmessage["Humidity"] = humidity;
  MQTTmessage["Pressure"] = pressure;
  MQTTmessage["Illuminance"] = illuminance;

  // JSON.stringify(myVar) can be used to convert the JSONVar to a String
  return JSON.stringify(MQTTmessage);

  // // setting a value to undefined can remove it from the object
  // MQTTmessage["x"] = undefined;
}

String LED_Json() {
  JSONVar MQTTmessage;
  MQTTmessage["Customer_LED"] = "ON";
  return JSON.stringify(MQTTmessage);
}

String Item_Json() {
  JSONVar MQTTmessage;
  MQTTmessage["Timestamp"] = getTimestamp();
  MQTTmessage["Milk_Stock"] = item["milk"]["quantity"];
  MQTTmessage["Coke_Stock"] = item["coke"]["quantity"];
  MQTTmessage["Cheese_Stock"] = item["cheese"]["quantity"];
  MQTTmessage["Milk_Price"] = item["milk"]["price"];
  MQTTmessage["Coke_Price"] = item["coke"]["price"];
  MQTTmessage["Cheese_Price"] = item["cheese"]["price"];
  MQTTmessage["Milk_Baditem"] = item["milk"]["baditem"];
  MQTTmessage["Coke_Baditem"] = item["coke"]["baditem"];
  MQTTmessage["Cheese_Baditem"] = item["cheese"]["baditem"];
  // JSON.stringify(myVar) can be used to convert the JSONVar to a String
  return JSON.stringify(MQTTmessage);

  // // setting a value to undefined can remove it from the object
  // MQTTmessage["x"] = undefined;
}

void Tag_Json() {
  JsonArray milk = tagID.createNestedArray("milk");
  for (int i = 0; i < sizeof(milkTagIds) / sizeof(milkTagIds[0]); i++) {
    milk.add(milkTagIds[i]);
  }
  JsonArray coke = tagID.createNestedArray("coke");
  for (int i = 0; i < sizeof(cokeTagIds) / sizeof(cokeTagIds[0]); i++) {
    coke.add(cokeTagIds[i]);
  }
  JsonArray cheese = tagID.createNestedArray("cheese");
  for (int i = 0; i < sizeof(cheeseTagIds) / sizeof(cheeseTagIds[0]); i++) {
    cheese.add(cheeseTagIds[i]);
  }
}


void environment() {
  if (millis() - lastMillis > period) {
    temperature = ENV.readTemperature();
    humidity = ENV.readHumidity();
    illuminance = ENV.readIlluminance();
    pressure = ENV.readPressure();

    lastMillis = millis();
    publishMessage("arduino/Environment", Environment_Json());
  }
}

//------------------------------------------------------------------
//-----------------------push button feedback-----------------------
void pushButton() {
  if (digitalRead(BUTTON_PIN) == 1) {
    delay(50);  // Push button debouncing
    if (digitalRead(BUTTON_PIN) == 1) {
      if (digitalRead(LED1_PIN) == LOW) {
        digitalWrite(LED1_PIN, HIGH);  //Turn On Custmoer light
        publishMessage("arduino/LED", LED_Json());
        SeeedOled.deactivateScroll();
        SeeedOled.clearDisplay();   // clear the screen and set start position to top left corner
        SeeedOled.setTextXY(2, 2);  // Set the cursor to the 3rd row, 1st column
        SeeedOled.putString("Assistance is");
        SeeedOled.setTextXY(3, 4);  // Set the cursor to the 3rd row, 1st column
        SeeedOled.putString("coming!");
        SeeedOled.setTextXY(4, 2);  // Set the cursor to the 3rd row, 1st column
        SeeedOled.putString("Please wait!");
      } else if (digitalRead(LED1_PIN) == HIGH) {
        digitalWrite(LED1_PIN, LOW);  //Turn On Custmoer light
        shelf_status();               // Activate Scroll
      }
    }
  }
}

//------------------------------------------------------------------
//----------------------------RFID sensor---------------------------
void RFID() {
  String card_number;
  if (Serial1.available()) {  // If there's data coming from RFID sensor, the data contains 14 hexadecimal numbers(STX + 2 * Start Character + 8 * Card Number + 2 * Checksum + ETX)
    char start = Serial1.read();
    if (start == '\u0002') {  // If the first character is not STX, discard received data
      String buffer = "";
      while (Serial1.available()) {  // Read data until ETX and store them in the buffer
        buffer = Serial1.readStringUntil('\u0003');
      }
      card_number = strtol(buffer.substring(2, 10).c_str(), NULL, 16);  // Convert Hexadecimal Card Number to long integer, which is actual the card number

      
      Serial.println("detected");
      // Check if the tag ID matches with any of the items
      String key = "";
      for (JsonPair pair : tagID.as<JsonObject>()) {
        for (JsonVariant element : pair.value().as<JsonArray>()) {
          if (element == card_number) {
            key = pair.key().c_str();
            break;
          }
        }
        if (key.length() > 0) {
          break;
        }
      }

      
      if (isElementInVector(twiceTagIds, card_number) == false) {
        item[std::string(key.c_str())]["quantity"] -= 1;
        twiceTagIds.push_back(card_number);
      } else {
        item[std::string(key.c_str())]["quantity"] += 1;
        item[std::string(key.c_str())]["baditem"] += 1;
        auto it = std::remove(twiceTagIds.begin(), twiceTagIds.end(), card_number);  // 删除所有值为card_number的元素
        twiceTagIds.erase(it, twiceTagIds.end());                                    // 从vector中删除值为card_number的元素
        Serial.println(item[std::string(key.c_str())]["baditem"]);
      }
      SeeedOled.clearDisplay();  // Clear the display
      SeeedOled.setNormalDisplay();
      // Set the display to normal mode (i.e. non-inverse mode)
      SeeedOled.setTextXY(3, 0);  // Set the cursor to the 3rd row, 1st column
      SeeedOled.putString((std::string(key.c_str()) + " QTY: ").c_str());
      SeeedOled.putNumber(item[std::string(key.c_str())]["quantity"]);  // Display the updated milk quantity on the OLED display
      publishMessage("arduino/Item", Item_Json());
      delay(1000);
      shelf_status();
    }
  }
}

String getTimestamp() {
  String formattedDate;
  String dayStamp;
  String timeStamp;
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }
  formattedDate = timeClient.getFormattedDate();
  int splitT = formattedDate.indexOf("T");
  return formattedDate.substring(0, splitT) + " " + formattedDate.substring(splitT + 1, formattedDate.length() - 1);
}
//------------------------------------------------------------------
//------------------------start_screen image------------------------
void start_screen() {
  SeeedOled.init();                                    //initialze SEEED OLED display
  SeeedOled.clearDisplay();                            // clear the screen and set start position to top left corner
  SeeedOled.drawBitmap((unsigned char*)UCLLogo, 512);  // 512 = 128 Pixels * 32 Pixels / 8
  SeeedOled.setNormalDisplay();                        //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();                             //Set addressing mode to Page Mode
  SeeedOled.setTextXY(5, 3);                           //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Smart Shelf");                  //Print the String
  SeeedOled.setTextXY(6, 7);                           //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("by");                           //Print the String
  SeeedOled.setTextXY(7, 5);                           //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Group 1");                      //Print the String
}


void ucl_animation() {
  Wire.begin();
  SeeedOled.init();                                                            // Initialize the OLED display
  SeeedOled.clearDisplay();                                                    // clear the screen and set start position to top left corner
  SeeedOled.drawBitmap((unsigned char*)UCLLogo, 512);                          // 512 = 128 Pixels * 32 Pixels / 8
  SeeedOled.setNormalDisplay();                                                //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();                                                     //Set addressing mode to Page Mode
  SeeedOled.setTextXY(5, 3);                                                   //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Smart Shelf");                                          //Print the String
  SeeedOled.setTextXY(6, 7);                                                   //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("by");                                                   //Print the String
  SeeedOled.setTextXY(7, 5);                                                   //Set the cursor to Xth Page, Yth Column
  SeeedOled.putString("Group 1");                                              //Print the String
  SeeedOled.setHorizontalScrollProperties(Scroll_Left, 0, 7, Scroll_3Frames);  //Set the properties of Horizontal Scrool
  SeeedOled.activateScroll();                                                  // Activate Scroll
}

void shelf_status() {
  SeeedOled.deactivateScroll();
  SeeedOled.clearDisplay();      // Clear the display
  SeeedOled.setNormalDisplay();  // Set the display to normal mode (i.e. non-inverse mode)

  SeeedOled.setTextXY(0, 5);  // Set the cursor to the 3rd row, 1st column
  SeeedOled.putString("Stock");
  SeeedOled.setTextXY(0, 11);  // Set the cursor to the 3rd row, 1st column
  SeeedOled.putString("Price");

  SeeedOled.setTextXY(1, 0);  // Set the cursor to the 3rd row, 1st column
  SeeedOled.putString("Milk");
  SeeedOled.setTextXY(1, 7);                      //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putNumber(item["milk"]["quantity"]);  // Display the updated milk quantity on the OLED display
  SeeedOled.setTextXY(1, 12);                     //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putFloat(item["milk"]["price"]);      // Display the updated milk quantity on the OLED display

  SeeedOled.setTextXY(3, 0);  // Set the cursor to the 3rd row, 1st column
  SeeedOled.putString("Coke");
  SeeedOled.setTextXY(3, 7);                      //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putNumber(item["coke"]["quantity"]);  // Display the updated milk quantity on the OLED display
  SeeedOled.setTextXY(3, 12);                     //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putFloat(item["coke"]["price"]);      // Display the updated milk quantity on the OLED display

  SeeedOled.setTextXY(5, 0);  // Set the cursor to the 3rd row, 1st column
  SeeedOled.putString("Cheese");
  SeeedOled.setTextXY(5, 7);                        //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putNumber(item["cheese"]["quantity"]);  // Display the updated milk quantity on the OLED display
  SeeedOled.setTextXY(5, 12);                       //Set the cursor to 2nd Page, 0th Column
  SeeedOled.putFloat(item["cheese"]["price"]);      // Display the updated milk quantity on the OLED display
}

bool isElementInVector(std::vector<String> vec, String element) {
  auto it = std::find(vec.begin(), vec.end(), element);
  if (it != vec.end()) {
    return true;
  } else {
    return false;
  }
}
