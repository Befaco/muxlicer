 /// LIBRARYS

#include <TimerOne.h>
#include <ClickEncoder.h>
#include <EEPROM.h>


/// Encoder
ClickEncoder *encoder;
int16_t lastEncoderValue, encoderValue;

void timerIsr() {
  encoder->service();
}



//// PORTS DEFINITION

//INS
const int mult_input = A0;   //// CV Multiplier IN / Divisor ; inverted ; 10V Range
const int address_input = A1;    //// Address CV IN ; inverted ; 10V Range
const int clock_detect_input = A2;  //// Detect external clock ; inverted
const int reset_input = A3;         //// reset / on shoot input
const int one_shot_switch = A4;     //// reset / one shoot toggle switch
const int start_stop_input = A5;    //// start / stop toggle switch
const int encoder_A = 2;
const int encoder_B = 3;
const int clock_input = 4;
const int encoder_button = 5;

//OUTS
const int clock_out = 0;
////remove next line if you want to debug
const int clock_out_led = 1;

const int mux_A_0_pin = 6;
const int mux_A_1_pin = 7;
const int mux_A_2_pin = 8;
const int range_output = 9;   /// to control the range transistor
const int all_gate_out = 10;  /// the output the outputs all the steps gates
const int enable_mux = 13;    /// enable of the DG409 MUX
const int eoc_out = 11;

/// VARIABLES

bool next_step_flag = false;
bool next_address_flag = false;

unsigned long main_tempo = 250000;
unsigned long old_main_tempo = 250000;
unsigned long old_internal_clock = 0;
unsigned long internal_tempo = 250000;
unsigned long mult_tempo = 0;
unsigned long current_micros = 0;
unsigned long old_micros = 0;
unsigned long old_micros_mult = 0;
unsigned long old_clock_out = 0;
unsigned long next_address_stamp = 0;
unsigned long gate_out_window = 0;

int clock_out_mult = 3;
int old_clock_out_mult = 0;

unsigned int clock_out_mult_counter = -1;
unsigned int clock_out_div_counter = 0;
unsigned long clock_out_tempo = 0;
bool clk_out_mul_changed = false;
bool clock_flag = false;
bool clock_out_state = true;
bool no_clock_out_when_stop = true;
bool clock_out_led_always_high = false;

bool first_no_clock_detect = false;

unsigned long clock_in_counter = 0;
bool clock_in_counter_first = false;

byte address_counter = 0;
byte selected_step = 0;

int mult_value = 0;
byte mult_amount = 1;
byte repetition_counter = 0;
int address_value = 0;


unsigned long eoc_time = 0;
bool eoc_state = false;
bool next_eoc = false;

unsigned int gate_delay_counter = 0;
unsigned int gate_delay_amount = 1000;
bool gate_delay_flag = false;


int repetitions_counter = 0;

bool counter_active = true;

bool start_stop_estate = false;
bool start_stop_pressed = false;
bool start_stop_hold = true;
unsigned long start_stop_counter = 0;
bool clock_detect = false;
bool start_on = false;
bool next_start_on = false;
bool first_start = false;
bool start_stop_debounced = false;

bool start_stop_down_first = true;

unsigned long  current_external_clock = 0;
unsigned long  ext_clock = 0;
unsigned long  ext_clock_mult = 0;
unsigned long  old_micros_address = 0;
unsigned long  old_external_clock = 0;
unsigned long gate_counter = 0;
unsigned long gate_counter_old = 0;
byte delay_mux = 10;
byte division_counter = 0;

int clk_in_mult = 0;
int old_clk_in_mult = 0;

byte start_stop_switch_state = 0;
byte one_shot_switch_state = 0;


/// tap tempo
unsigned long tap_tempo = 0;
unsigned long old_tap_tempo = 0;
unsigned long tap_tempo_array[4] = {0, 0, 0, 0};
byte max_taps = 0;
byte tap_index = 0;
unsigned long averaged_taps = 0;


bool clock_divided_estate = false;

bool first_gate = false;
bool second_gate = false;

bool no_gate_or_full = false;

bool external_clock_first = false;
bool reset_first = false;
bool reset_state = false;
bool one_shot_state = false;
bool one_shot_first = false;
bool one_shot_start = false;
bool no_gate = false;
bool first_one_shot_step = false;
bool one_shot_window = false;
unsigned long next_one_shot_window_close = 0;

