#include "RestAPI.h"

RestAPI::RestAPI(WebServer* server, TemperatureControl* temp, 
                 PhotocellControl* light, LedControl* led) {
    _server = server;
    _tempSensor = temp;
    _lightSensor = light;
    _led = led;
    _tempThreshold = 30.0;
    _lightThreshold = 50;
    _autoMode = false;
    _currentMode = "MANUEL";
}

void RestAPI::begin() {
    _server->on("/sensors", HTTP_GET, [this]() { this->handleGetSensors(); });
    _server->on("/sensors/temperature", HTTP_GET, [this]() { this->handleGetTemperature(); });
    _server->on("/sensors/light", HTTP_GET, [this]() { this->handleGetLight(); });
    _server->on("/led/on", HTTP_POST, [this]() { this->handleLedOn(); });
    _server->on("/led/off", HTTP_POST, [this]() { this->handleLedOff(); });
    _server->on("/led/toggle", HTTP_POST, [this]() { this->handleLedToggle(); });
    _server->on("/threshold/set", HTTP_POST, [this]() { this->handleSetThreshold(); });
    _server->on("/threshold", HTTP_GET, [this]() { this->handleGetThreshold(); });
    _server->on("/mode/set", HTTP_POST, [this]() { this->handleSetMode(); });
    _server->on("/status", HTTP_GET, [this]() { this->handleGetStatus(); });
    _server->on("/api-docs", HTTP_GET, [this]() { this->handleApiDocs(); });  // ✅ NOUVEAU
    _server->onNotFound([this]() { this->handleNotFound(); });
    
    _server->begin();
}

void RestAPI::handleClient() {
    _server->handleClient();
}

