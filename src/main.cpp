#include <FastLED.h>

// Hardware-Pins
#define LED_PIN      5
#define BUTTON_PIN   2    
#define STATUS_LED   13    // On-Board LED des Nano nur zu Testzwecken, zeigt an, ob der Modus aktiv ist (LED an) oder aus (LED aus)
#define NUM_LEDS     76
#define BRIGHTNESS   64    // Etwas höher zum Testen

CRGB leds[NUM_LEDS];

// Prototypen
void nextMode();
void modeLichtbogen();
void modeKerze();
void modeParty();

volatile uint8_t mode = 0; 
const uint8_t maxModes = 3; 

volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200; 

void setup() {
    // Kurze Sicherheitspause
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
    // Die Status-LED leuchtet nur, wenn wir nicht im Modus 0 (Aus) sind
    digitalWrite(STATUS_LED, (mode > 0) ? HIGH : LOW);

    switch (mode) {
        case 0: 
            fill_solid(leds, NUM_LEDS, CRGB(255, 200, 150));
            FastLED.show();
            break;
        case 1: modeLichtbogen(); break;
        case 2: modeKerze(); break;
        case 3: modeParty(); break;
    }
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

void nextMode() {
    unsigned long currentTime = millis();
    if ((currentTime - lastDebounceTime) > debounceDelay) {
        mode++;
        if (mode > maxModes) mode = 0;
        lastDebounceTime = currentTime;
    }
}