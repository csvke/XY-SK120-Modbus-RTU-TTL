// Fix HTTP method definition conflicts between ESPAsyncWebServer and WiFiManager
#define WEBSERVER_H  // Prevent WebServer.h from being included directly

// Define HTTP methods for ESPAsyncWebServer before including it
// These match the WebRequestMethod enum in ESPAsyncWebServer.h
#define HTTP_GET     0b00000001
#define HTTP_POST    0b00000010
#define HTTP_DELETE  0b00000100
#define HTTP_PUT     0b00001000
#define HTTP_PATCH   0b00010000
#define HTTP_HEAD    0b00100000
#define HTTP_OPTIONS 0b01000000
#define HTTP_ANY     0b01111111

#include "web_interface.h"
#include <ArduinoJson.h>
#include <FS.h>
#include <LittleFS.h>
#include <AsyncWebSocket.h>
#include <WiFi.h>
#include "wifi_interface/wifi_manager_wrapper.h"
#include "modbus_handler.h"
#include "config_manager.h"
#include "web_interface/log_utils.h" // Update to use the web_interface-specific log utils

// Include XY-SKxxx header to access power supply functions
#include "XY-SKxxx.h"

// Declare external power supply instance
extern XY_SKxxx* powerSupply;

// Forward declarations for functions used before definition
bool isPSUKeyLocked(XY_SKxxx* powerSupply);
void handleKeyLockRequest(AsyncWebSocketClient* client);
void handleSetKeyLock(AsyncWebSocketClient* client, const JsonObject &json);

AsyncWebSocket ws("/ws");

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".json")) return "application/json";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  return "text/plain";
}

bool handleFileRead(AsyncWebServerRequest *request) {
  String path = request->url();
  if (path.endsWith("/")) path += "index.html";
  
  String contentType = getContentType(path);
  
  // Check if file exists
  if (LittleFS.exists(path)) {
    request->send(LittleFS, path, contentType);
    return true;
  }
  
  // Special case for Apple Touch Icons and favicon - serve default if missing
  if (path.endsWith("apple-touch-icon.png") || 
      path.endsWith("apple-touch-icon-precomposed.png") ||
      path.endsWith("favicon.ico")) {
    
    // If we have a default icon file, serve that instead
    if (LittleFS.exists("/favicon.ico")) {
      request->send(LittleFS, "/favicon.ico", "image/x-icon");
      return true;
    } else {
      // Return a 204 No Content to prevent browser warnings
      request->send(204);
      return true;
    }
  }
  
  return false;
}

// Helper functions for XY-SKxxx power supply interface
// These work with the existing library methods instead of modifying the library

// Get voltage from power supply
float getPSUVoltage(XY_SKxxx* powerSupply) {
  float voltage = 0.0, current = 0.0, power = 0.0;
  if (powerSupply && powerSupply->testConnection()) {
    powerSupply->getOutput(voltage, current, power);
  }
  return voltage;
}

// Get current from power supply
float getPSUCurrent(XY_SKxxx* powerSupply) {
  float voltage = 0.0, current = 0.0, power = 0.0;
  if (powerSupply && powerSupply->testConnection()) {
    powerSupply->getOutput(voltage, current, power);
  }
  return current;
}

// Get power from power supply
float getPSUPower(XY_SKxxx* powerSupply) {
  float voltage = 0.0, current = 0.0, power = 0.0;
  if (powerSupply && powerSupply->testConnection()) {
    powerSupply->getOutput(voltage, current, power);
  }
  return power;
}

// Check if output is enabled - fixed implementation
bool isPSUOutputEnabled(XY_SKxxx* powerSupply) {
  if (powerSupply && powerSupply->testConnection()) {
    return powerSupply->isOutputEnabled(true); // Force refresh from device
  }
  return false;
}

// Set output state (on/off) - fixed implementation
bool setPSUOutput(XY_SKxxx* powerSupply, bool enable) {
  if (powerSupply && powerSupply->testConnection()) {
    if (enable) {
      return powerSupply->turnOutputOn();
    } else {
      return powerSupply->turnOutputOff();
    }
  }
  return false;
}

// Get operating mode from power supply
String getPSUOperatingMode(XY_SKxxx* powerSupply) {
  if (powerSupply && powerSupply->testConnection()) {
    OperatingMode mode = powerSupply->getOperatingMode(true);
    switch (mode) {
      case MODE_CV: return "CV";
      case MODE_CC: return "CC";
      case MODE_CP: return "CP";
      default: return "Unknown";
    }
  }
  return "Unknown";
}

