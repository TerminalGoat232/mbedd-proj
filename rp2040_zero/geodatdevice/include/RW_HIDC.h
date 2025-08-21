#include <LittleFS.h>
#include <ArduinoJson.h>
#define CFG_FILE "/config.json"

class RW_HIDC {
  public:
    bool begin() {
      return LittleFS.begin();
    }
    void SaveHIDKeys(uint8_t *hidc_addr_2_save) {
      File f2w = LittleFS.open(CFG_FILE, "w");
      if (!f2w) return; 

      JsonDocument config_json_w; 
      config_json_w["K_0"] = hidc_addr_2_save[0];
      config_json_w["K_1"] = hidc_addr_2_save[1];
      config_json_w["K_2"] = hidc_addr_2_save[2];

      serializeJsonPretty(config_json_w, f2w);
      f2w.close();
    }

    void LoadHIDKeys(uint8_t *hidc_addr_2_load) {
      if (!LittleFS.exists(CFG_FILE)) return;
      File f2r = LittleFS.open(CFG_FILE, "r");
      if (!f2r) return;

      JsonDocument config_json_r;
      deserializeJson(config_json_r, f2r); 

      *(hidc_addr_2_load    ) = config_json_r["K_0"];
      *(hidc_addr_2_load + 1) = config_json_r["K_1"];
      *(hidc_addr_2_load + 2) = config_json_r["K_2"]; 
      f2r.close();
    }
    
};
