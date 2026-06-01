#include <FastLED.h>

// Hardware-Pins
#define LED_PIN      5
#define BUTTON_PIN   2    
#define STATUS_LED   13    // On-Board LED des Nano nur zu Testzwecken, zeigt an, ob der Modus aktiv ist (LED an) oder aus (LED aus)
#define NUM_LEDS     53    
#define BRIGHTNESS   100    // Prozentwert 1-100, niedrige Helligkeit empfiehlt sich bei hohem Stromverbrauch

// LED-Streifen: SL-5050-144HC2812B, 5V, 144 LEDs/M, RGB

CRGB leds[NUM_LEDS];

// Prototypen
void nextMode();
void modeOff();
void modeLauflicht();
void modeKerze();
void modeParty();
void modeOrange();
void modeWhite();
void modeRed();
void modeGreen();
void modeBlue();
void voidCentral();
void setBrightness(uint8_t percent);
void handleSerialCommand();

// LED-Typ
#define LED_TYPE     WS2812B // WS2812B
#define COLOR_ORDER  GRB

volatile uint8_t mode = 0; 
const uint8_t maxModes = 9; 

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; 

void setup() {
    Serial.begin(9600); // Startet die serielle Kommunikation

    delay(1000);
               
    // LED-Strip initialisieren
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    // FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

    // FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000); // Schutz vor zu hohem Strom bei 5V-2812B-Streifen
    setBrightness(BRIGHTNESS);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    pinMode(STATUS_LED, OUTPUT); // On-Board LED als Ausgang
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), nextMode, FALLING);
    }

void loop() {
    // Variable, um den vorherigen Modus zu speichern
    static uint8_t lastMode = 255; 

    handleSerialCommand();

    // Wenn sich der Modus geändert hat, gib ihn im Seriellen Monitor aus
    if (mode != lastMode) {
        Serial.print("--- MODUS WECHSEL --- Aktueller Modus: ");
        switch(mode) {
            case 0: Serial.println("0 (AUS)"); break;
            case 1: Serial.println("1 (Lichtbogen)"); break;
            case 2: Serial.println("2 (Kerze)"); break;
            case 3: Serial.println("3 (Party)"); break;
            case 4: Serial.println("4 (Orange)"); break;
            case 5: Serial.println("5 (White)"); break;
            case 6: Serial.println("6 (Red)"); break;
            case 7: Serial.println("7 (Green)"); break;
            case 8: Serial.println("8 (Blue)"); break;
            case 9: Serial.println("9 (voidCentral)"); break;

        }
        lastMode = mode; // Aktualisieren, damit es nur einmal gedruckt wird
    }

    // Die Status-LED leuchtet nur, wenn wir nicht im Modus 0 (Aus) sind
    digitalWrite(STATUS_LED, (mode > 0) ? HIGH : LOW);

    switch (mode) {
        case 0: modeOff(); FastLED.clear(); break;
        case 1: modeLauflicht(); FastLED.clear(); break;
        case 2: modeKerze(); FastLED.clear(); break;
        case 3: modeParty(); FastLED.clear(); break;
        case 4: modeOrange(); FastLED.clear(); break;
        case 5: modeWhite(); FastLED.clear();break;
        case 6: modeRed(); FastLED.clear();break;
        case 7: modeGreen(); FastLED.clear();break;
        case 8: modeBlue(); FastLED.clear();break;
        case 9: voidCentral(); FastLED.clear();break;

    }


}

void modeOff() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void modeLauflicht() {
    static uint8_t pos = 0;
    fadeToBlackBy(leds, NUM_LEDS, 50); 
    leds[pos] = CRGB::Cyan;
    pos = (pos + 1) % NUM_LEDS;
    FastLED.show();
    delay(100); 
}

void modeKerze() {
    for (int i = 0; i < NUM_LEDS; i++) {
        // Farbwert (Hue) leicht variieren lassen zwischen ca. 12 (warmes Orange) und 22 (Gelb-Orange)
        uint8_t farbwert = random8(12, 22); 
        
        // Sättigung (Saturation) hoch halten für kräftige Farben (z.B. 240 - 255)
        uint8_t saettigung = random8(240, 255); 
        
        // Helligkeit (Value) flackern lassen
        uint8_t helligkeit = random8(130, 255); 

        leds[i] = CHSV(farbwert, saettigung, helligkeit); 
    }
    FastLED.show();
    delay(random8(40, 120)); // Zufällige Pause für ein organischeres Flackern
}

