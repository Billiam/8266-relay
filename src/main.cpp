#include <Arduino.h>

#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>
#include <PID_v1.h>
#include "ESPAsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "AsyncJson.h"
#include <RBD_Timer.h>

#include "secrets.h"
#include "fireplace.h"
#include "index.h"
 

AsyncWebServer server(80);
AsyncEventSource events("/events");
ESP8266WiFiMulti wifiMulti;
Fireplace fireplace;
RBD::Timer pidTimer;

unsigned long startTime;
const long maximumOnTime = 1000 * 60 * 60 * 3.5;
const uint32_t connectTimeoutMs = 5000;
const int fireplacePin = D3;

String templateCallback(const String& var)
{
  if(var == "TEMPERATURE") return String(fireplace.temp());
  if(var == "TARGET") return String(fireplace.targetTemp());
  if(var == "HEATING_CLASS") return fireplace.heating() ? F(" heating") : String();
  if(var == "ACTIVE_CLASS") return fireplace.on() ? F(" active") : String();

  return String();
}

void targetEvent() {
  static char temp[3];
  sprintf(temp, "%d", fireplace.targetTemp());
  events.send(temp, "target-temp", millis());
}

void temperatureEvent() {
  static char temp[3];
  sprintf(temp, "%d", fireplace.temp());
  events.send(temp, "current-temp", millis());
}

void handleIndex(AsyncWebServerRequest *request) {
  Serial.println("Received connection");
  request->send_P(200, "text/html", index_html, templateCallback);
}

void handleToggle(AsyncWebServerRequest *request) {
  fireplace.toggle();
  targetEvent();
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

  fireplace.setTemp(data["temp"]);

  temperatureEvent();
  request->send(200);
}

void handleTarget(AsyncWebServerRequest *request, JsonVariant &json) {
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
    targetEvent();
    request->send(200);
}

void setup() {
  Serial.begin(9600);

  pinMode(fireplacePin, OUTPUT);

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

  AsyncCallbackJsonWebHandler* targetHandler = new AsyncCallbackJsonWebHandler("/target.json", handleTarget);
  AsyncCallbackJsonWebHandler* temperatureHandler = new AsyncCallbackJsonWebHandler("/temp.json", handleTemp);

  server.addHandler(targetHandler);
  server.addHandler(temperatureHandler);

  pidTimer.setTimeout(3000);
  pidTimer.restart();

  server.begin();
}

void manageTemp() {
  if (!fireplace.on()) {
    return;
  }
  int target = fireplace.targetTemp();
  int current = fireplace.temp();
  if (current > 85) {
    fireplace.setOff();
    return;
  }
  if (current > 30 && target > 50) {
    if (current > target + 1) {
      fireplace.setHeating(false);
    } else if (current <= target - 2) {
      fireplace.setHeating(true);
    }
  }
}

void loop() {
  if (wifiMulti.run(connectTimeoutMs) != WL_CONNECTED) {
    return;
  }
  if (!MDNS.update()) {
    Serial.println("Error updating MDNS");
  }
  if (pidTimer.onRestart()) {
    manageTemp();
  }
  if (fireplace.onChanged()) {
    events.send(fireplace.on() ? "true" : "false", "on", millis());
  }
  if (fireplace.heatingChanged()) {
    events.send(fireplace.heating() ? "true" : "false", "heating", millis());
    if (fireplace.on()) {
      startTime = millis();
    }
  }

  if (fireplace.heating() && millis() - startTime > maximumOnTime) {
    fireplace.setOff();
  }

  digitalWrite(fireplacePin, fireplace.heating() ? HIGH : LOW);
}
