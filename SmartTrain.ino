#include "Lpf2Hub.h"
#include <WiFi.h>

// create a hub instance
Lpf2Hub myTrainHub;
byte port = (byte)PoweredUpHubPort::A;

const char* SSID = "FIWARE";
const char* PSSW = "!FIWARE!on!air!";
WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Starting the program...");
  connectToHub();
  connectToWifi();
}

void connectToWifi()
{
  WiFi.begin(SSID, PSSW);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  myTrainHub.setLedColor( GREEN );
  server.begin();
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
      if ( myTrainHub.isConnected() )
      {
        Serial.println("Connected to HUB");
        Serial.print("Hub address: ");
        Serial.println( myTrainHub.getHubAddress().toString().c_str() );
      }
      else
      {
        Serial.println( "Failed to connect to HUB" );
      }
    }
  }
}

int analog = 0;
long int last_read = 0;
void loop() {
    if (last_read + 80 < millis()){
        analog = analogRead(A0);
        last_read = millis();
      }
    
    handle_client();
}

String header;

void parse_header(String currentLine) {
    int b = currentLine.indexOf(" HTTP/1.1");
    int a = currentLine.indexOf("GET /");
    if (a == -1 || b == -1) return;
    String instr = currentLine.substring(a+5, b);
    if (instr[0] == 's') {
        int s = instr.substring(1).toInt();
        if (s > 100) return;
        if (s < -100) return;
        Serial.printf("Setting speed %d\n", s);
        myTrainHub.setBasicMotorSpeed(port, s);
    }
    if (instr[0] == 'c') {
        String color = instr.substring(1);
        color.toLowerCase();
        Serial.printf("Setting speed %s\n", color);
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
    } if (instr[0] == 'g') {
        Serial.printf("Sending data: %d\n", analog);
    } else {
        Serial.printf("Unkwon instruction: %s\n", instr);  
    }
}

void handle_client()
{
    WiFiClient client = server.available();
    if (client) // If a new client connects,
    {
        Serial.println("New Client.");          // print a message out in the serial port
        String currentLine = "";                // make a String to hold incoming data from the client

        while (client.connected()) // loop while the client's connected
        {
            if (client.available()) // if there's bytes to read from the client,
            {
                char c = client.read();
                if (c == '\n')
                { // if the byte is a newline character
                  // if the current line is blank, you got two newline characters in a row.
                // that's the end of the client HTTP request, so send a response:

                    if (currentLine.length() == 0)
                    {
                        client.println("Hello Mundo!");
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                { // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }
            }
            parse_header(currentLine);
        }
        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}
