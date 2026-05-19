#include <FastLED.h>

// Hardware-Pins
#define LED_PIN      5
#define BUTTON_PIN   2    
#define STATUS_LED   13    // On-Board LED des Nano nur zu Testzwecken, zeigt an, ob der Modus aktiv ist (LED an) oder aus (LED aus)
#define NUM_LEDS     70    // 76
#define BRIGHTNESS   30    // Etwas höher zum Testen

CRGB leds[NUM_LEDS];

// Prototypen
void nextMode();
void modeOff();
void modeLichtbogen();
void modeKerze();
void modeParty();
void modeOrange();

volatile uint8_t mode = 0; 
const uint8_t maxModes = 5; 

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; 

void setup() {
    Serial.begin(9600); // Startet die serielle Kommunikation

    delay(1000);

    FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    
    pinMode(BUTTON_PIN, INPUT_PULLUP); 
    pinMode(STATUS_LED, OUTPUT); // On-Board LED als Ausgang
    
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), nextMode, FALLING);
    
    // Testlauf beim Start: Einmal kurz alle LEDs auf Weiß
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(500);
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void loop() {
    // Variable, um den vorherigen Modus zu speichern
    static uint8_t lastMode = 255; 

    // Wenn sich der Modus geändert hat, gib ihn im Seriellen Monitor aus
    if (mode != lastMode) {
        Serial.print("--- MODUS WECHSEL --- Aktueller Modus: ");
        switch(mode) {
            case 0: Serial.println("0 (AUS)"); break;
            case 1: Serial.println("1 (Lichtbogen)"); break;
            case 2: Serial.println("2 (Kerze)"); break;
            case 3: Serial.println("3 (Party)"); break;
            case 4: Serial.println("4 (Orange)"); break;
        }
        lastMode = mode; // Aktualisieren, damit es nur einmal gedruckt wird
    }

    // Die Status-LED leuchtet nur, wenn wir nicht im Modus 0 (Aus) sind
    digitalWrite(STATUS_LED, (mode > 0) ? HIGH : LOW);

    switch (mode) {
        case 0: modeOff(); break;
        case 1: modeLichtbogen(); break;
        case 2: modeKerze(); break;
        case 3: modeParty(); break;
        case 4: modeOrange(); break;
    }
}

void modeOff() {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
}

void modeLichtbogen() {
    static uint8_t pos = 0;
    fadeToBlackBy(leds, NUM_LEDS, 50); 
    leds[pos] = CRGB::Cyan;
    pos = (pos + 1) % NUM_LEDS;
    FastLED.show();
    delay(100); 
}

void modeKerze() {
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV(25, 255, random8(150, 255)); 
    }
    FastLED.show();
    delay(random8(50, 150));
}

void modeParty() {
    fill_rainbow(leds, NUM_LEDS, millis() / 5, 10);
    FastLED.show();
    delay(20);
}

void modeOrange() {
    fill_solid(leds, NUM_LEDS, CRGB(255, 156, 0));
    FastLED.show();
}

void nextMode() {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        mode++;
        if (mode > maxModes) mode = 0;
        lastDebounceTime = currentTime;
    }
}