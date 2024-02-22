#include <Adafruit_NeoPixel.h>

int dReady                        = 2;
int dPIN1                         = 3;
int dPIN2                         = 4;
int dPIN3                         = 5;

int lPIN1                         = 6;
int lPIN2                         = 7;
int lPIN3                         = 8;
int BODY_PIXELS_COUNT             = 200;
int SHOOTER_PIXELS1_COUNT         = 10;
int SHOOTER_PIXELS2_COUNT         = 10;

Adafruit_NeoPixel BODY_PIXELS     = Adafruit_NeoPixel(BODY_PIXELS_COUNT, lPIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel SHOOTER_PIXELS1 = Adafruit_NeoPixel(SHOOTER_PIXELS1_COUNT, lPIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel SHOOTER_PIXELS2 = Adafruit_NeoPixel(SHOOTER_PIXELS2_COUNT, lPIN2, NEO_GRB + NEO_KHZ800);

uint32_t color                    = BODY_PIXELS.Color(0,0,0);
uint32_t last_error_color         = BODY_PIXELS.Color(0,0,0);
uint32_t rainbow_color_add        = 0;

int tPIN                          = 13;

enum StatusEnum { BOOTING, NORMAL, SHOOTING, AUTO, VIBE, ERROR };

StatusEnum status;

void setup() {
  status = 1;

  pinMode(dReady, INPUT);
  pinMode(dPIN1,  INPUT);
  pinMode(dPIN2,  INPUT);
  pinMode(dPIN3,  INPUT);
  
  pinMode(lPIN1,  OUTPUT);
  pinMode(lPIN2,  OUTPUT);
  pinMode(lPIN3,  OUTPUT);
  
  BODY_PIXELS.begin();
  SHOOTER_PIXELS1.begin();
  SHOOTER_PIXELS2.begin();
  
  pinMode(tPIN,   OUTPUT);
  Serial.print("INIT COMPLETE");
}

void loop() {
  status = getStatus();

  updateLED();
}

void updateLED() {
  switch ( status ) {
    case BOOTING:
      color = BODY_PIXELS.Color(0,0,0); // OFF
      break;
    case NORMAL:
      color = BODY_PIXELS.Color(0,255,0); // green
      break;
    case SHOOTING:
      color = BODY_PIXELS.Color(255,150,0); // ORANGE
      break;
    case AUTO:
      color = BODY_PIXELS.Color(0,255,215); // CYAN
      break;
    case VIBE:
      color = BODY_PIXELS.Color(0,0,255); // BLUE
      break;
    default:
      color = BODY_PIXELS.Color(255,0,0); // RED
      break;
  }
  writePixels();
}

StatusEnum getStatus() {
  if ((digitalRead(dPIN1) == LOW && digitalRead(dPIN2) == LOW) && digitalRead(dPIN3) == LOW) {
    return 0; // booting
  }
  if ((digitalRead(dPIN1) == LOW && digitalRead(dPIN2) == LOW) && digitalRead(dPIN3) == HIGH) {
    return 1; // Normal
  }
  if ((digitalRead(dPIN1) == LOW && digitalRead(dPIN2) == HIGH) && digitalRead(dPIN3) == LOW) {
    return 2; // Shooting
  }
  if ((digitalRead(dPIN1) == LOW && digitalRead(dPIN2) == HIGH) && digitalRead(dPIN3) == HIGH) {
    return 3; // auto
  }
  if ((digitalRead(dPIN1) == HIGH && digitalRead(dPIN2) == LOW) && digitalRead(dPIN3) == LOW) {
    return 4; // vibe
  }
  if ((digitalRead(dPIN1) == HIGH && digitalRead(dPIN2) == LOW) && digitalRead(dPIN3) == HIGH) {
    return 5; // error
  }

  return 5;
  
}

void writePixels() {
  switch ( status ) {
    case VIBE:
      rainbow();
      break;
    case ERROR:
      error();
      break;
    default:
      BODY_PIXELS.fill(color);
      SHOOTER_PIXELS1.fill(color);
      SHOOTER_PIXELS2.fill(color);
      BODY_PIXELS.show();
      SHOOTER_PIXELS1.show();
      SHOOTER_PIXELS2.show();
      break;
  } 
}


void rainbow() {
  // Hue is a circular value from 0 to 255. 0 and 255 are the same (red).
  // This function will cycle through the hue values to create a rainbow effect.
  uint16_t i, j;

  for (j = 0; j < 256; j++) { // One cycle of all 256 hues.
    for (i = 0; i < BODY_PIXELS.numPixels(); i++) {
      BODY_PIXELS.setPixelColor(i, Wheel((i + j) & 255));
    }
    BODY_PIXELS.show();
    delay(10); // Adjust for smoother or faster color transition
  }
}

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return BODY_PIXELS.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return BODY_PIXELS.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return BODY_PIXELS.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}


void error() {
  if (last_error_color == BODY_PIXELS.Color(0,0,0)) {
    last_error_color = BODY_PIXELS.Color(255,0,0);
    BODY_PIXELS.fill(last_error_color);
    SHOOTER_PIXELS1.fill(last_error_color);
    SHOOTER_PIXELS2.fill(last_error_color);
  } else {
    last_error_color = BODY_PIXELS.Color(0,0,0);
    BODY_PIXELS.fill(last_error_color);
    SHOOTER_PIXELS1.fill(last_error_color);
    SHOOTER_PIXELS2.fill(last_error_color);
  }
  
  BODY_PIXELS.show();
  SHOOTER_PIXELS1.show();
  SHOOTER_PIXELS2.show();
  if (digitalRead(dPIN1) != HIGH && digitalRead(dPIN2) != LOW && digitalRead(dPIN3) != HIGH) {
    delay(10);
  }
}