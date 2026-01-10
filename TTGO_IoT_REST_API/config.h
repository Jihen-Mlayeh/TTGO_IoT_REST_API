#ifndef CONFIG_H
#define CONFIG_H

// ========================================
// CONFIGURATION WIFI
// ========================================
#define WIFI_SSID "PlanetCampus - Prive 004347"
#define WIFI_PASSWORD "CM337tbXD9o6F"

// ========================================
// CONFIGURATION FIREBASE
// ========================================
#define API_KEY "AIzaSyC0OjWPyd-qCoButAayU1vmc-ph66RN4Lc"
#define DATABASE_URL "https://ttgo-iot-project-default-rtdb.europe-west1.firebasedatabase.app"
#define USER_EMAIL "mlayehjihen@gmail.com"
#define USER_PASSWORD "123456"

// ========================================
// CONFIGURATION PINS
// ========================================
#define LED_PIN 32
#define TEMP_SENSOR_PIN 36
#define LDR_PIN 33
#define BUTTON_LEFT 0
#define BUTTON_RIGHT 35

// ========================================
// PARAMÈTRES THERMISTANCE NTC
// ========================================
#define R0 10000.0
#define R_AT_25C 10000.0
#define B_COEFFICIENT 3950.0
#define T0_KELVIN 298.15
#define VREF 5.0
#define ADC_MAX 4095.0
#define ESP32_VMAX 3.3

// ========================================
// PARAMÈTRES SYSTÈME
// ========================================
#define FILTER_SIZE 10
#define FIREBASE_UPDATE_INTERVAL 5000

#endif
