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

// debug button
#define TEST_VIC_BUTTON 13

// the amount of corresponding LEDs, BUTTONs
#define LED_N_REG_NUM  4

int led_instr[100]; 
int answering[100];

int led_pins[LED_N_REG_NUM] = {B_LED_PIN, G_LED_PIN, R_LED_PIN, Y_LED_PIN};
int reg_pins[LED_N_REG_NUM] = {REG_PIN_1, REG_PIN_2, REG_PIN_3, REG_PIN_4};

bool b_states[LED_N_REG_NUM];
bool last_b_states[LED_N_REG_NUM];

int delayness = 300;

int b_state = HIGH;
int last_b_state = HIGH; 
int seq_running = 0;
int current_instr_length = 0;
void (*resetBoard)(void) = 0;

void setup(){
  Serial.begin(9600);
  for (int pin = 0; pin < LED_N_REG_NUM; pin++ ) { 
    pinMode(led_pins[pin], OUTPUT);
    pinMode(reg_pins[pin], INPUT_PULLUP);
  }
  pinMode(SPEAKER_PIN, OUTPUT);

  pinMode(TEST_VIC_BUTTON, INPUT_PULLUP);
  
  randomSeed(analogRead(0));
}

void blinkLED(int LED_PIN, int delayness){

  digitalWrite(LED_PIN, HIGH);
  digitalWrite(SPEAKER_PIN, HIGH);

  delay(delayness);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(SPEAKER_PIN, LOW);

  delay(delayness);
  
}
void initLedSeq(int *led_seq, int length){
  for (int x = 0; x < length; x++) {
    blinkLED(led_seq[x], delayness);
  }
  Serial.println("done");
  seq_running = 0;
}

void loseSeq(){
   for (int n = 0; n <= LED_N_REG_NUM; n++) {
      digitalWrite(led_pins[n], HIGH);
      delay(100);
      digitalWrite(led_pins[n], LOW);
      delay(100);
    }
}



void loop(){
  //digitalWrite(led_pins[0], HIGH);
  for (int button = 0; button < LED_N_REG_NUM; button++){
    b_states[button] = digitalRead(reg_pins[button]);

    if (b_states[button] == LOW && last_b_states[button] == HIGH) {
      answering[current_instr_length-1] = led_pins[button];
      digitalWrite(led_pins[button], HIGH);
      digitalWrite(SPEAKER_PIN, HIGH);
      delay(delayness);
    }
    digitalWrite(led_pins[button], LOW);
    digitalWrite(SPEAKER_PIN, LOW);
    last_b_states[button] = b_states[button];
  }

  b_state = digitalRead(TEST_VIC_BUTTON);
  if ( b_state == LOW && last_b_state == HIGH) {

    for (int i = 0; i < current_instr_length; i++) {
      if ((answering[i] != led_instr[i]) && (answering[i] != 0)) {
        seq_running = 1;
        loseSeq();
        memset(answering, 0, sizeof(answering));
        memset(led_instr, 0, sizeof(answering));
        resetBoard();
      }
      Serial.print(answering[i]);
      Serial.print(" "); 
    }
    led_instr[current_instr_length] = led_pins[random(4)];
    ++current_instr_length;
    if (!seq_running) {
      initLedSeq(led_instr, current_instr_length);
    }
  }
  last_b_state = b_state;


  
}