// Get operating mode details including settings
void getPSUOperatingModeDetails(XY_SKxxx* powerSupply, String& modeName, float& setValue) {
  if (!powerSupply || !powerSupply->testConnection()) {
    modeName = "Unknown";
    setValue = 0.0;
    return;
  }
  
  OperatingMode mode = powerSupply->getOperatingMode(true);
  switch (mode) {
    case MODE_CV:
      modeName = "Constant Voltage";
      setValue = powerSupply->getCachedConstantVoltage(false);
      break;
    case MODE_CC:
      modeName = "Constant Current";
      setValue = powerSupply->getCachedConstantCurrent(false);
      break;
    case MODE_CP:
      modeName = "Constant Power";
      setValue = powerSupply->getCachedConstantPower(false);
      break;
    default:
      modeName = "Unknown";
      setValue = 0.0;
  }
}

// Add a unified function to fetch complete PSU status
void sendCompletePSUStatus(AsyncWebSocketClient* client) {
  if (!client || !powerSupply || !powerSupply->testConnection()) {
    return;
  }
  
  // Fetch all status information
  DynamicJsonDocument responseDoc(1024);
  responseDoc["action"] = "statusResponse";
  
  // Get basic readings
  float voltage = getPSUVoltage(powerSupply);
  float current = getPSUCurrent(powerSupply);
  float power = getPSUPower(powerSupply);
  bool outputEnabled = isPSUOutputEnabled(powerSupply);
  
  // Add data to response
  responseDoc["connected"] = true;
  responseDoc["outputEnabled"] = outputEnabled;
  responseDoc["voltage"] = voltage;
  responseDoc["current"] = current;
  responseDoc["power"] = power;
  
  // Add operating mode information - using the backend cache
  String operatingMode = getPSUOperatingMode(powerSupply);
  responseDoc["operatingMode"] = operatingMode;
  
  // Add detailed operating mode information
  String modeName;
  float setValue;
  getPSUOperatingModeDetails(powerSupply, modeName, setValue);
  responseDoc["operatingModeName"] = modeName;
  responseDoc["setValue"] = setValue;
  
  // Add more detailed settings for all modes - using the backend cache
  responseDoc["voltageSet"] = powerSupply->getCachedConstantVoltage(false);
  responseDoc["currentSet"] = powerSupply->getCachedConstantCurrent(false);
  responseDoc["cpModeEnabled"] = powerSupply->isConstantPowerModeEnabled(false);
  responseDoc["powerSet"] = powerSupply->getCachedConstantPower(false);
  
  // Add device info
  responseDoc["model"] = powerSupply->getModel();
  responseDoc["version"] = powerSupply->getVersion();
  
  // Add key lock state to status response - now uses function that's properly declared
  responseDoc["keyLockEnabled"] = isPSUKeyLocked(powerSupply);
  
  // Send the response
  String response;
  serializeJson(responseDoc, response);
  client->text(response);
  
  // Also send specific operating mode information
  sendOperatingModeDetails(client);
}

// Function to specifically send operating mode details
void sendOperatingModeDetails(AsyncWebSocketClient* client) {
  if (!client || !powerSupply || !powerSupply->testConnection()) {
    return;
  }
  
  DynamicJsonDocument responseDoc(256);
  responseDoc["action"] = "operatingModeResponse";
  
  // Get the operating mode - using the backend cache
  OperatingMode mode = powerSupply->getOperatingMode(true);
  String modeCode, modeName;
  float setValue = 0.0;
  
  switch (mode) {
    case MODE_CV:
      modeCode = "CV";
      modeName = "Constant Voltage";
      setValue = powerSupply->getCachedConstantVoltage(false);
      break;
    case MODE_CC:
      modeCode = "CC";
      modeName = "Constant Current";
      setValue = powerSupply->getCachedConstantCurrent(false);
      break;
    case MODE_CP:
      modeCode = "CP";
      modeName = "Constant Power";
      setValue = powerSupply->getCachedConstantPower(false);
      break;
    default:
      modeCode = "Unknown";
      modeName = "Unknown";
  }
  
  responseDoc["success"] = true;
  responseDoc["modeCode"] = modeCode;
  responseDoc["modeName"] = modeName;
  responseDoc["setValue"] = setValue;
  
  // Add detailed settings for all modes
  responseDoc["voltageSet"] = powerSupply->getCachedConstantVoltage(false);
  responseDoc["currentSet"] = powerSupply->getCachedConstantCurrent(false);
  
  // Check if CP mode is enabled and get its set value
  bool cpModeEnabled = powerSupply->isConstantPowerModeEnabled(false);
  responseDoc["cpModeEnabled"] = cpModeEnabled;
  if (cpModeEnabled) {
    responseDoc["powerSet"] = powerSupply->getCachedConstantPower(false);
  }
  
  String response;
  serializeJson(responseDoc, response);
  client->text(response);
}

