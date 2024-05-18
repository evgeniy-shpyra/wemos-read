#include <Arduino.h>
#include <ArduinoJson.h>

#include "storage.h"
#include "led.h"
#include "button.h"

#include "reader.h"

#include "server.h"
#include <PubSubClient.h>

#define SETTINGS_LED_PIN 14
#define WORK_LED_PIN 5
#define STATUS_LED_PIN 4

#define RESET_BTN 2
#define READ_PIN 12

const char *AP_SSID = "WemosRead";
const char *AP_PASS = "12345678";

String name;
String hubIp;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

Button resetBtn(RESET_BTN);
Led settingsLed(SETTINGS_LED_PIN);
Led workLed(WORK_LED_PIN);
Led readerLed(STATUS_LED_PIN);

Reader reader(READ_PIN, readerLed);

Storage storage;
SettingsServer settingsServer(storage, settingsLed);

bool currStatus = false;
bool isAutoToggled = false;
bool isAutoChanging = true;
bool statusBeforeAction = false;

bool isWorkMode = false;


void sendStatus()
{
  String topic = "sensor/" + name + "/status";
  String jsonString = "{\"status\": " + String(reader.getStatus() ? "true" : "false") + "}";

  Serial.println(jsonString);

  mqttClient.publish(topic.c_str(), jsonString.c_str());
}

void readButtons()
{
  resetBtn.loop();

  if (resetBtn.isClick())
  {
    Serial.println("resetBtn");
    storage.deleteSettings();
    ESP.restart();
  }
}

void MQTTcallback(char *topic, byte *payload, unsigned int length)
{
  String changeStatusTopic = "device/" + name + "/status/change";

  Serial.print("Message received in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  String message;
  for (int i = 0; i < length; i++)
  {
    message = message + (char)payload[i];
  }

  Serial.println(message);

  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, message);
  if (error)
  {
    Serial.println(error.c_str());
    return;
  }

  if (strcmp(topic, "ping") == 0)
  {
    sendStatus();
  }
}

void tryMQTTConnect()
{
  mqttClient.setServer(hubIp.c_str(), 1883);
  while (!mqttClient.connected())
  {
    // Serial.println("Connecting to MQTT");
    readButtons();
    String mqttName = "sensor-" + name;
    if (mqttClient.connect(mqttName.c_str()))
    {
      Serial.println("connected");
      workLed.on(); 
    }
    else
    {
      workLed.toggle();
      Serial.print(".");
      delay(250);
    }
  }
  mqttClient.setCallback(MQTTcallback);
  mqttClient.subscribe("ping");
}

void setup()
{
  Serial.begin(115200);


  bool isSettings = storage.settingsExist();

  if (isSettings == true)
  {
    isWorkMode = true;
    resetBtn.setup();
    workLed.setup();  
    reader.setup();
    SettingsStructure settings = storage.getSettings();

    name = settings.name;
    hubIp = settings.hubIp;

    WiFi.begin(settings.wifiSsid, settings.wifiPassword);
    while (WiFi.status() != WL_CONNECTED)
    {
      workLed.toggle();
      readButtons();
      Serial.print(".");
      delay(500);
    }
    workLed.off();
    Serial.print("Connected to WiFi :");
    Serial.println(WiFi.SSID());

    
    tryMQTTConnect();
    sendStatus();
  }
  else
  {
    isWorkMode = false;
    settingsServer.setup();
    WiFi.mode(WIFI_AP);
    WiFi.softAP(AP_SSID, AP_PASS);
    IPAddress IP = WiFi.softAPIP();

    Serial.print("Access Point IP address: ");
    Serial.println(IP);

    settingsServer.startServer();
  }
}

void loop()
{
  readButtons();

  if (isWorkMode)
  {
    if (!mqttClient.connected())
    {
      workLed.toggle();
      delay(250);
      workLed.toggle();
      delay(250);
      tryMQTTConnect();
    }

    reader.loop();
    if (!reader.getIsRoadStatus())
    {
      sendStatus();
    }

    mqttClient.loop();
  }
  else
  {
    settingsServer.loop();
  }
}