byte encoder_button_state = 0;
unsigned long hold_encoder_button_counter_old = 0;
unsigned long hold_encoder_button_counter = 0;
bool encoder_first_time = true;
unsigned long encoder_time_stamp = 0;

bool range_adjust = 0;
int range_value = 1023;
int old_range_value = 2;
int max_range = 1023;
int min_range = 2;
bool range_changed = false;
bool first_range_switch = true;
unsigned long first_range_timer = 0;
unsigned int acc_range = 1;



bool no_odd_gate_mode = 0;   //// to avoid choosing non-quadratic repetitions in gate mode pot
bool no_odd_clocks = 0;       //// to avoid choosing non-quadratic repetitions in clock out or clock in mult/div
int no_odd_clocks_array [9] = { -31, -15, -7, -3, -1, 0, 1, 3, 7};
int no_odd_clock_out_index = 0;
int no_odd_clock_in_index = 0;


bool EEPROM_modified = false;
unsigned long EEPROM_counter = 0;


bool new_code = false;

/// EDITABLES

unsigned int tempo_increment = 200;
unsigned int tempo_increment_min = 200;

int max_clk_in_mult = 15;
int min_clk_in_mult = -15;

int max_clk_out_mult = 15;
int min_clk_out_mult = -15; 

unsigned int full_gate_down = 5000; // approximately add 3000 to the desired value

unsigned int max_acc_range = 128;

unsigned int blink_time = 200;

unsigned long hold_time_no_clock_out = 3000000;

unsigned long start_stop_debounce_time = 1500;

//// BORRA

unsigned long value1 = 0;
unsigned long value2 = 0;
unsigned long value3 = 0;
unsigned long value4 = 0;
unsigned long borra_old_counter = 0;
unsigned long borra_counter = 0;
bool borra_reset = 0;
bool caca = false;

