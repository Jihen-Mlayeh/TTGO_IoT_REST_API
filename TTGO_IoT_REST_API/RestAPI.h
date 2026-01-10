#ifndef REST_API_H
#define REST_API_H

#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"
#include "TemperatureControl.h"
#include "PhotocellControl.h"
#include "LedControl.h"

class RestAPI {
private:
    WebServer* _server;
    TemperatureControl* _tempSensor;
    PhotocellControl* _lightSensor;
    LedControl* _led;
    
    float _tempThreshold;
    int _lightThreshold;
    bool _autoMode;
    String _currentMode;  // ✅ NOUVEAU
    
    void handleGetSensors();
    void handleGetTemperature();
    void handleGetLight();
    void handleLedOn();
    void handleLedOff();
    void handleLedToggle();
    void handleSetThreshold();
    void handleGetThreshold();
    void handleSetMode();  // ✅ NOUVEAU
    void handleGetStatus();
    void handleNotFound();
    
public:
    RestAPI(WebServer* server, TemperatureControl* temp, 
            PhotocellControl* light, LedControl* led);
    void begin();
    void handleClient();
    void setThreshold(float temp, int light);
    void setAutoMode(bool mode);
    bool getAutoMode();
    void setCurrentMode(String mode);  // ✅ NOUVEAU
    void updateAutoMode(float currentTemp, int currentLight);
};

#endif