// Add function to read key lock status from PSU
bool isPSUKeyLocked(XY_SKxxx* powerSupply) {
  if (!powerSupply) return false;
  
  // Force refresh the key lock status to get latest value
  // This is important to detect changes made on the physical device
  return powerSupply->isKeyLocked(true);
}

// Add dedicated key lock status handler
void handleKeyLockRequest(AsyncWebSocketClient* client) {
  XY_SKxxx* psu = powerSupply;
  if (!psu) {
    DynamicJsonDocument doc(256);
    doc["action"] = "keyLockStatusResponse";
    doc["success"] = false;
    doc["error"] = "No PSU connected";
    String response;
    serializeJson(doc, response);
    client->text(response);
    return;
  }
  
  DynamicJsonDocument doc(256);
  doc["action"] = "keyLockStatusResponse";
  doc["success"] = true;
  
  // Force refresh to get current state
  bool isLocked = psu->isKeyLocked(true);
  doc["locked"] = isLocked;
  
  String response;
  serializeJson(doc, response);
  client->text(response);
}

// Change parameter type to const JsonObject& to fix the binding error
void handleSetKeyLock(AsyncWebSocketClient* client, const JsonObject &json) {
  XY_SKxxx* psu = powerSupply;
  if (!psu) {
    DynamicJsonDocument doc(256);
    doc["action"] = "setKeyLockResponse";
    doc["success"] = false;
    doc["error"] = "No PSU connected";
    String response;
    serializeJson(doc, response);
    client->text(response);
    return;
  }
  
  bool lock = json["lock"] | false;
  bool success = psu->setKeyLock(lock);
  
  DynamicJsonDocument doc(256);
  doc["action"] = "setKeyLockResponse";
  doc["success"] = success;
  
  // Always return the actual current state (which may differ if the operation failed)
  doc["locked"] = psu->isKeyLocked(true);
  
  String response;
  serializeJson(doc, response);
  client->text(response);
}

