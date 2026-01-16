// test_Temperature.ino
// Tests unitaires pour TemperatureControl

#include "TemperatureControl.h"
#include "config.h"

TemperatureControl* tempSensor;
int testsPassed = 0;
int testsFailed = 0;

// ========================================
// FONCTIONS DE TEST
// ========================================

void printTestHeader(String testName) {
    Serial.println("\n========================================");
    Serial.println("TEST: " + testName);
    Serial.println("========================================");
}

void printTestResult(bool passed) {
    if (passed) {
        Serial.println("✅ RÉUSSI");
        testsPassed++;
    } else {
        Serial.println("❌ ÉCHOUÉ");
        testsFailed++;
    }
}

// ========================================
// TEST 1: Lecture température ambiante
// ========================================
void test_temperature_ambient() {
    printTestHeader("Température ambiante (12-20°C)");
    
    float temp = tempSensor->readTemperature();
    
    Serial.print("Température mesurée: ");
    Serial.print(temp, 1);
    Serial.println(" °C");
    
    // Vérifie que la température est dans une plage réaliste
    bool passed = (temp >= 10.0 && temp <= 25.0);
    
    if (!passed) {
        Serial.println("⚠️ Température hors de la plage attendue (10-25°C)");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 2: Stabilité des lectures
// ========================================
void test_temperature_stability() {
    printTestHeader("Stabilité des lectures (10 mesures)");
    
    float readings[10];
    float sum = 0;
    
    // Prend 10 lectures
    for (int i = 0; i < 10; i++) {
        readings[i] = tempSensor->readTemperature();
        sum += readings[i];
        delay(100);
    }
    
    float average = sum / 10.0;
    
    // Calcule l'écart max par rapport à la moyenne
    float maxDeviation = 0;
    for (int i = 0; i < 10; i++) {
        float deviation = abs(readings[i] - average);
        if (deviation > maxDeviation) {
            maxDeviation = deviation;
        }
    }
    
    Serial.print("Moyenne: ");
    Serial.print(average, 1);
    Serial.println(" °C");
    Serial.print("Écart max: ");
    Serial.print(maxDeviation, 2);
    Serial.println(" °C");
    
    // L'écart ne doit pas dépasser 2°C
    bool passed = (maxDeviation < 2.0);
    
    if (!passed) {
        Serial.println("⚠️ Écart trop important (>2°C)");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 3: Lecture ADC brute
// ========================================
void test_raw_adc() {
    printTestHeader("Lecture ADC brute");
    
    int rawADC = tempSensor->getRawADC();
    
    Serial.print("Valeur ADC: ");
    Serial.println(rawADC);
    
    // Vérifie que l'ADC est dans la plage valide (0-4095)
    bool passed = (rawADC >= 0 && rawADC <= 4095);
    
    if (!passed) {
        Serial.println("⚠️ Valeur ADC invalide");
    } else {
        // Affiche des infos supplémentaires
        float voltage = (rawADC / 4095.0) * 3.3;
        Serial.print("Tension correspondante: ");
        Serial.print(voltage, 3);
        Serial.println(" V");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 4: Conversion ADC → Tension
// ========================================
void test_adc_to_voltage() {
    printTestHeader("Conversion ADC → Tension");
    
    // Test avec différentes valeurs ADC
    int testValues[] = {0, 1024, 2048, 3072, 4095};
    float expectedVoltages[] = {0.0, 0.825, 1.65, 2.475, 3.3};
    
    bool allPassed = true;
    
    for (int i = 0; i < 5; i++) {
        float voltage = (testValues[i] / 4095.0) * 3.3;
        float error = abs(voltage - expectedVoltages[i]);
        
        Serial.print("ADC ");
        Serial.print(testValues[i]);
        Serial.print(" → ");
        Serial.print(voltage, 3);
        Serial.print(" V (attendu: ");
        Serial.print(expectedVoltages[i], 3);
        Serial.println(" V)");
        
        if (error > 0.01) {
            allPassed = false;
        }
    }
    
    printTestResult(allPassed);
}

// ========================================
// TEST 5: Calcul résistance NTC
// ========================================
void test_resistance_calculation() {
    printTestHeader("Calcul résistance NTC");
    
    float testVoltages[] = {1.0, 1.5, 2.0, 2.5};
    float resistor = 10000.0;  // R0 renommé en resistor
    float vref = 3.3;          // Vref renommé en vref
    
    bool allValid = true;
    
    for (int i = 0; i < 4; i++) {
        float voltage = testVoltages[i];
        float Rth = resistor * ((vref - voltage) / voltage);
        
        Serial.print("Tension: ");
        Serial.print(voltage, 2);
        Serial.print(" V → Rth: ");
        Serial.print(Rth, 0);
        Serial.println(" Ω");
        
        // La résistance doit être positive et raisonnable
        if (Rth < 100 || Rth > 200000) {
            allValid = false;
        }
    }
    
    printTestResult(allValid);
}

// ========================================
// TEST 6: Test interactif (chauffer la NTC)
// ========================================
void test_temperature_change() {
    printTestHeader("Test interactif - Chauffer la NTC");
    
    Serial.println("📌 Instructions:");
    Serial.println("   1. Note la température initiale");
    Serial.println("   2. Touche/chauffe la NTC pendant 5s");
    Serial.println("   3. Observe l'augmentation");
    Serial.println();
    
    // Température initiale
    float tempInitial = tempSensor->readTemperature();
    Serial.print("Température initiale: ");
    Serial.print(tempInitial, 1);
    Serial.println(" °C");
    
    Serial.println("\n👆 CHAUFFE la NTC maintenant...");
    delay(5000);
    
    // Température après chauffage
    float tempFinal = tempSensor->readTemperature();
    Serial.print("Température finale: ");
    Serial.print(tempFinal, 1);
    Serial.println(" °C");
    
    float difference = tempFinal - tempInitial;
    Serial.print("Différence: ");
    Serial.print(difference, 1);
    Serial.println(" °C");
    
    // La température doit avoir augmenté d'au moins 1°C
    bool passed = (difference > 1.0);
    
    if (!passed) {
        Serial.println("⚠️ Température n'a pas assez augmenté");
        Serial.println("   Assure-toi de bien chauffer la NTC!");
    }
    
    printTestResult(passed);
}

// ========================================
// SETUP - INITIALISATION
// ========================================
void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║   TESTS UNITAIRES - TemperatureControl ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    // Initialise le capteur
    Serial.println("\n🔧 Initialisation du capteur...");
    tempSensor = new TemperatureControl(36);  // Pin GPIO36
    tempSensor->begin();
    delay(500);
    Serial.println("✓ Capteur initialisé");
    
    // Lance tous les tests
    test_raw_adc();
    test_adc_to_voltage();
    test_resistance_calculation();
    test_temperature_ambient();
    test_temperature_stability();
    test_temperature_change();
    
    // Résumé final
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║          RÉSUMÉ DES TESTS              ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.print("Tests réussis: ");
    Serial.println(testsPassed);
    Serial.print("Tests échoués: ");
    Serial.println(testsFailed);
    Serial.print("Total: ");
    Serial.println(testsPassed + testsFailed);
    
    if (testsFailed == 0) {
        Serial.println("\n🎉 TOUS LES TESTS SONT RÉUSSIS ! 🎉");
    } else {
        Serial.println("\n⚠️ Certains tests ont échoué");
    }
    
    Serial.println("\n✓ Tests terminés");
}

void loop() {
    // Rien dans loop - Les tests s'exécutent dans setup()
}