void setup() {

  //Serial.begin(115200);

  /// INPUTS
  pinMode (mult_input, INPUT);
  pinMode (address_input, INPUT);
  pinMode (clock_detect_input, INPUT);
  pinMode (reset_input, INPUT_PULLUP);
  pinMode (start_stop_input, INPUT_PULLUP);
  pinMode (one_shot_switch, INPUT_PULLUP);
  pinMode (clock_input, INPUT_PULLUP);


  pinMode (encoder_button, INPUT_PULLUP);
  pinMode (encoder_A, INPUT_PULLUP);
  pinMode (encoder_B, INPUT_PULLUP);


  // OUTPUTS
  pinMode (mux_A_0_pin, OUTPUT);
  pinMode (mux_A_1_pin, OUTPUT);
  pinMode (mux_A_2_pin, OUTPUT);
  pinMode (range_output, OUTPUT);
  pinMode (clock_out, OUTPUT);
  ////remove next line if you want to debug
  pinMode (clock_out_led, OUTPUT);
  pinMode (all_gate_out, OUTPUT);
  pinMode (enable_mux, OUTPUT);

  pinMode (eoc_out, OUTPUT);


  /// Encoder
  encoder = new ClickEncoder(encoder_A, encoder_B, encoder_button);
  Timer1.initialize(100);
  Timer1.attachInterrupt(timerIsr);


  /// read TEMPO from EEPROM
  main_tempo = (EEPROM.read(0) & 0xFF) + (((long)EEPROM.read(1) << 8) & 0xFFFF) + (((long)EEPROM.read(2) << 16) & 0xFFFFFF) + (((long)EEPROM.read(3) << 24) & 0xFFFFFFFF);
  internal_tempo = main_tempo;
  old_main_tempo = main_tempo;
  old_internal_clock = main_tempo;
  if (main_tempo  == 4294967295) {
    new_code = true;
    main_tempo = 240000;
    internal_tempo = main_tempo;
    old_main_tempo = main_tempo;
    old_internal_clock = main_tempo;
    write_tempo_to_EEPROM ();
  }


  /// read external clock mult / div from EEPROM
  if (!new_code) {
    clk_in_mult = (EEPROM.read(4) & 0xFF) + (((long)EEPROM.read(5) << 8) & 0xFFFF) + (((long)EEPROM.read(6) << 16) & 0xFFFFFF) + (((long)EEPROM.read(7) << 24) & 0xFFFFFFFF);
    old_clk_in_mult = clk_in_mult;
  }
  else {
    clk_in_mult = 0;
    write_clock_in_mult_to_EEPROM ();
  }

  /// read CV output range from EEPROM
  if (!new_code) {
    range_value = (EEPROM.read(16) & 0xFF) + (((long)EEPROM.read(17) << 8) & 0xFFFF) + (((long)EEPROM.read(18) << 16) & 0xFFFFFF) + (((long)EEPROM.read(19) << 24) & 0xFFFFFFFF);
    old_range_value = range_value;
  }
  else {
    range_value = 1023;
    write_range_to_EEPROM();
  }
  Timer1.pwm (range_output, range_value);
  /// read clock_out_mult from EEPROM
  if (!new_code) {
    clock_out_mult = (EEPROM.read(10) & 0xFF) + (((long)EEPROM.read(11) << 8) & 0xFFFF) + (((long)EEPROM.read(12) << 16) & 0xFFFFFF) + (((long)EEPROM.read(13) << 24) & 0xFFFFFFFF);
    old_clock_out_mult = clock_out_mult;
  }
  else {
    clock_out_mult = 0;
    write_clock_out_mult_to_EEPROM ();
  }

  /// read no_odd_gate_mode from EEPROM
  if (!new_code) {
    no_odd_gate_mode = EEPROM.read(9);
  }
  else {
    no_odd_gate_mode = 0;
    write_no_odd_gate_mode_to_EEPROM ();
  }
  if (!digitalRead(one_shot_switch)) {
    no_odd_gate_mode = !no_odd_gate_mode;
    write_no_odd_gate_mode_to_EEPROM();
    confirmation_blink(no_odd_gate_mode);
  }
  else {
    initial_blink ();
  }

  /// read no_odd_clocks from EEPROM
  if (!new_code) {
    no_odd_clocks = EEPROM.read(14);
  }
  else {
    no_odd_clocks = 0;
    write_no_odd_clocks_to_EEPROM ();
  }

  if (!digitalRead(start_stop_input)) {
    no_odd_clocks = !no_odd_clocks;
    write_no_odd_clocks_to_EEPROM();
    confirmation_blink(no_odd_clocks);
  }
  else {
    initial_blink ();
  }
  if (no_odd_clocks) {
    change_no_odds_clock_in();
    change_no_odds_clock_out();
    digitalWrite(enable_mux, HIGH);
  }
  if (!new_code) {
    no_clock_out_when_stop = EEPROM.read(15);

  }
  else {
    no_clock_out_when_stop = 1;
    write_no_clock_when_stop_to_EEPROM ();
  }


}

void loop() {

  //// TEMPO ENCODER OR RANGE ADJUST WITH ENCODER
  current_micros = micros();
  read_clock_detect_input ();
  read_encoder ();
  read_internal_clock_tap ();
  read_start_toggle ();
  read_one_shot_reset_input ();
  read_one_shot_reset_toggle ();
  read_address ();




  borra_counter = millis();
  if (borra_counter > borra_old_counter + 1000) {
    borra_old_counter = borra_counter;
    //Serial.print("clk_detect: ");           Serial.print(clock_detect);
    //Serial.print("// main_tempo: ");        Serial.println(main_tempo);
    //Serial.print("// start_on: ");          Serial.print(start_on);
    //Serial.print("// one_shot_state: ");    Serial.print(one_shot_state);
    //Serial.print("// borra_reset: ");       Serial.print(borra_reset);
    //Serial.print("// counter_active: ");    Serial.print(counter_active);
    //Serial.print(" // clk_in_mult: ");        Serial.print(clk_in_mult);
    //Serial.print(" // clk_out_mult: ");        Serial.print(clock_out_mult);
    //Serial.print(" // mul_aomunt: ");        Serial.println(mult_amount);
    //Serial.print(" // range_value: ");        Serial.println(range_value);
    //Serial.print(" // INT: ");        Serial.println(main_tempo);
      //  Serial.print(" // No_clk_out: ");        Serial.println(no_clock_out_when_stop);
  }

  //one_shot_reset_control ();
  read_clock() ;
  event_control();
  check_one_shot_windows ();
  control_eoc ();
  next_address();
  next_step();
  gate_delay();
  control_clock_out();
  gate_to_low_control ();
}
