#include "Lpf2Hub.h"
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

#define WIFI_SSID "FIWARE"
#define WIFI_PSK "!FIWARE!on!air!"

byte port = (byte)PoweredUpHubPort::A;
int speed = 0;  // Placeholder temperature variable
int state = 0;
int position = 0;
String color = "blue"; // Placeholder color variable
int potValue = 0;

AsyncWebServer server(80);
Lpf2Hub myTrainHub;

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
    delay(1000);
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

    server.on("/g", HTTP_GET, [](AsyncWebServerRequest *request){
        String response = "position:" + String(position);
        request->send(200, "application/json", response);
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String response = "<html><body><a href=\"./s?value=00\">Speed = 0</a><br><a href=\"./s?value=40\">Speed = 40</a> </body></html>";
        request->send(200, "text/html", response);
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

  server.begin();
}

void loop()
{
    potValue = analogRead(A0);
    Serial.println(potValue);
    if (state == 0 && potValue < 4000){
        state = 1;
        position++;
        if (position == 1){
            myTrainHub.setLedColor(PINK);
        }
        if (position == 1) myTrainHub.setLedColor(PINK);
        else if (position == 2) myTrainHub.setLedColor(PURPLE);
        else if (position == 3) myTrainHub.setLedColor(BLUE);
        else if (position == 4) myTrainHub.setLedColor(LIGHTBLUE);
        else if (position == 5) myTrainHub.setLedColor(CYAN);
        else if (position == 6) myTrainHub.setLedColor(GREEN);
        else if (position == 7) myTrainHub.setLedColor(YELLOW);
        else if (position == 8) myTrainHub.setLedColor(ORANGE);
        else if (position == 9) { 
          myTrainHub.setLedColor(RED);
          position = 0;
        }
        
    }
    else if (state == 1 && potValue >= 4000){
        state = 0;
    }
}

String createJsonResponse(const char* status, const char* message) {
  DynamicJsonDocument jsonDoc(1024);
  jsonDoc["status"] = status;
  jsonDoc["message"] = message;
  String response;
  serializeJson(jsonDoc, response);
  return response;
}
