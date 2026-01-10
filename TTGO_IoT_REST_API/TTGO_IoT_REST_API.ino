// ========================================
// TTGO IoT REST API - Projet ESP32
// ========================================

#include <WiFi.h>
#include <WebServer.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <TFT_eSPI.h>

#include "config.h"
#include "TemperatureControl.h"
#include "PhotocellControl.h"
#include "LedControl.h"
#include "DisplayControl.h"
#include "RestAPI.h"

// ========================================
// OBJETS GLOBAUX
// ========================================
TFT_eSPI tft = TFT_eSPI();
WebServer server(80);

TemperatureControl tempSensor(TEMP_SENSOR_PIN);
PhotocellControl lightSensor(LDR_PIN);
LedControl led(LED_PIN);
DisplayControl display(&tft);
RestAPI api(&server, &tempSensor, &lightSensor, &led);

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variables globales
bool wifiConnected = false;
bool firebaseReady = false;
unsigned long lastFirebaseUpdate = 0;
unsigned long lastDisplayUpdate = 0;

bool lastButtonLeft = HIGH;
bool lastButtonRight = HIGH;
String currentMode = "MANUEL";

// ========================================
// SETUP
// ========================================
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("   TTGO IoT REST API");
  Serial.println("========================================\n");
  
  // Initialisation composants
  tempSensor.begin();
  lightSensor.begin();
  led.begin();
  display.begin();
  
  pinMode(BUTTON_LEFT, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT, INPUT_PULLUP);
  
  display.showMessage("TTGO IoT", "Demarrage...", TFT_CYAN);
  delay(1500);
  
  // ========================================
  // CONNEXION WIFI
  // ========================================
  display.showMessage("WiFi", "Connexion...", TFT_YELLOW);
  Serial.println(">>> Connexion WiFi...");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int wifiAttempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifiAttempts < 40) {
    delay(500);
    Serial.print(".");
    wifiAttempts++;
    if(wifiAttempts % 10 == 0) {
      Serial.println();
      Serial.print("Tentative ");
      Serial.print(wifiAttempts);
      Serial.print("/40...");
    }
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi OK!");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    
    display.showMessage("WiFi OK", WiFi.localIP().toString(), TFT_GREEN);
    delay(2000);
    
    // ========================================
    // DÉMARRAGE API REST
    // ========================================
    display.showMessage("API REST", "Demarrage...", TFT_YELLOW);
    Serial.println("\n>>> Demarrage serveur HTTP...");
    
    api.begin();
    
    Serial.println("Serveur HTTP demarre!");
    Serial.print("Adresse API: http://");
    Serial.println(WiFi.localIP());
    
    display.showMessage("API REST", "Demarre!", TFT_GREEN);
    delay(1500);
    
    // ========================================
    // CONNEXION FIREBASE
    // ========================================
    display.showMessage("Firebase", "Connexion...", TFT_YELLOW);
    Serial.println("\n>>> Configuration Firebase...");
    
    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    config.token_status_callback = tokenStatusCallback;
    
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);
    
    int fbAttempts = 0;
    while (!Firebase.ready() && fbAttempts < 15) {
      delay(500);
      Serial.print(".");
      fbAttempts++;
    }
    Serial.println();
    
    if (Firebase.ready()) {
      firebaseReady = true;
      Serial.println("Firebase OK!");
      display.showMessage("Firebase", "Connecte!", TFT_GREEN);
      delay(1500);
    } else {
      Serial.println("Firebase timeout");
      display.showMessage("Firebase", "Timeout", TFT_ORANGE);
      delay(1500);
    }
    
  } else {
    Serial.println("WiFi ERREUR");
    display.showMessage("WiFi", "ERREUR", TFT_RED);
    delay(3000);
  }
  
  display.showMessage("Systeme", "PRET!", TFT_GREEN);
  delay(1000);
  
  Serial.println("\n========================================");
  Serial.println("   Systeme operationnel");
  Serial.println("========================================");
  Serial.println("\nRoutes API disponibles:");
  Serial.println("  GET  /sensors");
  Serial.println("  GET  /sensors/temperature");
  Serial.println("  GET  /sensors/light");
  Serial.println("  POST /led/on");
  Serial.println("  POST /led/off");
  Serial.println("  POST /led/toggle");
  Serial.println("  POST /threshold/set?temp=30&light=50");
  Serial.println("  GET  /threshold");
  Serial.println("  POST /mode/set?mode=AUTO-TEMP");
  Serial.println("  GET  /status");
  Serial.println("\n========================================\n");
}

