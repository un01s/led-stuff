/**
 *  testing code for sf (65)
 *  adapted from ldirko
 *  https://github.com/ldirko/Fibonacci-64-nano-demoreel/
 *  https://github.com/jasoncoon/fibonacci64-touch-demo 
 *  
 *  2023-06-25
 *  
 *  * A1 is the LED pin.
 *  * no touch 
 *  
 */

#include <FastLED.h>

#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    65 // 260 = 65*4
#define DATA_PIN    A1 // D1

#define BRIGHTNESS  64
#define MAX_POWER_MILLIAMPS 500
#define MAX_BRIGHTNESS  128

CRGB leds[NUM_LEDS+1];

#define UPDATES_PER_SECOND 100

#define NUM_COLS_PLANAR 9            // resolution of planar lookup table
#define NUM_ROWS_PLANAR 9            // resolution of planar lookup table
#define NUM_LEDS_PLANAR NUM_COLS_PLANAR*NUM_ROWS_PLANAR

#define NUM_COLS_CILINDR 28           // resolution of cilindrical lookup table
#define NUM_ROWS_CILINDR 7            // resolution of cinindrical lookup table
#define NUM_LEDS_CILINDR NUM_COLS_CILINDR*NUM_ROWS_CILINDR

byte lastSafeIndex = NUM_LEDS;
byte start = 1;                       // play start animation 
byte InitNeeded = 1;
byte automodeOn = 255;                //state of autochange pattern
uint16_t automodeOndelay = 15000;     //this delay for automode on if left\right buttons not pressed 
byte brigtness = 64;

uint8_t gCurrentPatternNumber = 0;    // Index number of which pattern is current

byte rain [(NUM_COLS_PLANAR+2)*(NUM_ROWS_PLANAR+2)];           //need for Digital Rain and Fire Comets effects

#include "palletes.h"
#include "tables.h"
#include "patterns.h"

void setup() {

  Serial.begin(115200);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS)
  .setCorrection( TypicalLEDStrip );
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, MAX_POWER_MILLIAMPS);   
  FastLED.setBrightness(brigtness);
  FastLED.clear();
  
  random16_set_seed(123);
  gTargetPalette = ( gGradientPalettes[random8(gGradientPaletteCount)]);       // shoose random pallete on start
} 

void loop() {
  
  if (start) {StartAnimation (); start = 0;}  
  
  random16_add_entropy(random());

  EVERY_N_SECONDS( SECONDS_PER_PALETTE ) {   //random change palettes
    gCurrentPaletteNumber = random8 (gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];
  }

  EVERY_N_MILLISECONDS(40) {   //blend current palette to next palette
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 16);
  }

  EVERY_N_SECONDS( 15 ) {  // speed of change patterns periodically
    if (automodeOn) {
      FadeOut (150);        // fade out current effect
      gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE(gPatterns); //next effect
      InitNeeded=1; //flag if init something need
      FadeIn (200);        // fade in current effect
    }
  }
   
  gPatterns[gCurrentPatternNumber](); //play current pattern
  FastLED.show();
  delay(4);        //some time fast call rapidly FastLED.show() on esp32 causes flicker, this delay() is easy way to fix this 
}


void StartAnimation (){
  for (int i=0; i<1150; i++) {
    StartFibo();
    FastLED.show();
    delay(4);  
  }
}

void FadeOut (byte steps){
  for (int i=0; i<=steps; i++) {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8 (brigtness, 0, 255*i/steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show(); 
    delay(10);
  
  }
}

void FadeIn (byte steps){
  for (int i=steps+1; i--; i>=0) {
    gPatterns[gCurrentPatternNumber]();
    byte fadeOut = lerp8by8 (brigtness, 0, 255*i/steps);
    FastLED.setBrightness(fadeOut);
    FastLED.show(); 
    delay(10);

  }
}
