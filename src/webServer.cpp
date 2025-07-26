#include "webServer.h"
#include "config.h"
#include "buttonHandler.h"
#include "puzzleSetup.h"
#include "puzzleLogic.h"

AsyncWebServer server(80);
String boxId;

// External variable declarations
extern Button* gridButtons[9];
extern int cornerButtonPins[4];
extern int selectedPuzzleLayout;

void initWiFi(const char* ssid, const char* password) {
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    boxId = "box_" + WiFi.macAddress();
    
    Serial.println();
    Serial.print("Connected! IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("Box ID: ");
    Serial.println(boxId);
}

void initWebServer() {
    server.on("/api/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "application/json", getCurrentStatus());
    });
    
    server.on("/api/puzzle", HTTP_POST, [](AsyncWebServerRequest *request) {
        Serial.println("Received puzzle deployment request");
        Serial.printf("Request params: %d\n", request->params());
        
        String puzzleData = "";
        
        // Try different parameter sources
        if (request->hasParam("data", true)) {
            puzzleData = request->getParam("data", true)->value();
            Serial.println("Found POST data: " + puzzleData);
        } else if (request->hasParam("data", false)) {
            puzzleData = request->getParam("data", false)->value();
            Serial.println("Found GET data: " + puzzleData);
        } else {
            Serial.println("ERROR: Missing data parameter");
            // List all parameters for debugging
            for (int i = 0; i < request->params(); i++) {
                AsyncWebParameter* p = request->getParam(i);
                Serial.printf("Param[%d]: %s = %s\n", i, p->name().c_str(), p->value().c_str());
            }
            request->send(400, "application/json", "{\"error\":\"missing data\"}");
            return;
        }
        
        handlePuzzleUpdate(puzzleData);
        request->send(200, "application/json", "{\"status\":\"ok\"}");
    });
    
    server.on("/api/reset", HTTP_POST, [](AsyncWebServerRequest *request) {
        currentState = RESET;
        request->send(200, "application/json", "{\"status\":\"reset\"}");
    });
    
    server.begin();
    Serial.println("Web server started on port 80");
    
    // Register with management server
    registerWithServer();
}

String getCurrentStatus() {
    StaticJsonDocument<2048> doc;
    
    doc["boxId"] = boxId;
    doc["state"] = static_cast<int>(currentState);
    doc["ip"] = WiFi.localIP().toString();
    doc["solveColor"] = static_cast<int>(getCurrentSolveColor());
    doc["winCondition"] = checkWinCondition();
    
    JsonArray grid = doc["grid"].to<JsonArray>();
    for (int i = 0; i < 9; i++) {
        JsonObject btn = grid.add<JsonObject>();
        btn["type"] = static_cast<int>(gridButtons[i]->type);
        btn["pressed"] = gridButtons[i]->isPressed;
        btn["row"] = gridButtons[i]->row;
        btn["col"] = gridButtons[i]->col;
    }
    
    JsonArray corners = doc["corners"].to<JsonArray>();
    for (int i = 0; i < 4; i++) {
        corners.add(digitalRead(cornerButtonPins[i]) == LOW);
    }
    
    String output;
    serializeJson(doc, output);
    return output;
}

void handlePuzzleUpdate(const String& puzzleData) {
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, puzzleData);
    
    if (doc["layout"].is<JsonArray>()) {
        Serial.println("Deploying web puzzle layout");
        
        // Set the web deployment flag
        useWebDeployedPuzzle = true;
        
        // Extract and set the button layout
        ButtonType newLayout[9];
        for (int i = 0; i < 9; i++) {
            newLayout[i] = static_cast<ButtonType>(doc["layout"][i].as<int>());
            webDeployedLayout[i] = newLayout[i]; // Store for resets
            Serial.printf("Button %d: Type %d\n", i, newLayout[i]);
        }
        setButtonTypes(newLayout);
        
        // Set solve color if provided
        if (doc["solveColor"].is<int>()) {
            webDeployedSolveColor = static_cast<ButtonType>(doc["solveColor"].as<int>());
            Serial.printf("Solve color set to: %d\n", webDeployedSolveColor);
        }
        
        // Reset puzzle state
        currentState = INITIALISE;
    }
    else if (doc["puzzleIndex"].is<int>()) {
        Serial.println("Switching to hardcoded puzzle");
        
        // Switch back to hardcoded puzzles
        useWebDeployedPuzzle = false;
        selectedPuzzleLayout = doc["puzzleIndex"].as<int>();
        currentState = INITIALISE;
    }
}

void registerWithServer() {
    // Wait a bit for WiFi to stabilize
    delay(2000);
    
    HTTPClient http;
    http.begin("http://192.168.1.68:3000/api/boxes/register");
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000); // 10 second timeout
    
    StaticJsonDocument<256> doc;
    doc["boxId"] = boxId;
    doc["ip"] = WiFi.localIP().toString();
    
    String jsonString;
    serializeJson(doc, jsonString);
    
    Serial.println("Registering with server: " + jsonString);
    
    int httpResponseCode = http.POST(jsonString);
    if (httpResponseCode > 0) {
        Serial.printf("Registration response: %d\n", httpResponseCode);
        String response = http.getString();
        Serial.println("Server response: " + response);
    } else {
        Serial.printf("Registration failed: %d\n", httpResponseCode);
    }
    
    http.end();
}

void broadcastStatus() {
    static unsigned long lastBroadcast = 0;
    if (millis() - lastBroadcast > 1000) {
        lastBroadcast = millis();
        Serial.println("STATUS: " + getCurrentStatus());
    }
}