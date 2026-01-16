#include "RestAPI.h"

// ========================================
// CONSTRUCTEUR
// ========================================
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

// ========================================
// HEADERS CORS
// ========================================
void RestAPI::sendCorsHeaders() {
    _server->sendHeader("Access-Control-Allow-Origin", "*");
    _server->sendHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    _server->sendHeader("Access-Control-Allow-Headers", "Content-Type, Authorization");
    _server->sendHeader("Access-Control-Max-Age", "86400");
}

// ========================================
// INITIALISATION DU SERVEUR
// ========================================
void RestAPI::begin() {
    // Handlers OPTIONS pour CORS
    _server->on("/sensors", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/sensors/temperature", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/sensors/light", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/led/on", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/led/off", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/led/toggle", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/threshold/set", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/threshold", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/mode/set", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });
    _server->on("/status", HTTP_OPTIONS, [this]() { sendCorsHeaders(); _server->send(204); });

    // Routes normales
    _server->on("/sensors", HTTP_GET, [this]() { handleGetSensors(); });
    _server->on("/sensors/temperature", HTTP_GET, [this]() { handleGetTemperature(); });
    _server->on("/sensors/light", HTTP_GET, [this]() { handleGetLight(); });
    _server->on("/led/on", HTTP_POST, [this]() { handleLedOn(); });
    _server->on("/led/off", HTTP_POST, [this]() { handleLedOff(); });
    _server->on("/led/toggle", HTTP_POST, [this]() { handleLedToggle(); });
    _server->on("/threshold/set", HTTP_POST, [this]() { handleSetThreshold(); });
    _server->on("/threshold", HTTP_GET, [this]() { handleGetThreshold(); });
    _server->on("/mode/set", HTTP_POST, [this]() { handleSetMode(); });
    _server->on("/status", HTTP_GET, [this]() { handleGetStatus(); });
    _server->onNotFound([this]() { handleNotFound(); });

    _server->begin();
}

// ========================================
// GESTION CLIENT
// ========================================
void RestAPI::handleClient() {
    _server->handleClient();
}

// ========================================
// HANDLERS API
// ========================================
void RestAPI::handleGetSensors() {
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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
    sendCorsHeaders();
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

void RestAPI::handleNotFound() {
    sendCorsHeaders();
    StaticJsonDocument<200> doc;
    String response;

    doc["code"] = 404;
    doc["status"] = "ERROR";
    doc["message"] = "Route non trouvee";

    serializeJson(doc, response);
    _server->send(404, "application/json", response);
}

// ========================================
// FONCTIONS UTILITAIRES
// ========================================
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

String RestAPI::getCurrentMode() {
    return _currentMode;
}

void RestAPI::updateAutoMode(float currentTemp, int currentLightPercent) {
    if (!_autoMode) return;

    if (_currentMode == "AUTO-TEMP") {
        if (currentTemp > _tempThreshold) {
            _led->on();
        } else {
            _led->off();
        }
    } else if (_currentMode == "AUTO-LIGHT") {
        if (currentLightPercent < _lightThreshold) {
            _led->on();
        } else {
            _led->off();
        }
    }
}
