#include <Arduino.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"

#include "secrets.h"
#include "fireplace.h"
#include "index.h"

AsyncWebServer server(80);
AsyncEventSource events("/events");
ESP8266WiFiMulti wifiMulti;
Fireplace fireplace;

const uint32_t connectTimeoutMs = 5000;

String templateCallback(const String& var)
{
  if(var == "TEMPERATURE") return String(fireplace.temp());
  if(var == "TARGET") return String(fireplace.targetTemp());
  if(var == "HEATING_CLASS") fireplace.heating() ? F(" heating") : String();
  if(var == "ACTIVE_CLASS") return fireplace.on() ? F(" active") : String();

  return String();
}

void temperatureEvent() {
  static char temp[3];
  sprintf(temp, "%d", fireplace.targetTemp());
  events.send(temp, "target-temp", millis());
}

void handleIndex(AsyncWebServerRequest *request) {
  Serial.println("Received connection");
  request->send_P(200, "text/html", index_html, templateCallback);
}

void handleToggle(AsyncWebServerRequest *request) {
  fireplace.toggle();
  temperatureEvent();
  request->send(200);
}

void handleTemp(AsyncWebServerRequest *request, JsonVariant &json) {
    Serial.println("Received JSON");
    if (not json.is<JsonObject>()) {
      request->send(400, "text/plain", "Not an object");
      return;
    }
    auto&& data = json.as<JsonObject>();
    if (not data["temp"].is<int>()) {
      request->send(400, "text/plain", "temp is not an integer");
      return;
    }
    if (data["temp"] > 90) {
      request->send(400, "text/plain", "temp invalid");
    }

    fireplace.setTarget(data["temp"]);
    temperatureEvent();
    request->send(200);
}

void setup() {
  Serial.begin(9600);

  wifiMulti.addAP(WIFI_SSID1, WIFI_PASS1);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASS2);
  WiFi.hostname("Fireplace");

  delay(500);
  
  while (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED) {
    Serial.println(".");
    delay(2000); 
  }

  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer

  if (!MDNS.begin("fireplace")) {
    Serial.println("Error starting mDNS");
  } else {
    MDNS.addService("http", "tcp", 80);
    Serial.println("mDNS responder started");
  }
    
  server.addHandler(&events);

  server.on("/", handleIndex);
  server.on("/toggle.json", HTTP_POST, handleToggle);

  AsyncCallbackJsonWebHandler* temperatureHandler = new AsyncCallbackJsonWebHandler("/temp.json", handleTemp);
  server.addHandler(temperatureHandler);
  server.begin();
}

void loop() {
  if (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED) {
    return;
  }
  if (!MDNS.update()) {
    Serial.println("Error updating MDNS");
  }
  if (fireplace.onChanged()) {
    events.send(fireplace.on() ? "true" : "false", "on", millis());
  }
  if (fireplace.heatingChanged()) {
    events.send(fireplace.heating() ? "true" : "false", "heating", millis());
  }
}
