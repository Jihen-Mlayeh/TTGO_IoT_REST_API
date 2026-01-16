// test_LED.ino
// Tests unitaires pour LedControl

#include "LedControl.h"
#include "config.h"

LedControl* led;
int testsPassed = 0;
int testsFailed = 0;

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
// TEST 1: État initial (LED éteinte)
// ========================================
void test_initial_state() {
    printTestHeader("État initial (doit être OFF)");
    
    bool state = led->getState();
    
    Serial.print("État de la LED: ");
    Serial.println(state ? "ON" : "OFF");
    
    bool passed = (state == false);
    
    if (!passed) {
        Serial.println("⚠️ La LED devrait être éteinte au démarrage");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 2: Allumage
// ========================================
void test_turn_on() {
    printTestHeader("Allumage de la LED");
    
    led->on();
    delay(500);
    
    bool state = led->getState();
    
    Serial.print("État après on(): ");
    Serial.println(state ? "ON" : "OFF");
    
    bool passed = (state == true);
    
    if (!passed) {
        Serial.println("⚠️ La LED devrait être allumée");
    } else {
        Serial.println("💡 Vérifie visuellement que la LED est allumée");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 3: Extinction
// ========================================
void test_turn_off() {
    printTestHeader("Extinction de la LED");
    
    led->off();
    delay(500);
    
    bool state = led->getState();
    
    Serial.print("État après off(): ");
    Serial.println(state ? "ON" : "OFF");
    
    bool passed = (state == false);
    
    if (!passed) {
        Serial.println("⚠️ La LED devrait être éteinte");
    } else {
        Serial.println("⚫ Vérifie visuellement que la LED est éteinte");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 4: Toggle (basculement)
// ========================================
void test_toggle() {
    printTestHeader("Fonction Toggle");
    
    // État initial
    led->off();
    bool stateInitial = led->getState();
    
    // Premier toggle (OFF → ON)
    led->toggle();
    delay(300);
    bool state1 = led->getState();
    
    // Deuxième toggle (ON → OFF)
    led->toggle();
    delay(300);
    bool state2 = led->getState();
    
    Serial.print("État initial: ");
    Serial.println(stateInitial ? "ON" : "OFF");
    Serial.print("Après 1er toggle: ");
    Serial.println(state1 ? "ON" : "OFF");
    Serial.print("Après 2ème toggle: ");
    Serial.println(state2 ? "ON" : "OFF");
    
    bool passed = (stateInitial == false && state1 == true && state2 == false);
    
    if (!passed) {
        Serial.println("⚠️ Le toggle ne fonctionne pas correctement");
    } else {
        Serial.println("💡 La LED a basculé correctement");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 5: Clignotement rapide
// ========================================
void test_blink() {
    printTestHeader("Clignotement (10 fois)");
    
    Serial.println("💡 La LED va clignoter 10 fois...");
    
    int blinkCount = 10;
    for (int i = 0; i < blinkCount; i++) {
        led->on();
        delay(100);
        led->off();
        delay(100);
        
        Serial.print("Clignotement ");
        Serial.print(i + 1);
        Serial.print("/");
        Serial.println(blinkCount);
    }
    
    bool passed = (led->getState() == false);
    
    Serial.println("✓ Test de clignotement terminé");
    printTestResult(passed);
}

// ========================================
// TEST 6: Séquence ON/OFF répétée
// ========================================
void test_repeated_on_off() {
    printTestHeader("Séquence ON/OFF répétée (5x)");
    
    bool allCorrect = true;
    
    for (int i = 0; i < 5; i++) {
        led->on();
        bool stateOn = led->getState();
        
        led->off();
        bool stateOff = led->getState();
        
        if (stateOn != true || stateOff != false) {
            allCorrect = false;
        }
        
        Serial.print("Cycle ");
        Serial.print(i + 1);
        Serial.print(": ON=");
        Serial.print(stateOn ? "OK" : "FAIL");
        Serial.print(", OFF=");
        Serial.println(stateOff ? "FAIL" : "OK");
        
        delay(200);
    }
    
    printTestResult(allCorrect);
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║      TESTS UNITAIRES - LedControl      ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    Serial.println("\n🔧 Initialisation de la LED...");
    led = new LedControl(32);  // Pin GPIO32
    led->begin();
    delay(500);
    Serial.println("✓ LED initialisée");
    
    test_initial_state();
    test_turn_on();
    test_turn_off();
    test_toggle();
    test_blink();
    test_repeated_on_off();
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║          RÉSUMÉ DES TESTS              ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.print("Tests réussis: ");
    Serial.println(testsPassed);
    Serial.print("Tests échoués: ");
    Serial.println(testsFailed);
    
    if (testsFailed == 0) {
        Serial.println("\n🎉 TOUS LES TESTS SONT RÉUSSIS ! 🎉");
    } else {
        Serial.println("\n⚠️ Certains tests ont échoué");
    }
    
    Serial.println("\n✓ Tests terminés - LED éteinte");
    led->off();
}

void loop() {
    // Rien dans loop - Les tests s'exécutent dans setup()
}
