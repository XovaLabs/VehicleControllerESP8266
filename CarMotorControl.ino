#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <WebSocketsServer.h>

#include <MotorControl.h>
#include <CommandMapper.h>
#include <ArduinoJson.h>

class WebServerManager {
public:
    WebServerManager(const char* ssid, const char* password)
    : ssid(ssid), password(password), server(80), webSocket(81),
      newDataReceived(false), xPosition1(0), yPosition1(0), x1(0), y1(0),
      xPosition2(0), yPosition2(0), x2(0), y2(0), direction1(""), direction2("") {}

    void begin() {
        Serial.begin(115200);

        if (!LittleFS.begin()) {
            Serial.println("An Error has occurred while mounting LittleFS");
            return;
        }

        WiFi.begin(ssid, password);
        while (WiFi.status() != WL_CONNECTED) {
            delay(1000);
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi");
        Serial.println(WiFi.localIP());

        setupServerRoutes();
        server.begin();
        webSocket.begin();
        webSocket.onEvent(std::bind(&WebServerManager::onWebSocketEvent, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }

    void loop() {
        webSocket.loop();
    }

    bool isNewDataReceived() {
        return newDataReceived;
    }

    int getXPosition1() { return xPosition1; }
    int getYPosition1() { return yPosition1; }
    int getX1() { return x1; }
    int getY1() { return y1; }
    String getDirection1() { return direction1; }
    
    int getXPosition2() { return xPosition2; }
    int getYPosition2() { return yPosition2; }
    int getX2() { return x2; }
    int getY2() { return y2; }
    String getDirection2() { return direction2; }

    void resetNewDataFlag() {
        newDataReceived = false;
    }

private:
    const char* ssid;
    const char* password;
    AsyncWebServer server;
    WebSocketsServer webSocket;

    volatile bool newDataReceived;
    int xPosition1, yPosition1, x1, y1;
    String direction1;
    
    int xPosition2, yPosition2, x2, y2;
    String direction2;

    void setupServerRoutes() {
        server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/jerk.html", String(), false, std::bind(&WebServerManager::processor, this, std::placeholders::_1));
        });

        server.on("/joy.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/joy.js", "text/javascript");
        });

        server.on("/joy.min.js", HTTP_GET, [this](AsyncWebServerRequest *request) {
            request->send(LittleFS, "/joy.min.js", "text/javascript");
        });

        server.onNotFound(std::bind(&WebServerManager::notFound, this, std::placeholders::_1));
    }

    String processor(const String& var) {
        if (var == "STATE") {
            return "0";
        }
        return String();
    }

    void notFound(AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    }

    void handleWebSocketMessage(uint8_t num, uint8_t *data, size_t len) {
        data[len] = 0;
        Serial.printf("WebSocket Message: %s\n", (char *)data);

        DynamicJsonDocument jsonDoc(2048);
        deserializeJson(jsonDoc, (char *)data);

        JsonObject joy1 = jsonDoc["joy1"];
        xPosition1 = joy1["xPosition"].as<int>();
        yPosition1 = joy1["yPosition"].as<int>();
        direction1 = joy1["direction"].as<String>();
        x1 = joy1["x"].as<int>();
        y1 = joy1["y"].as<int>();

        JsonObject joy2 = jsonDoc["joy2"];
        xPosition2 = joy2["xPosition"].as<int>();
        yPosition2 = joy2["yPosition"].as<int>();
        direction2 = joy2["direction"].as<String>();
        x2 = joy2["x"].as<int>();
        y2 = joy2["y"].as<int>();

        newDataReceived = true;

        webSocket.sendTXT(num, "Message received");
    }

    void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
        switch (type) {
            case WStype_DISCONNECTED:
                Serial.printf("[%u] Disconnected!\n", num);
                break;
            case WStype_CONNECTED: {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connection from %s\n", num, ip.toString().c_str());
            }
            break;
            case WStype_TEXT:
                handleWebSocketMessage(num, payload, length);
                break;
        }
    }
};

MotorController motor(5, 4, 0, 2); // forward, reverse, left, right pins
CommandMapper mapper(motor);

WebServerManager serverManager("Rodriguez", "-HB9,bkCwR}FzXVp");

void setup() {
    serverManager.begin();
}

void loop() {
    serverManager.loop();

    if (serverManager.isNewDataReceived()) {
        Serial.println("Processing new joystick data...");

        int xPos1 = serverManager.getXPosition1();
        int yPos1 = serverManager.getYPosition1();
        int x1 = serverManager.getX1();
        int y1 = serverManager.getY1();
        String dir1 = serverManager.getDirection1();

        int xPos2 = serverManager.getXPosition2();
        int yPos2 = serverManager.getYPosition2();
        int x2 = serverManager.getX2();
        int y2 = abs(serverManager.getY2());
        String dir2 = serverManager.getDirection2();

        Serial.printf("Joystick 1: xPos=%d, yPos=%d, x=%d, y=%d, direction=%s\n", xPos1, yPos1, x1, y1, dir1.c_str());
        Serial.printf("Joystick 2: xPos=%d, yPos=%d, x=%d, y=%d, direction=%s\n", xPos2, yPos2, x2, y2, dir2.c_str());

        String command = dir2 + String(map(y2, 0, 100, 0, 255));

        Serial.println("command: " + command);
        mapper.processCommand(command.c_str());

        serverManager.resetNewDataFlag();
    }
}
