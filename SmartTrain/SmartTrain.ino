#include "Lpf2Hub.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>


#define WIFI_SSID "FIWARE"
#define WIFI_PSK "!FIWARE!on!air!"

Lpf2Hub myTrainHub;
byte port = (byte)PoweredUpHubPort::A;

int speed = 0;  // Placeholder temperature variable
String color = "blue"; // Placeholder color variable
AsyncWebServer server(80);

void setup()
{
    Serial.begin(115200);
    Serial.println("Starting the program...");
    connectToHub();
    connectToWifi();
    setRoutes();
}

void connectToWifi() 
{
    WiFi.begin(WIFI_SSID, WIFI_PSK);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    myTrainHub.setLedColor( GREEN );
}

void connectToHub()
{
  if (!myTrainHub.isConnected() && !myTrainHub.isConnecting())
  {
    myTrainHub.init();
  }
  while (!myTrainHub.isConnected())
  {
    if (myTrainHub.isConnecting())
    {
      myTrainHub.connectHub();
      if ( !myTrainHub.isConnected() )
      {
        Serial.println( "Failed to connect to HUB" );
      }
    }
  }
}

void setRoutes()
{
    server.on("/s", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      speed = request->getParam("value")->value().toInt();
      if (speed > 100 || speed < -100) 
      {
        String response = createJsonResponse("fail", "Speed needs to be [-100..100]");
        request->send(400, "application/json", response);
      }
      else 
      {
        myTrainHub.setBasicMotorSpeed(port, speed);
        String response = createJsonResponse("success", ("Speed set to " + String(speed)).c_str());
        request->send(200, "application/json", response);
      }
      
    } else {
      String response = createJsonResponse("fail", "Missing argument");
      request->send(400, "application/json", response);
    }
  });

    server.on("/c", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      color = request->getParam("value")->value();
      if (color == "black") myTrainHub.setLedColor(BLACK);
      else if (color == "pink") myTrainHub.setLedColor(PINK);
      else if (color == "purple") myTrainHub.setLedColor(PURPLE);
      else if (color == "blue") myTrainHub.setLedColor(BLUE);
      else if (color == "lightblue") myTrainHub.setLedColor(LIGHTBLUE);
      else if (color == "cyan") myTrainHub.setLedColor(CYAN);
      else if (color == "green") myTrainHub.setLedColor(GREEN);
      else if (color == "yellow") myTrainHub.setLedColor(YELLOW);
      else if (color == "orange") myTrainHub.setLedColor(ORANGE);
      else if (color == "red") myTrainHub.setLedColor(RED);
      else if (color == "white") myTrainHub.setLedColor(WHITE);
      else if (color == "none") myTrainHub.setLedColor(NONE);

      String response = createJsonResponse("fail", ("Setting color to "+ color).c_str());
      request->send(200, "application/json", response);
    } else {
      String response = createJsonResponse("fail", "Missing argument");
      request->send(400, "application/json", response);
    }
  });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

  server.begin();
}

void loop()
{
}

String createJsonResponse(const char* status, const char* message) {
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["status"] = status;
  jsonDoc["message"] = message;
  String response;
  serializeJson(jsonDoc, response);
  return response;
}
