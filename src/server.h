#ifndef SERVER_H
#define SERVER_H
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
class SettingsServer
{
private:
  ESP8266WebServer server;
  bool isRunning = false;
  Storage &storage;
  Led &led;

public:
  SettingsServer(Storage &storageRef, Led &ledRef) : storage(storageRef), led(ledRef)
  {
    this->storage = storage;
    this->led = led;
  }

  void startServer()
  {
    led.on();
    isRunning = true;

    server.on("/", HTTP_GET, [this]()
              { this->rootRoute(); });
    server.on("/save", HTTP_POST, [this]()
              { this->submitRoute(); });
    server.on("/save", HTTP_OPTIONS, [this]()
              { this->optionsRoute(); });

    server.begin();
    Serial.println("HTTP server started");
  }

  void stopServer()
  {
    isRunning = false;
    led.off();
    server.stop();
  }

  void loop()
  {
    if (!isRunning)
      return;
    server.handleClient();
  }
  void setup()
  {
    led.setup();
  }

private:
  String generateHtml()
  {
    StaticJsonDocument<512> doc;

    JsonArray ssids = doc.createNestedArray("ssids");

    int networkCount = WiFi.scanNetworks();
    for (unsigned int i = 0; i < min(networkCount, 10); ++i)
    {
      String ssidName = WiFi.SSID(i);
      ssids.add(ssidName);
    }

    String jsonString;
    serializeJson(doc, jsonString);

    return jsonString;
  }

  void optionsRoute()
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.sendHeader("Access-Control-Max-Age", "86400");
    server.send(200);
  }

  void rootRoute()
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.sendHeader("Access-Control-Max-Age", "86400");
    server.send(200, "application/json", generateHtml());
  }

  void submitRoute()
  {
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.sendHeader("Access-Control-Allow-Methods", "GET, POST");
    server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
    server.sendHeader("Access-Control-Max-Age", "86400");

    String jsonStr = server.arg("plain");

    DynamicJsonDocument doc(512);
    DeserializationError error = deserializeJson(doc, jsonStr);

    String name = doc["name"];
    String wifiSsid = doc["ssid"];
    String wifiPassword = doc["password"];
    String hubIp = doc["hubIp"];

    Serial.println(name);
    Serial.println(wifiSsid);
    Serial.println(wifiPassword);
    Serial.println(hubIp);

    SettingsStructure newSettings;

    name.toCharArray(newSettings.name, sizeof(newSettings.name) - 1);
    wifiSsid.toCharArray(newSettings.wifiSsid, sizeof(newSettings.wifiSsid) - 1);
    wifiPassword.toCharArray(newSettings.wifiPassword, sizeof(newSettings.wifiPassword) - 1);
    hubIp.toCharArray(newSettings.hubIp, sizeof(newSettings.hubIp) - 1);

    storage.updateSettings(newSettings);

    server.send(200);

    delay(2000);

    this->stopServer();
    led.off();
    ESP.restart();
  }
};

#endif // SERVER_H