// ========================================
// LOOP PRINCIPAL
// ========================================
void loop() {
  // ========================================
  // GESTION SERVEUR HTTP
  // ========================================
  api.handleClient();
  
  // ========================================
  // LECTURE CAPTEURS
  // ========================================
  float temperature = tempSensor.readTemperature();
  int lightRaw = lightSensor.readValue();
  int lightPercent = lightSensor.readPercent();
  
  // ========================================
  // GESTION BOUTONS
  // ========================================
  bool btnL = digitalRead(BUTTON_LEFT);
  bool btnR = digitalRead(BUTTON_RIGHT);
  
  // Bouton GAUCHE : Toggle LED
  if (lastButtonLeft == HIGH && btnL == LOW) {
    delay(50);
    led.toggle();
    Serial.print("LED: ");
    Serial.println(led.getState() ? "ON" : "OFF");
  }
  
  // Bouton DROIT : Cycle modes
  if (lastButtonRight == HIGH && btnR == LOW) {
    delay(50);
    if (currentMode == "MANUEL") {
      currentMode = "AUTO-TEMP";
      api.setAutoMode(true);  // ✅ ACTIVE autoMode
      api.setCurrentMode("AUTO-TEMP");  // ✅ NOUVEAU : informe l'API du mode
      api.setThreshold(30.0, 50);
      Serial.println(">>> Mode AUTO-TEMP");
    } else if (currentMode == "AUTO-TEMP") {
      currentMode = "AUTO-LIGHT";
      api.setAutoMode(true);  // ✅ RESTE true
      api.setCurrentMode("AUTO-LIGHT");  // ✅ NOUVEAU
      Serial.println(">>> Mode AUTO-LIGHT");
    } else {
      currentMode = "MANUEL";
      api.setAutoMode(false);  // ✅ DÉSACTIVE autoMode
      api.setCurrentMode("MANUEL");  // ✅ NOUVEAU
      Serial.println(">>> Mode MANUEL");
    }
  }
  
  lastButtonLeft = btnL;
  lastButtonRight = btnR;
  
  // ========================================
  // MODE AUTOMATIQUE
  // ========================================
  if (api.getAutoMode()) {
    api.updateAutoMode(temperature, lightPercent);
  }
  
  // ========================================
  // ENVOI FIREBASE
  // ========================================
  if (wifiConnected && firebaseReady) {
    if (millis() - lastFirebaseUpdate >= FIREBASE_UPDATE_INTERVAL) {
      if (Firebase.ready()) {
        Serial.println(">>> Envoi Firebase...");
        
        Firebase.RTDB.setFloat(&fbdo, "/sensors/temperature", temperature);
        Firebase.RTDB.setInt(&fbdo, "/sensors/lightRaw", lightRaw);
        Firebase.RTDB.setInt(&fbdo, "/sensors/lightPercent", lightPercent);
        Firebase.RTDB.setBool(&fbdo, "/actuators/led", led.getState());
        Firebase.RTDB.setString(&fbdo, "/settings/mode", currentMode);
        Firebase.RTDB.setBool(&fbdo, "/settings/autoMode", api.getAutoMode());  // ✅ AJOUTÉ
        Firebase.RTDB.setTimestamp(&fbdo, "/sensors/lastUpdate");
        
        Serial.println("OK");
        lastFirebaseUpdate = millis();
      }
    }
  }
  
  // ========================================
  // MISE À JOUR ÉCRAN
  // ========================================
  if (millis() - lastDisplayUpdate >= 500) {
    display.showStatus(temperature, lightPercent, led.getState(), 
                       wifiConnected, firebaseReady, currentMode);
    lastDisplayUpdate = millis();
  }
  
  // ========================================
  // SERIAL MONITOR
  // ========================================
  static unsigned long lastSerialPrint = 0;
  if (millis() - lastSerialPrint > 2000) {
    Serial.printf("T:%.1fC | L:%d%% | LED:%s | Mode:%s\n", 
                  temperature, lightPercent,
                  led.getState() ? "ON" : "OFF",
                  currentMode.c_str());
    lastSerialPrint = millis();
  }
  
  delay(50);
}
