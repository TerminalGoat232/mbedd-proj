#include <Arduino.h>
#include <string.h>
// LEDs
#define B_LED_PIN 3
#define G_LED_PIN 4
#define R_LED_PIN 5
#define Y_LED_PIN 6

// buttons for input
#define REG_PIN_1 11
#define REG_PIN_2 10 
#define REG_PIN_3 9
#define REG_PIN_4 8

// speaker
#define SPEAKER_PIN 7

// verify button
#define VERIFY_BUTTON 12

// the amount of corresponding LEDs, BUTTONs
#define LED_N_REG_NUM  4
#define PLAYING_SIZE 100

uint8_t led_instr[PLAYING_SIZE]; 
uint8_t answering[PLAYING_SIZE];

int led_pins[LED_N_REG_NUM] = {B_LED_PIN, G_LED_PIN, R_LED_PIN, Y_LED_PIN};
int reg_pins[LED_N_REG_NUM] = {REG_PIN_1, REG_PIN_2, REG_PIN_3, REG_PIN_4};

bool b_states[LED_N_REG_NUM];
bool last_b_states[LED_N_REG_NUM];

int delay_ms = 300;

int verify_b_state = HIGH;
int last_verify_b_state = HIGH; 
int seq_running = 0;
int current_instr_length = 0;
int current_ans_length = 0;
int pressed_verify_button = 0;

//void (*resetBoard)(void) = 0;
// much safer way to soft-resetting the board instead of 
// pointer manipulating magic fuckery
void resetBoard(){
  asm volatile ("jmp 0");
}

void setup(){
  Serial.begin(9600);
  for (int pin = 0; pin < LED_N_REG_NUM; pin++ ) { 
    pinMode(led_pins[pin], OUTPUT);
    pinMode(reg_pins[pin], INPUT_PULLUP);
  }
  pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(VERIFY_BUTTON, INPUT_PULLUP);
  
  randomSeed(analogRead(0));
}

void blinkLED(int LED_PIN, int delay_ms){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SPEAKER_PIN, HIGH);

  delay(delay_ms);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(SPEAKER_PIN, LOW);

  delay(delay_ms);
  
}
void initLedSeq(uint8_t *led_seq, int length){
  for (int x = 0; x < length; x++) {
    blinkLED(led_seq[x], delay_ms);
  }  
  seq_running = 0;
}

void losingSeq(){
   for (int n = 0; n < LED_N_REG_NUM; n++) {
      digitalWrite(led_pins[n], HIGH);
      delay(100);
      digitalWrite(led_pins[n], LOW);
      delay(100);
    }
    digitalWrite(SPEAKER_PIN, LOW);
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
    if (pressed_verify_button > 1) checkIfLosing();
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
