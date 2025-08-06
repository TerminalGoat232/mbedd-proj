#include <Arduino.h>
#include <Esp.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>
#include <math.h>
// LEDs
#define B_LED_PIN 4
#define G_LED_PIN 5
#define R_LED_PIN 6
#define Y_LED_PIN 7

// buttons for input
#define REG_PIN_1 9
#define REG_PIN_2 10 
#define REG_PIN_3 11
#define REG_PIN_4 12

// speaker
#define SPEAKER_PIN 8

// verify button
#define VERIFY_BUTTON 18

// the amount of corresponding LEDs, BUTTONs
#define LED_N_REG_NUM  4
#define PLAYING_SIZE 100

#define BAUD_RATE 115200

#define UWU_RGB_ACTIVA_PIN 14
#define NEOPXL_BRIGHTNESS 100
Adafruit_NeoPixel pixels(1, 48, NEO_GRB + NEO_KHZ800);

uint8_t led_instr[PLAYING_SIZE]; 
uint8_t answering[PLAYING_SIZE];

uint8_t led_pins[LED_N_REG_NUM] = {B_LED_PIN, G_LED_PIN, R_LED_PIN, Y_LED_PIN};
uint8_t reg_pins[LED_N_REG_NUM] = {REG_PIN_1, REG_PIN_2, REG_PIN_3, REG_PIN_4};

bool b_states[LED_N_REG_NUM];
bool last_b_states[LED_N_REG_NUM];
bool seq_running = 0;

uint16_t delay_ms = 200;

uint8_t verify_b_state = HIGH;
uint8_t last_verify_b_state = HIGH; 
uint8_t current_instr_length = 0;
uint8_t current_ans_length = 0;
uint8_t pressed_verify_button = 0;

TaskHandle_t Thread1;

//void (*resetBoard)(void) = 0;
// much safer way to soft-resetting the board instead of 
// pointer manipulating magic fuckery
// use this for AVR atmel idk.
// void resetBoard(){
//   asm volatile ("jmp 0");
// }
void resetBoard(){
  ESP.restart();
}
void blinkLED(uint8_t LED_PIN, uint16_t delay_ms){
  
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SPEAKER_PIN, HIGH);

  delay(delay_ms);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(SPEAKER_PIN, LOW);

  delay(delay_ms);
  
}
void initLedSeq(uint8_t *led_seq, uint8_t length){
  for (int x = 0; x < length; x++) {
    blinkLED(led_seq[x], delay_ms);
  }  
  seq_running = 0;
}

void losingSeq(){
   digitalWrite(SPEAKER_PIN, LOW);
    for (int n = 0; n < LED_N_REG_NUM; n++) {
      digitalWrite(led_pins[n], HIGH);
      analogWrite(SPEAKER_PIN, 255-n*10);
      delay(100);
      digitalWrite(led_pins[n], LOW);
      analogWrite(SPEAKER_PIN, 0);
      delay(100);
    }    
}
void Lose() {
    losingSeq();
    // just in case
    memset(answering, 0, sizeof(answering));
    memset(led_instr, 0, sizeof(led_instr));
    pressed_verify_button = 0;
    current_instr_length = 0;
    
    resetBoard();
}
void checkIfLosing() {
  current_ans_length = 0;
  if (memcmp(led_instr, answering, sizeof(answering))) {
     Lose();
  }
  pressed_verify_button--;
  memset(answering, 0, sizeof(answering));
}
void idk_sleeping_lamp_ig(void *pvParameters) {
    for (;;) {
        uint8_t pressed_uwu_rgb_button = !digitalRead(UWU_RGB_ACTIVA_PIN);
        uint8_t last_pressed_uwu_rgb_button = HIGH;
        if (pressed_uwu_rgb_button == LOW && last_pressed_uwu_rgb_button == HIGH) {
              for (long H = 0;H <= 5*16<<16;H+=256) {
                pixels.rainbow(H,1,255,NEOPXL_BRIGHTNESS);
                pixels.show();
                delay(20);
              }
          last_pressed_uwu_rgb_button = pressed_uwu_rgb_button;
        }
        neopixelWrite(RGB_BUILTIN, 0, 50, 10);
    }
}

void setup(){
  Serial.begin(BAUD_RATE);
  pixels.begin();
  for (int pin = 0; pin < LED_N_REG_NUM; pin++ ) { 
    pinMode(led_pins[pin], OUTPUT);
    pinMode(reg_pins[pin], INPUT_PULLUP);
  }
  pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(VERIFY_BUTTON, INPUT_PULLUP);
  pinMode(UWU_RGB_ACTIVA_PIN, INPUT_PULLUP);
 
  //stupid ass me 
  xTaskCreatePinnedToCore(
    idk_sleeping_lamp_ig,
    "Sleeping Lamp",
    10000,
    NULL,
    1,
    &Thread1,
    1
  );

  randomSeed(analogRead(0));
}

void loop(){
    
  // verifying stage
  verify_b_state = digitalRead(VERIFY_BUTTON);
  
  if ( verify_b_state == LOW && last_verify_b_state == HIGH ) {
    ++pressed_verify_button;
    led_instr[current_instr_length] = led_pins[random(4)];
    ++current_instr_length;
    //Buffer overflow prevention
    if (current_instr_length > PLAYING_SIZE) {
      Serial.println("Wut der heil bro, btw GG!");
      Lose();
    }
    if (pressed_verify_button > 1)  checkIfLosing();
      
    if (!seq_running) {
      initLedSeq(led_instr, current_instr_length);
    }
  }
  last_verify_b_state = verify_b_state;
  

  // registering  
  if (pressed_verify_button) {
    for (int button = 0; button < LED_N_REG_NUM; button++) {
      b_states[button] = digitalRead(reg_pins[button]);
      if ( b_states[button] == LOW && last_b_states[button] == HIGH) {
        answering[current_ans_length] = led_pins[button]; 
        ++current_ans_length;

        digitalWrite(led_pins[button], HIGH);
        digitalWrite(SPEAKER_PIN, HIGH);
        delay(delay_ms);

        if (current_ans_length == current_instr_length) checkIfLosing();

      }
      digitalWrite(led_pins[button], LOW);
      digitalWrite(SPEAKER_PIN, LOW);
      last_b_states[button] = b_states[button];
    }
  }
}
