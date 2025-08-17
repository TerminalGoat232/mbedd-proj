#include <Adafruit_TinyUSB.h>

#define BTT_PIN_QUANTITY 3

Adafruit_USBD_HID usb_hid;


uint8_t _k_pins[BTT_PIN_QUANTITY];
uint8_t _hid_codes[BTT_PIN_QUANTITY];

class Process_HID {
  private:
    uint32_t delay_ms; 
    bool keyPressed = false;
    uint8_t btt_cpy_size = sizeof(uint8_t) * BTT_PIN_QUANTITY;

    void proc_hid(){
      if (!usb_hid.ready()) return;

      uint8_t cnt = 0; 
      uint8_t keycode[6] = {0};
      
      for (uint8_t n = 0; n < BTT_PIN_QUANTITY; n++) {
        if (!digitalRead(_k_pins[n])) continue; 
        keycode[cnt++] = _hid_codes[n];
        if (cnt == 6) break; 
      }

      if (TinyUSBDevice.suspended() && cnt) TinyUSBDevice.remoteWakeup();

      if (cnt) {
        keyPressed = true;
        usb_hid.keyboardReport(0, 0, keycode);
      } else if (keyPressed) {
        keyPressed = false;
        usb_hid.keyboardRelease(0);
      }
    }

  public:
    
    void setPollInterval(uint32_t _ms) {
      delay_ms = _ms;
    }
    
    void setGPIO(uint8_t *_pins) {
      memcpy(_k_pins, _pins, btt_cpy_size);
    }
    
    void setHIDCode(uint8_t *_hidc) {
      memcpy(_hid_codes, _hidc, btt_cpy_size);
    }

    void run(){
      static unsigned int ms_cnt = 0; 
      if (millis() - ms_cnt > delay_ms) {
        proc_hid();
        ms_cnt = millis();
    }

  }
};

