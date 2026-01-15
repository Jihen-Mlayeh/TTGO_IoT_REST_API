// test_Photocell.ino
// Tests unitaires pour PhotocellControl

#include "PhotocellControl.h"
#include "config.h"

PhotocellControl* photocell;
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
// TEST 1: Lecture ADC brute
// ========================================
void test_raw_reading() {
    printTestHeader("Lecture ADC brute");
    
    int rawValue = photocell->readValue();
    
    Serial.print("Valeur ADC: ");
    Serial.println(rawValue);
    
    bool passed = (rawValue >= 0 && rawValue <= 4095);
    
    if (!passed) {
        Serial.println("⚠️ Valeur ADC invalide");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 2: Conversion en pourcentage
// ========================================
void test_percentage_conversion() {
    printTestHeader("Conversion en pourcentage");
    
    int percent = photocell->readPercent();
    
    Serial.print("Luminosité: ");
    Serial.print(percent);
    Serial.println(" %");
    
    bool passed = (percent >= 0 && percent <= 100);
    
    if (!passed) {
        Serial.println("⚠️ Pourcentage hors limites");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 3: Stabilité des lectures
// ========================================
void test_stability() {
    printTestHeader("Stabilité (10 lectures)");
    
    int readings[10];
    long sum = 0;
    
    for (int i = 0; i < 10; i++) {
        readings[i] = photocell->readPercent();
        sum += readings[i];
        delay(100);
    }
    
    int average = sum / 10;
    
    int maxDeviation = 0;
    for (int i = 0; i < 10; i++) {
        int deviation = abs(readings[i] - average);
        if (deviation > maxDeviation) {
            maxDeviation = deviation;
        }
    }
    
    Serial.print("Moyenne: ");
    Serial.print(average);
    Serial.println(" %");
    Serial.print("Écart max: ");
    Serial.print(maxDeviation);
    Serial.println(" %");
    
    bool passed = (maxDeviation < 10);
    
    if (!passed) {
        Serial.println("⚠️ Écart trop important (>10%)");
    }
    
    printTestResult(passed);
}

// ========================================
// TEST 4: Test interactif (cache la photorésistance)
// ========================================
void test_light_change() {
    printTestHeader("Test interactif - Variation lumière");
    
    Serial.println("📌 Instructions:");
    Serial.println("   1. Note la luminosité initiale");
    Serial.println("   2. Cache la photorésistance pendant 3s");
    Serial.println("   3. Observe la diminution");
    Serial.println();
    
    int lightInitial = photocell->readPercent();
    Serial.print("Luminosité initiale: ");
    Serial.print(lightInitial);
    Serial.println(" %");
    
    Serial.println("\n✋ CACHE la photorésistance maintenant...");
    delay(3000);
    
    int lightFinal = photocell->readPercent();
    Serial.print("Luminosité finale: ");
    Serial.print(lightFinal);
    Serial.println(" %");
    
    int difference = lightInitial - lightFinal;
    Serial.print("Différence: ");
    Serial.print(difference);
    Serial.println(" %");
    
    bool passed = (difference > 5);
    
    if (!passed) {
        Serial.println("⚠️ Pas assez de variation détectée");
        Serial.println("   Cache bien la photorésistance!");
    }
    
    printTestResult(passed);
}

void setup() {
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n\n");
    Serial.println("╔════════════════════════════════════════╗");
    Serial.println("║   TESTS UNITAIRES - PhotocellControl   ║");
    Serial.println("╚════════════════════════════════════════╝");
    
    Serial.println("\n🔧 Initialisation du capteur...");
    photocell = new PhotocellControl(33);  // Pin GPIO33
    photocell->begin();
    delay(500);
    Serial.println("✓ Capteur initialisé");
    
    test_raw_reading();
    test_percentage_conversion();
    test_stability();
    test_light_change();
    
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
}

void loop() {
    // Rien dans loop - Les tests s'exécutent dans setup()
}
