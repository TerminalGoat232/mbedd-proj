#include <Arduino.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <WiFi.h>

#define BUTTONS_PIN 17
#define O_LED_PIN 10 
#define Y_LED_PIN 11 
#define B_LED_PIN 12
#define G_LED_PIN 13 

#define reinbau_ms 10

#define USB_PRODUCT_NAME "Saygex Device, very col indeed"
#define USB_MANUFACTURER_NAME "TerminalGoat"

USBHIDKeyboard Keyboard;

const uint8_t LED_pins[] = {
  O_LED_PIN,
  Y_LED_PIN,
  G_LED_PIN,
  B_LED_PIN
};



// These values are resulted from analogRead in which 
// each value is defined by a specific corresponding resistor:
static const uint8_t resi_vals[]= {
  33, // 0, Vcc = 3.3V
  31, // 100K 
  24, // 100K + 110 
  11  // 100K + 220
};

char HID_code[] = {
  KEY_UP_ARROW, 
  'w',
  'z',
  KEY_ESC
};

// void Reinbau() {
//   for (long H=0; H < 5*16<<16; H+=256) {
//     pixels.rainbow(H,1,255,NEOPXL_BRIGHTNESS);
//     pixels.show();
//     delay(reinbau_ms); 
//   }
// }


void setup() {
  Serial.begin(115200);
  Keyboard.begin();

  USB.PID(0xC0F1);
  USB.productName(USB_PRODUCT_NAME);
  USB.manufacturerName(USB_MANUFACTURER_NAME);
  USB.begin();
  
  for (int x: LED_pins) pinMode(x, OUTPUT); 
}

void loop() {
  int rd = analogRead(BUTTONS_PIN);
  int V = static_cast<int>(rd*(3.3 / 4095)*10); 
  for (
       uint8_t t = 0; 
       t < sizeof(HID_code)/sizeof(uint8_t);
       t++ 
      ) if (V == resi_vals[t])  { 
            digitalWrite(LED_pins[3], HIGH);
            Keyboard.press(HID_code[t]);
      }
  if (!V) for (int x: HID_code ) {
    Keyboard.releaseAll();
    digitalWrite(LED_pins[3], LOW);
  }
}