void RestAPI::handleGetSensors() {
    StaticJsonDocument<256> doc;
    String response;
    
    doc["code"] = 200;
    doc["status"] = "OK";
    
    JsonArray sensors = doc.createNestedArray("sensors");
    
    JsonObject temp = sensors.createNestedObject();
    temp["type"] = "temperature";
    temp["name"] = "NTC 10k";
    temp["pin"] = TEMP_SENSOR_PIN;
    temp["unit"] = "celsius";
    
    JsonObject light = sensors.createNestedObject();
    light["type"] = "light";
    light["name"] = "Photocell";
    light["pin"] = LDR_PIN;
    light["unit"] = "percent";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleGetTemperature() {
    StaticJsonDocument<200> doc;
    String response;
    
    float temp = _tempSensor->readTemperature();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["temperature_celsius"] = temp;
    doc["sensor"] = "NTC 10k";
    doc["pin"] = TEMP_SENSOR_PIN;
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleGetLight() {
    StaticJsonDocument<200> doc;
    String response;
    
    int lightRaw = _lightSensor->readValue();
    int lightPercent = _lightSensor->readPercent();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["light_raw"] = lightRaw;
    doc["light_percent"] = lightPercent;
    doc["sensor"] = "Photocell";
    doc["pin"] = LDR_PIN;
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleLedOn() {
    StaticJsonDocument<200> doc;
    String response;
    
    _led->on();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["led_state"] = "ON";
    doc["message"] = "LED allumee";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleLedOff() {
    StaticJsonDocument<200> doc;
    String response;
    
    _led->off();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["led_state"] = "OFF";
    doc["message"] = "LED eteinte";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleLedToggle() {
    StaticJsonDocument<200> doc;
    String response;
    
    _led->toggle();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["led_state"] = _led->getState() ? "ON" : "OFF";
    doc["message"] = "LED basculee";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleSetThreshold() {
    StaticJsonDocument<200> doc;
    String response;
    
    if (!_server->hasArg("temp") || !_server->hasArg("light")) {
        doc["code"] = 400;
        doc["status"] = "ERROR";
        doc["message"] = "Parametres manquants: temp, light";
        serializeJson(doc, response);
        _server->send(400, "application/json", response);
        return;
    }
    
    _tempThreshold = _server->arg("temp").toFloat();
    _lightThreshold = _server->arg("light").toInt();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["temp_threshold"] = _tempThreshold;
    doc["light_threshold"] = _lightThreshold;
    doc["auto_mode"] = _autoMode;
    doc["message"] = "Seuils definis";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleGetThreshold() {
    StaticJsonDocument<200> doc;
    String response;
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["temp_threshold"] = _tempThreshold;
    doc["light_threshold"] = _lightThreshold;
    doc["auto_mode"] = _autoMode;
    doc["current_mode"] = _currentMode;
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleSetMode() {
    StaticJsonDocument<200> doc;
    String response;
    
    if (!_server->hasArg("mode")) {
        doc["code"] = 400;
        doc["status"] = "ERROR";
        doc["message"] = "Parametre manquant: mode (MANUEL, AUTO-TEMP, AUTO-LIGHT)";
        serializeJson(doc, response);
        _server->send(400, "application/json", response);
        return;
    }
    
    String mode = _server->arg("mode");
    mode.toUpperCase();
    
    if (mode == "AUTO-TEMP") {
        _autoMode = true;
        _currentMode = "AUTO-TEMP";
    } else if (mode == "AUTO-LIGHT") {
        _autoMode = true;
        _currentMode = "AUTO-LIGHT";
    } else if (mode == "MANUEL") {
        _autoMode = false;
        _currentMode = "MANUEL";
    } else {
        doc["code"] = 400;
        doc["status"] = "ERROR";
        doc["message"] = "Mode invalide. Utilisez: MANUEL, AUTO-TEMP, AUTO-LIGHT";
        serializeJson(doc, response);
        _server->send(400, "application/json", response);
        return;
    }
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["mode"] = _currentMode;
    doc["auto_mode"] = _autoMode;
    doc["message"] = "Mode defini avec succes";
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleGetStatus() {
    StaticJsonDocument<300> doc;
    String response;
    
    float temp = _tempSensor->readTemperature();
    int lightRaw = _lightSensor->readValue();
    int lightPercent = _lightSensor->readPercent();
    bool ledState = _led->getState();
    
    doc["code"] = 200;
    doc["status"] = "OK";
    
    JsonObject sensors = doc.createNestedObject("sensors");
    sensors["temperature"] = temp;
    sensors["light_raw"] = lightRaw;
    sensors["light_percent"] = lightPercent;
    
    JsonObject actuators = doc.createNestedObject("actuators");
    actuators["led"] = ledState;
    
    JsonObject settings = doc.createNestedObject("settings");
    settings["auto_mode"] = _autoMode;
    settings["current_mode"] = _currentMode;
    settings["temp_threshold"] = _tempThreshold;
    settings["light_threshold"] = _lightThreshold;
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

// ✅ NOUVELLE FONCTION : Documentation OpenAPI/Swagger
void RestAPI::handleApiDocs() {
    // Note: Pour ESP32, on divise en plusieurs parties pour gérer la mémoire
    String response = "";
    
    // Partie 1: En-tête OpenAPI
    response += "{\n";
    response += "  \"openapi\": \"3.0.0\",\n";
    response += "  \"info\": {\n";
    response += "    \"title\": \"TTGO IoT REST API\",\n";
    response += "    \"version\": \"1.0.0\",\n";
    response += "    \"description\": \"API REST pour controle ESP32 TTGO avec capteurs temperature et lumiere\"\n";
    response += "  },\n";
    response += "  \"servers\": [{\n";
    response += "    \"url\": \"http://";
    response += WiFi.localIP().toString();
    response += "\",\n";
    response += "    \"description\": \"Serveur ESP32 local\"\n";
    response += "  }],\n";
    response += "  \"paths\": {\n";
    
    // GET /sensors
    response += "    \"/sensors\": {\n";
    response += "      \"get\": {\n";
    response += "        \"summary\": \"Liste tous les capteurs\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"Liste des capteurs disponibles\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // GET /sensors/temperature
    response += "    \"/sensors/temperature\": {\n";
    response += "      \"get\": {\n";
    response += "        \"summary\": \"Lecture temperature\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"Temperature en Celsius\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // GET /sensors/light
    response += "    \"/sensors/light\": {\n";
    response += "      \"get\": {\n";
    response += "        \"summary\": \"Lecture lumiere\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"Niveau de lumiere (raw et pourcentage)\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // POST /led/on
    response += "    \"/led/on\": {\n";
    response += "      \"post\": {\n";
    response += "        \"summary\": \"Allumer la LED\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"LED allumee\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // POST /led/off
    response += "    \"/led/off\": {\n";
    response += "      \"post\": {\n";
    response += "        \"summary\": \"Eteindre la LED\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"LED eteinte\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // POST /led/toggle
    response += "    \"/led/toggle\": {\n";
    response += "      \"post\": {\n";
    response += "        \"summary\": \"Basculer l'etat de la LED\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"LED basculee\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // POST /threshold/set
    response += "    \"/threshold/set\": {\n";
    response += "      \"post\": {\n";
    response += "        \"summary\": \"Definir les seuils\",\n";
    response += "        \"parameters\": [\n";
    response += "          {\n";
    response += "            \"name\": \"temp\",\n";
    response += "            \"in\": \"query\",\n";
    response += "            \"required\": true,\n";
    response += "            \"schema\": {\"type\": \"number\"},\n";
    response += "            \"description\": \"Seuil de temperature en Celsius\"\n";
    response += "          },\n";
    response += "          {\n";
    response += "            \"name\": \"light\",\n";
    response += "            \"in\": \"query\",\n";
    response += "            \"required\": true,\n";
    response += "            \"schema\": {\"type\": \"integer\"},\n";
    response += "            \"description\": \"Seuil de lumiere en pourcentage\"\n";
    response += "          }\n";
    response += "        ],\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\"description\": \"Seuils definis\"},\n";
    response += "          \"400\": {\"description\": \"Parametres manquants\"}\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // GET /threshold
    response += "    \"/threshold\": {\n";
    response += "      \"get\": {\n";
    response += "        \"summary\": \"Obtenir les seuils actuels\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"Seuils et mode actuel\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // POST /mode/set
    response += "    \"/mode/set\": {\n";
    response += "      \"post\": {\n";
    response += "        \"summary\": \"Definir le mode de fonctionnement\",\n";
    response += "        \"parameters\": [{\n";
    response += "          \"name\": \"mode\",\n";
    response += "          \"in\": \"query\",\n";
    response += "          \"required\": true,\n";
    response += "          \"schema\": {\n";
    response += "            \"type\": \"string\",\n";
    response += "            \"enum\": [\"MANUEL\", \"AUTO-TEMP\", \"AUTO-LIGHT\"]\n";
    response += "          },\n";
    response += "          \"description\": \"Mode: MANUEL, AUTO-TEMP ou AUTO-LIGHT\"\n";
    response += "        }],\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\"description\": \"Mode defini\"},\n";
    response += "          \"400\": {\"description\": \"Mode invalide\"}\n";
    response += "        }\n";
    response += "      }\n";
    response += "    },\n";
    
    // GET /status
    response += "    \"/status\": {\n";
    response += "      \"get\": {\n";
    response += "        \"summary\": \"Status complet du systeme\",\n";
    response += "        \"responses\": {\n";
    response += "          \"200\": {\n";
    response += "            \"description\": \"Capteurs, actuateurs et parametres\"\n";
    response += "          }\n";
    response += "        }\n";
    response += "      }\n";
    response += "    }\n";
    
    response += "  }\n";
    response += "}\n";
    
    _server->send(200, "application/json", response);
}

void RestAPI::handleNotFound() {
    StaticJsonDocument<200> doc;
    String response;
    
    doc["code"] = 404;
    doc["status"] = "ERROR";
    doc["message"] = "Route non trouvee";
    
    serializeJson(doc, response);
    _server->send(404, "application/json", response);
}

void RestAPI::setThreshold(float temp, int light) {
    _tempThreshold = temp;
    _lightThreshold = light;
}

void RestAPI::setAutoMode(bool mode) {
    _autoMode = mode;
}

bool RestAPI::getAutoMode() {
    return _autoMode;
}

void RestAPI::setCurrentMode(String mode) {
    _currentMode = mode;
}

void RestAPI::updateAutoMode(float currentTemp, int currentLightPercent) {
    if (!_autoMode) return;
    
    if (_currentMode == "AUTO-TEMP") {
        if (currentTemp > _tempThreshold) {
            _led->on();
        } else {
            _led->off();
        }
    } 
    else if (_currentMode == "AUTO-LIGHT") {
        if (currentLightPercent < _lightThreshold) {
            _led->on();
        } else {
            _led->off();
        }
    }
}