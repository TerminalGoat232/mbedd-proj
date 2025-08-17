#include <Arduino.h> 
#include <Adafruit_NeoPixel.h> 
#include <ProcessHID.h>

#define NEOPIXL_PIN 16
#define NEOPIXL_BRIGHTNESS 100

#define BUTTON_0_PIN 10
#define BUTTON_1_PIN 11
#define BUTTON_2_PIN 12

#define POLLING_INTV 2

#ifndef BTT_PIN_QUANTITY
  #define BTT_PIN_QUANTITY 3
#endif

Adafruit_NeoPixel pixl(1, NEOPIXL_PIN, NEO_GRB + NEO_KHZ800);
Process_HID proc;

uint8_t button_pins[BTT_PIN_QUANTITY] {
  BUTTON_0_PIN,
  BUTTON_1_PIN,
  BUTTON_2_PIN 
};

uint8_t const desc_hid_report[] = {
      TUD_HID_REPORT_DESC_KEYBOARD()
};

// temporary HID key codes
uint8_t hid_codes[BTT_PIN_QUANTITY] = {
  HID_KEY_W, 
  HID_KEY_ARROW_UP, 
  HID_KEY_Z
};

void setup() {
  Serial.begin(115200);
  pinMode(NEOPIXL_PIN, OUTPUT);
  pixl.begin();
  
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
}

  
void reinbau(){
  for (long H = 0;H <= 5*16<<16;H+=256) {  
    pixl.rainbow(H,1,255,NEOPIXL_BRIGHTNESS);
    pixl.show();
    delay(20);
  }
}


void loop(){
  
 #ifdef TINYUSB_NEED_POLLING_TASK
    TinyUSBDevice.task()
 #endif
 proc.setGPIO(button_pins);
 proc.setHIDCode(hid_codes);
 proc.setPollInterval(POLLING_INTV);
 proc.run();
}
