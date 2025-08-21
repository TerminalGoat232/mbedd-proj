#include <Arduino.h> 
#include <Adafruit_NeoPixel.h> 
#include <ProcessHID.h>
#include <RW_HIDC.h>

#define NEOPIXL_PIN 16
#define NEOPIXL_BRIGHTNESS 100

#define BUTTON_0_PIN 10
#define BUTTON_1_PIN 11
#define BUTTON_2_PIN 12

#define POLLING_INTV 1

#ifndef BTT_PIN_QUANTITY
  #define BTT_PIN_QUANTITY 3
#endif

Adafruit_NeoPixel pixl(1, NEOPIXL_PIN, NEO_GRB + NEO_KHZ800);
Process_HID proc;
RW_HIDC read_write_hidc; 

uint8_t button_pins[BTT_PIN_QUANTITY] {
  BUTTON_0_PIN,
  BUTTON_1_PIN,
  BUTTON_2_PIN 
};

uint8_t const desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD()
};

// default HID codes 
uint8_t hid_codes[BTT_PIN_QUANTITY] = {
  HID_KEY_W, 
  HID_KEY_ARROW_UP, 
  HID_KEY_Z
};

void setup() {
  Serial.begin(115200);
  LittleFS.begin();
  
  read_write_hidc.LoadHIDKeys(hid_codes); 

  if (!TinyUSBDevice.isInitialized()) TinyUSBDevice.begin(0);

  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(POLLING_INTV);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  

  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  } 

  for (uint8_t button: button_pins) pinMode(button, INPUT_PULLDOWN);

  // yaoi 
  pinMode(NEOPIXL_PIN, OUTPUT);
  pixl.begin();
  
}
 
void reinbau(){
  static uint16_t H = 0;
    H+=256;
    pixl.rainbow(H,1,255,NEOPIXL_BRIGHTNESS);
    pixl.show();
    if (H >= 1<<16) H = 0;
}

void loop(){

   static unsigned int ms_cnt = 0; 
   if (millis() - ms_cnt > 20) {
      reinbau();     
      ms_cnt = millis();
   }

   while (Serial.available() >= BTT_PIN_QUANTITY) {
      for (uint8_t k = 0; k < BTT_PIN_QUANTITY; k++) {
        hid_codes[k] = Serial.read(); 
      }
      read_write_hidc.SaveHIDKeys(hid_codes);

    }
   
   #ifdef TINYUSB_NEED_POLLING_TASK
      TinyUSBDevice.task();
   #endif
   
   proc.setGPIO(button_pins);
   proc.setHIDCode(hid_codes);
   proc.setPollInterval(POLLING_INTV);
   proc.run();
}
