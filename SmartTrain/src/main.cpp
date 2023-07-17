#include <Arduino.h>
#include <Lpf2Hub.h>
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <mutex>

std::mutex serialMutex;

// TODO: Change to FIWARE_fair
const char* ssid = "FIWARE";
const char* password = "!FIWARE!on!air!";
const char* mqtt_server = "192.168.20.35"; // TODO: Change to right IP Adress

const char* mqtt_username = "LegoDemonstrator";
const char* mqtt_password = "Lego12Demo34nstr56ator";
const char* mqtt_client_id = "SmartTrain-ESP32";

const char* mqtt_attrs_topic = "/idFZy8D9KzFko7db/train001/attrs";
const char* mqtt_train_cmd_topic = "/idFZy8D9KzFko7db/train001/cmd";

byte port = (byte)PoweredUpHubPort::A;
Lpf2Hub trainHub;
WiFiClient espClient;
PubSubClient client(espClient);
StaticJsonDocument<16> jsonDoc;

unsigned int state = 0;
unsigned int position = 0;
int speed = 0;

const unsigned long interval = 25000;
unsigned long previousMillis = 0;

void callback(char*, byte*, unsigned int);
void connectToHub();
void connectToWiFi();
void connectMqtt();
void updateState();
void reconnect();
void sendPosition();
void sendSpeed();
void setup();
void loop();

void setup() {
    Serial.begin(115200);
    Serial.println("Starting the program...");
    
    Serial.println("Connecting to HUB");
    connectToHub();
    
    Serial.println("Connecting to WiFi");
    connectToWiFi();

    delay(3000);

    Serial.println("Connecting to MQTT");
    client.setBufferSize(256); // otherwise it stops to recieve messages after some time
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    connectMqtt();
}

void loop() {
    // put your main code here, to run repeatedly:
    if (!client.connected()) reconnect();
    updateState();
    sendPosition();
    client.loop();
}

void connectToWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("WiFi connected.");
    Serial.println(WiFi.localIP());
    trainHub.setLedColor( WHITE );
}

void connectToHub() {
    if ( !trainHub.isConnected() && !trainHub.isConnecting()) {
        trainHub.init();
    }
        
    while (!trainHub.isConnected()) {
        if (trainHub.isConnecting()) {
            trainHub.connectHub();
            if (!trainHub.isConnected()) {
                Serial.println("Failed to connect to HUB");
            }
        }
        delay(1000);
    }
    trainHub.setLedColor( BLUE );
}

void updateState() {
    int potValue = analogRead(A0);
    if (state == 0 && potValue < 4000) {
        state = 1;
        position++;
    }
    else if (state == 1 && potValue >= 4000) {
        state = 0;
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    serialMutex.lock();
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++)
        Serial.print((char)payload[i]);
    Serial.println();
    serialMutex.unlock();

    char buffer[length+1];
    strncpy(buffer, reinterpret_cast<char*>(payload), length);
    buffer[length] = '\0';

    DeserializationError error  =deserializeJson(jsonDoc, buffer);
    if (error) {
        serialMutex.lock();
        Serial.print("Error: deserializeJson() failed: ");
        Serial.println(error.c_str());
        serialMutex.unlock();
        return;
    }
    if (!jsonDoc["setspeed"].is<int>()){
        serialMutex.lock();
        Serial.print("Error: Speed recieved is not an int!");
        Serial.println(error.c_str());
        serialMutex.unlock();
        return;
    }
    if (speed != jsonDoc["setspeed"]){
        speed = jsonDoc["setspeed"];
        trainHub.setBasicMotorSpeed(port, speed);
        sendSpeed();
    }
}

void connectMqtt(){
    if (client.connect(mqtt_client_id, mqtt_username, mqtt_password)) {
        Serial.println("connected to MQTT");
        client.subscribe(mqtt_train_cmd_topic);
        
        if (!client.connected()) {
            reconnect();
        }

        trainHub.setLedColor( GREEN ); 
    }
}

void reconnect() {
    while (!client.connected()){
        trainHub.setLedColor( RED );
        serialMutex.lock();
        Serial.println("Attempting MQTT connection...");
        if ( client.connect(mqtt_client_id, mqtt_username, mqtt_password) ) {
        Serial.println("connected");
        client.subscribe(mqtt_train_cmd_topic);

        } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
        delay(5000);
        }
        serialMutex.unlock();
    }
    trainHub.setLedColor( GREEN );
}

void sendPosition() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval){
        String payload = "";
        jsonDoc.clear();
        jsonDoc["position"] = position;
        serializeJson(jsonDoc, payload);
        client.publish(mqtt_attrs_topic, payload.c_str());
        serialMutex.lock();
        Serial.print("Sending data to Server: ");
        Serial.println(payload.c_str());
        if (client.connected()) Serial.println("MQTT Status: Connected");
        else Serial.println("MQTT Status: Disconnected");
        serialMutex.unlock();
        previousMillis = millis();
    }
}

void sendSpeed() {
    String payload = "";
    jsonDoc.clear();
    jsonDoc["speed"] = speed;
    serializeJson(jsonDoc, payload);
    client.publish(mqtt_attrs_topic, payload.c_str());
    serialMutex.lock();
    Serial.print("Sending data to Server: ");
    Serial.println(payload.c_str());
    serialMutex.unlock();
}