void handleWebSocketMessage(AsyncWebSocket* webSocket, AsyncWebSocketClient* client, 
                           AwsFrameInfo* info, uint8_t* data, size_t len) {
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = String((char*)data);
    
    // Enhanced logging with IP address information
    IPAddress clientIP = client->remoteIP();
    IPAddress serverIP = WiFi.localIP();
    LOG_WS(clientIP, serverIP, "WebSocket received: " + message);
    
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, message);
    
    if (error) {
      LOG_ERROR("deserializeJson() failed: " + String(error.c_str()));
      return;
    }
    
    String action = doc["action"];
    
    // Add ping response handler
    if (action == "ping") {
        // Simply respond with a pong message
        client->text("{\"action\":\"pong\"}");
        LOG_WS(serverIP, clientIP, "WebSocket sent: {\"action\":\"pong\"}");
        return;
    }
    
    if (action == "getData") {
      // Simply call the comprehensive status function instead of duplicating the logic
      sendCompletePSUStatus(client);
    } 
    else if (action == "setConfig") {
      // Handle configuration settings
      client->text("{\"status\":\"success\",\"message\":\"Configuration updated\"}");
    }
    // Power supply control commands
    else if (action == "powerOutput") {
      // Toggle power output on/off - ensure we get the correct current state first
      if (powerSupply && powerSupply->testConnection()) {
        bool enable = doc["enable"];
        LOG_INFO("Power output command received. Setting output to: " + String(enable ? "ON" : "OFF"));
        
        bool success = setPSUOutput(powerSupply, enable);
        
        // Wait a moment for the command to take effect
        delay(100);
        
        // Get current status after change
        bool outputEnabled = isPSUOutputEnabled(powerSupply);
        
        LOG_INFO("Output status after command: " + String(outputEnabled ? "ON" : "OFF"));
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "powerOutputResponse";
        responseDoc["success"] = success;
        responseDoc["enabled"] = outputEnabled;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      } else {
        String errorMsg = "{\"action\":\"powerOutputResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    else if (action == "setVoltage") {
      // Set voltage
      if (powerSupply && powerSupply->testConnection()) {
        float voltage = doc["voltage"];
        bool success = powerSupply->setVoltage(voltage);
        
        // Read current settings after change
        float v = getPSUVoltage(powerSupply);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "setVoltageResponse";
        responseDoc["success"] = success;
        responseDoc["voltage"] = v;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      } else {
        String errorMsg = "{\"action\":\"setVoltageResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    else if (action == "setCurrent") {
      // Set current
      if (powerSupply && powerSupply->testConnection()) {
        float current = doc["current"];
        bool success = powerSupply->setCurrent(current);
        
        // Read current settings after change
        float c = getPSUCurrent(powerSupply);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "setCurrentResponse";
        responseDoc["success"] = success;
        responseDoc["current"] = c;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      } else {
        String errorMsg = "{\"action\":\"setCurrentResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    else if (action == "getStatus") {
      // No need for duplicate code, just call the unified function
      sendCompletePSUStatus(client);
    }
    // Key lock control
    else if (action == "setKeyLock") {
      if (powerSupply && powerSupply->testConnection()) {
        bool lock = doc["lock"];
        LOG_INFO("Key lock command received. Setting keys to: " + String(lock ? "LOCKED" : "UNLOCKED"));
        
        bool success = powerSupply->setKeyLock(lock);
        
        // Get current status after change
        bool keyLocked = powerSupply->isKeyLocked(true);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "keyLockResponse";
        responseDoc["success"] = success;
        responseDoc["locked"] = keyLocked;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      } else {
        String errorMsg = "{\"action\":\"keyLockResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    // Constant Voltage mode
    else if (action == "setConstantVoltage") {
      if (powerSupply && powerSupply->testConnection()) {
        float voltage = doc["voltage"];
        bool success = powerSupply->setConstantVoltage(voltage);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "constantVoltageResponse";
        responseDoc["success"] = success;
        responseDoc["voltage"] = voltage;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
        
        // Wait a moment for the changes to take effect
        delay(100);
        
        // Send updated status and operating mode
        sendCompletePSUStatus(client);
      } else {
        String errorMsg = "{\"action\":\"constantVoltageResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    // Constant Current mode
    else if (action == "setConstantCurrent") {
      if (powerSupply && powerSupply->testConnection()) {
        float current = doc["current"];
        bool success = powerSupply->setConstantCurrent(current);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "constantCurrentResponse";
        responseDoc["success"] = success;
        responseDoc["current"] = current;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
        
        // Wait a moment for the changes to take effect
        delay(100);
        
        // Send updated status and operating mode
        sendCompletePSUStatus(client);
      } else {
        String errorMsg = "{\"action\":\"constantCurrentResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    // Constant Power mode
    else if (action == "setConstantPower") {
      if (powerSupply && powerSupply->testConnection()) {
        float power = doc["power"];
        bool success = powerSupply->setConstantPower(power);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "constantPowerResponse";
        responseDoc["success"] = success;
        responseDoc["power"] = power;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
        
        // Wait a moment for the changes to take effect
        delay(100);
        
        // Send updated status and operating mode
        sendCompletePSUStatus(client);
      } else {
        String errorMsg = "{\"action\":\"constantPowerResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    // Constant Power mode toggle
    else if (action == "setConstantPowerMode") {
      if (powerSupply && powerSupply->testConnection()) {
        bool enable = doc["enable"];
        bool success = powerSupply->setConstantPowerMode(enable);
        
        // Get current state after change
        bool isEnabled = powerSupply->isConstantPowerModeEnabled(true);
        
        // Send response
        DynamicJsonDocument responseDoc(256);
        responseDoc["action"] = "constantPowerModeResponse";
        responseDoc["success"] = success;
        responseDoc["enabled"] = isEnabled;
        
        String response;
        serializeJson(responseDoc, response);
        client->text(response);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
        
        // Wait a moment for the changes to take effect
        delay(100);
        
        // Send updated status and operating mode
        sendCompletePSUStatus(client);
      } else {
        String errorMsg = "{\"action\":\"constantPowerModeResponse\",\"success\":false,\"error\":\"Power supply not connected\"}";
        client->text(errorMsg);
        LOG_WS(serverIP, clientIP, "WebSocket sent: " + errorMsg);
      }
    }
    // Add a specific action to get operating mode details
    else if (action == "getOperatingMode") {
      sendOperatingModeDetails(client);
    }
    // Add a comprehensive status request action
    else if (action == "getStatus") {
      sendCompletePSUStatus(client);
    }
    // Add a WebSocket handler for WiFi status
    else if (action == "getWifiStatus") {
      DynamicJsonDocument responseDoc(256);
      responseDoc["action"] = "wifiStatusResponse"; 
      responseDoc["status"] = isWiFiConnected() ? "connected" : "disconnected";
      responseDoc["ssid"] = getWiFiSSID();
      responseDoc["ip"] = getWiFiIP();
      responseDoc["rssi"] = getWiFiRSSI();
      responseDoc["mac"] = getWiFiMAC();
      
      String response;
      serializeJson(responseDoc, response);
      client->text(response);
      LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
    }
    // Handle incoming message...
    if (action == "getKeyLockStatus") {
      handleKeyLockRequest(client);
      return;
    }
    
    // Handle key lock command - Fix this call by using as<JsonObject>()
    if (action == "setKeyLock") {
      // Convert doc to JsonObject to match function parameter
      handleSetKeyLock(client, doc.as<JsonObject>());
      return;
    }
    
    // Add a handler for time zone settings requests
    if (action == "getTimeZones") {
      String timeZones = getAvailableTimeZones();
      String currentTZ = getCurrentTimeZone();
      
      DynamicJsonDocument responseDoc(1024);
      responseDoc["action"] = "timeZonesResponse";
      responseDoc["timeZones"] = serialized(timeZones);
      responseDoc["current"] = serialized(currentTZ);
      
      String response;
      serializeJson(responseDoc, response);
      client->text(response);
      LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      return;
    }
    
    // Handle time zone setting changes
    if (action == "setTimeZone") {
      int tzIndex = doc["index"];
      
      bool success = setTimeZoneByIndex(tzIndex);
      
      DynamicJsonDocument responseDoc(256);
      responseDoc["action"] = "setTimeZoneResponse";
      responseDoc["success"] = success;
      
      // If successful, include the new time zone info
      if (success) {
        responseDoc["timeZone"] = serialized(getCurrentTimeZone());
      }
      
      String response;
      serializeJson(responseDoc, response);
      client->text(response);
      LOG_WS(serverIP, clientIP, "WebSocket sent: " + response);
      return;
    }
  }
}

void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client, 
              AwsEventType type, void* arg, uint8_t* data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      LOG_INFO("WebSocket client #" + String(client->id()) + " connected from " + client->remoteIP().toString());
      break;
    case WS_EVT_DISCONNECT:
      LOG_INFO("WebSocket client #" + String(client->id()) + " disconnected");
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(server, client, (AwsFrameInfo*)arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void setupWebServer(AsyncWebServer* server) {
  // Try to configure NTP for better logging timestamps
  if (WiFi.status() == WL_CONNECTED) {
    configureNTP();
  }
  
  // Wrap in try-catch to handle possible initialization errors
  try {
    // Initialize WebSocket
    ws.onEvent(onWsEvent);
    server->addHandler(&ws);
    
    // Set up CORS headers for all requests
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");
    
    // Route for root / web page
    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/index.html", "text/html");
    });
    
    // Route to load style.css file
    server->on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/style.css", "text/css");
    });
    
    // Route to load main.js file
    server->on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/main.js", "application/javascript");
    });
    
    // API endpoints - remove the /api/data endpoint that used dummy sensor data
    server->on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
      DynamicJsonDocument doc(1024);
      
      // Add power supply status information instead of modbus data
      if (powerSupply && powerSupply->testConnection()) {
        float voltage = 0, current = 0, power = 0;
        powerSupply->getOutput(voltage, current, power);
        bool outputEnabled = powerSupply->isOutputEnabled(true);
        
        doc["outputEnabled"] = outputEnabled;
        doc["voltage"] = voltage;
        doc["current"] = current;
        doc["power"] = power;
      }
      
      String jsonString;
      serializeJson(doc, jsonString);
      
      // Simply send the string as a response
      request->send(200, "application/json", jsonString);
    });

    server->on("/api/config", HTTP_GET, [](AsyncWebServerRequest *request){
      DynamicJsonDocument doc(1024);
      DeviceConfig config = getConfig();
      
      doc["deviceName"] = config.deviceName;
      doc["modbusId"] = config.modbusId;
      doc["baudRate"] = config.baudRate;
      doc["dataBits"] = config.dataBits;
      doc["parity"] = config.parity;
      doc["stopBits"] = config.stopBits;
      doc["updateInterval"] = config.updateInterval;
      
      String jsonString;
      serializeJson(doc, jsonString);
      
      // Send the JSON string directly
      request->send(200, "application/json", jsonString);
    });

    server->on("/api/config", HTTP_POST, [](AsyncWebServerRequest *request){
      // Dummy response for now
      request->send(200, "application/json", "{\"success\":true,\"message\":\"Configuration saved\"}");
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      // Handle POST data when it's available
      if (len > 0) {
        DynamicJsonDocument doc(1024);
        DeserializationError error = deserializeJson(doc, data, len);
        
        if (!error) {
          DeviceConfig& config = getConfig();
          
          if (doc.containsKey("deviceName")) strlcpy(config.deviceName, doc["deviceName"], sizeof(config.deviceName));
          if (doc.containsKey("modbusId")) config.modbusId = doc["modbusId"];
          if (doc.containsKey("baudRate")) config.baudRate = doc["baudRate"];
          if (doc.containsKey("dataBits")) config.dataBits = doc["dataBits"];
          if (doc.containsKey("parity")) config.parity = doc["parity"];
          if (doc.containsKey("stopBits")) config.stopBits = doc["stopBits"];
          if (doc.containsKey("updateInterval")) config.updateInterval = doc["updateInterval"];
          
          // Save the updated configuration
          saveConfig();
        }
      }
    });

    // WiFi management API endpoints
    server->on("/api/wifi/status", HTTP_GET, [](AsyncWebServerRequest *request){
      DynamicJsonDocument doc(256);
      
      // More robust status response
      doc["status"] = isWiFiConnected() ? "connected" : "disconnected";
      doc["ssid"] = getWiFiSSID();
      doc["ip"] = getWiFiIP();
      doc["rssi"] = getWiFiRSSI();
      doc["mac"] = getWiFiMAC();
      
      // Ensure the response is valid JSON with proper error handling
      String response;
      serializeJson(doc, response);
      
      // Add CORS headers for this specific endpoint
      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      resp->addHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
      resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(resp);
    });
    
    server->on("/api/wifi/reset", HTTP_POST, [](AsyncWebServerRequest *request){
      // This will trigger a WiFi settings reset and device restart
      AsyncWebServerResponse *response = request->beginResponse(200, "application/json", 
                                                              "{\"status\":\"success\",\"message\":\"WiFi settings reset. Device will restart...\"}");
      request->send(response);
      
      // Schedule WiFi reset for after the response is sent
      delay(500);
      resetWiFiSettings();
      delay(500);
      ESP.restart();
    });
    
    // Add an API endpoint for time zone settings
    server->on("/api/timezone", HTTP_GET, [](AsyncWebServerRequest *request){
      String timeZones = getAvailableTimeZones();
      String currentTZ = getCurrentTimeZone();
      
      DynamicJsonDocument doc(1024);
      doc["timeZones"] = serialized(timeZones);
      doc["current"] = serialized(currentTZ);
      
      String jsonString;
      serializeJson(doc, jsonString);
      
      // Add CORS headers
      AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", jsonString);
      resp->addHeader("Access-Control-Allow-Origin", "*");
      resp->addHeader("Access-Control-Allow-Methods", "GET, OPTIONS");
      resp->addHeader("Access-Control-Allow-Headers", "Content-Type");
      request->send(resp);
    });
    
    server->on("/api/timezone", HTTP_POST, [](AsyncWebServerRequest *request){
      request->send(200, "application/json", "{\"success\":true}");
    }, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      if (len > 0) {
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, data, len);
        
        if (!error && doc.containsKey("index")) {
          int tzIndex = doc["index"];
          bool success = setTimeZoneByIndex(tzIndex);
          
          // Respond with success status
          String response = "{\"success\":" + String(success ? "true" : "false") + "}";
          AsyncWebServerResponse *resp = request->beginResponse(200, "application/json", response);
          resp->addHeader("Access-Control-Allow-Origin", "*");
          request->send(resp);
        }
      }
    });

    // Add a simple health check endpoint
    server->on("/health", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "OK");
    });
    
    // Add a simple health check endpoint that doesn't require AsyncTCP
    server->on("/ping", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(200, "text/plain", "pong");
    });
    
    LOG_INFO("Web server routes configured successfully");
  } 
  catch (const std::exception& e) {
    LOG_ERROR("Error setting up web server routes");
  }
  
  // Handle file reads
  server->onNotFound([](AsyncWebServerRequest *request){
    if (!handleFileRead(request)) {
      request->send(404, "text/plain", "File Not Found");
    }
  });
}