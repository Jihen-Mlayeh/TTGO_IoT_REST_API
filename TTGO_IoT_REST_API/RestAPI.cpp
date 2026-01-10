#include "RestAPI.h"

RestAPI::RestAPI(WebServer* server, TemperatureControl* temp, 
                 PhotocellControl* light, LedControl* led) {
    _server = server;
    _tempSensor = temp;
    _lightSensor = light;
    _led = led;
    _tempThreshold = 30.0;
    _lightThreshold = 2000;
    _autoMode = false;
}

void RestAPI::begin() {
    // Routes API
    _server->on("/sensors", HTTP_GET, [this]() { this->handleGetSensors(); });
    _server->on("/sensors/temperature", HTTP_GET, [this]() { this->handleGetTemperature(); });
    _server->on("/sensors/light", HTTP_GET, [this]() { this->handleGetLight(); });
    _server->on("/led/on", HTTP_POST, [this]() { this->handleLedOn(); });
    _server->on("/led/off", HTTP_POST, [this]() { this->handleLedOff(); });
    _server->on("/led/toggle", HTTP_POST, [this]() { this->handleLedToggle(); });
    _server->on("/threshold/set", HTTP_POST, [this]() { this->handleSetThreshold(); });
    _server->on("/threshold", HTTP_GET, [this]() { this->handleGetThreshold(); });
    _server->on("/status", HTTP_GET, [this]() { this->handleGetStatus(); });
    _server->onNotFound([this]() { this->handleNotFound(); });
    
    _server->begin();
}

void RestAPI::handleClient() {
    _server->handleClient();
}

void RestAPI::handleGetSensors() {
    StaticJsonDocument<300> doc;
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
    doc["message"] = "LED allumee avec succes";
    
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
    doc["message"] = "LED eteinte avec succes";
    
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
    doc["message"] = "LED basculee avec succes";
    
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
    _autoMode = true;
    
    doc["code"] = 200;
    doc["status"] = "OK";
    doc["temp_threshold"] = _tempThreshold;
    doc["light_threshold"] = _lightThreshold;
    doc["auto_mode"] = true;
    doc["message"] = "Seuils definis avec succes";
    
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
    
    serializeJson(doc, response);
    _server->send(200, "application/json", response);
}

void RestAPI::handleGetStatus() {
    StaticJsonDocument<400> doc;
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
    settings["temp_threshold"] = _tempThreshold;
    settings["light_threshold"] = _lightThreshold;
    
    serializeJson(doc, response);
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

void RestAPI::updateAutoMode(float currentTemp, int currentLight) {
    if (_autoMode) {
        if (currentTemp > _tempThreshold || currentLight < _lightThreshold) {
            _led->on();
        } else {
            _led->off();
        }
    }
}