void modeParty() {
    fill_rainbow(leds, NUM_LEDS, millis() / 5, 10);
    FastLED.show();
    delay(20);
}

void modeOrange() {
    fill_solid(leds, NUM_LEDS, CRGB(255, 100, 0)); // Orange "CRGB(255, 156, 0)"
    FastLED.show();
}

void modeWhite(){
    fill_solid(leds, NUM_LEDS, CRGB(255, 255, 255)); // white full power
    FastLED.show();    
}

void modeRed(){
    fill_solid(leds, NUM_LEDS, CRGB(255, 0, 0)); // white full power
    FastLED.show();    
}

void modeGreen(){
    fill_solid(leds, NUM_LEDS, CRGB(0, 255, 0)); // white full power
    FastLED.show();    
}

void modeBlue(){
    fill_solid(leds, NUM_LEDS, CRGB(0, 0, 255)); // white full power
    FastLED.show();    
}

void voidCentral() {
    // 1. Definition der Streifen-Eigenschaften
    const int len[] = {25, 19, 9}; 
    const int numStrips = 3;

    int startIdx[numStrips];
    startIdx[0] = 0;
    startIdx[1] = len[0];
    startIdx[2] = len[0] + len[1];

    int midIdx[numStrips];
    int maxSteps[numStrips];

    for (int i = 0; i < numStrips; i++) {
        midIdx[i] = len[i] / 2;          
        maxSteps[i] = len[i] - midIdx[i]; 
    }

    int totalMaxSteps = 0;
    for (int i = 0; i < numStrips; i++) {
        if (maxSteps[i] > totalMaxSteps) {
            totalMaxSteps = maxSteps[i];
        }
    }

    // ==========================================
    // SCHLEIFE 1: Nach AUSSEN laufen (Anschalten)
    // ==========================================
    for (int step = 0; step < totalMaxSteps; step++) {
        for (int i = 0; i < numStrips; i++) {
            if (step < maxSteps[i]) {
                int leftLED  = startIdx[i] + midIdx[i] - step;
                int rightLED = startIdx[i] + midIdx[i] + step;

                leds[leftLED]  = CRGB::White;
                leds[rightLED] = CRGB::White;
            }
        }
        FastLED.show();
        delay(200); 
    }

    // ==========================================
    // SCHLEIFE 2: Zur MITTE zurücklaufen (Ausschalten)
    // ==========================================
    // Wir starten bei dem letzten Schritt (totalMaxSteps - 1) und zählen rückwärts bis 0
    for (int step = totalMaxSteps - 1; step >= 0; step--) {
        for (int i = 0; i < numStrips; i++) {
            if (step < maxSteps[i]) {
                int leftLED  = startIdx[i] + midIdx[i] - step;
                int rightLED = startIdx[i] + midIdx[i] + step;

                // LEDs wieder ausschalten (auf Schwarz setzen)
                leds[leftLED]  = CRGB::Black;
                leds[rightLED] = CRGB::Black;
            }
        }
        FastLED.show();
        delay(200); // Geschwindigkeit für den Rückweg
    }
}


void setBrightness(uint8_t percent) {
    if (percent < 1) percent = 1;
    if (percent > 100) percent = 100;

    uint8_t fastledBrightness;
    if (percent >= 100) {
        fastledBrightness = 255;
    } else {
        fastledBrightness = (percent * 254 + 50) / 100;
        if (fastledBrightness < 1) {
            fastledBrightness = 1;
        }
    }

    FastLED.setBrightness(fastledBrightness);
    Serial.print("Helligkeit: ");
    Serial.print(percent);
    Serial.println(" %");
}

void handleSerialCommand() {
    if (!Serial.available()) {
        return;
    }

    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() == 0) {
        return;
    }

    input.toLowerCase();
    const String prefix = "set brightness ";
    if (input.startsWith(prefix)) {
        String valueString = input.substring(prefix.length());
        valueString.trim();
        int value = valueString.toInt();
        if (value >= 1 && value <= 100) {
            setBrightness((uint8_t)value);
        } else {
            Serial.print("Ungültiger Helligkeitswert: ");
            Serial.println(valueString);
            Serial.println("Benutze: set brightness 1..100");
        }
    } else {
        Serial.print("Unbekannter Befehl: ");
        Serial.println(input);
        Serial.println("Erwarte: set brightness x");
    }
}

void nextMode() {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        mode++;
        if (mode > maxModes) mode = 0;
        lastDebounceTime = currentTime;
    }
}