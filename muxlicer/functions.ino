void read_clock_detect_input () {
  clock_detect = !digitalRead(clock_detect_input);
  if (!clock_detect) {
    if (first_no_clock_detect) {
      first_no_clock_detect = false;
      clk_in_mult = 0;
      write_clock_in_mult_to_EEPROM ();
    }
  }
  else {
    first_no_clock_detect = true;
  }
}

void read_encoder () {
  if (encoder_button_state == 3) {
    if (current_micros - tap_tempo > 300000) {
      encoderValue += encoder->getValue();              //// CHECK THE ENCODER TO KNOW IF we are changing the clock output
      if (encoderValue != lastEncoderValue) {
        lastEncoderValue = encoderValue;
        if (encoderValue >= 4) {                      ///// INCREASE CLOCK OUTPUT
          encoderValue = 0;
          clk_out_mul_changed = true;
          if (no_odd_clocks) {
            no_odd_clock_out_index--;
            if (no_odd_clock_out_index < 0) no_odd_clock_out_index = 0;
            clock_out_mult = no_odd_clocks_array [no_odd_clock_out_index];
          }
          else {
            clock_out_mult--;
          }
        }
        else if (encoderValue <= -4) {                 ///// DECREASE CLOCK OUTPUT
          encoderValue = 0;
          clk_out_mul_changed = true;
          if (no_odd_clocks) {
            no_odd_clock_out_index++;
            if (no_odd_clock_out_index > 8) no_odd_clock_out_index = 8;
            clock_out_mult = no_odd_clocks_array [no_odd_clock_out_index];
          }
          else {
            clock_out_mult++;
          }

        }
        if (clock_out_mult > max_clk_out_mult) {        ///// CLOCK OUTPUT MULT LIMITER
          clock_out_mult = max_clk_out_mult;
        }
        if (clock_out_mult < min_clk_out_mult) {
          clock_out_mult = min_clk_out_mult;
        }
        old_tap_tempo = current_micros + 5010000;
      }


      if (digitalRead(one_shot_switch) == 0 ) {             //////////    RANGE DOWN
        bitWrite(one_shot_switch_state, 0, 1);
      }
      else {
        bitWrite(one_shot_switch_state, 0, 0);
      }

      if (one_shot_switch_state == 1) {
        bitWrite(one_shot_switch_state, 1, 1);
        first_range_timer = current_micros;
        acc_range = 1;
        range_value -= acc_range;
        if (range_value < min_range) {
          range_value = min_range;
        }
        Timer1.pwm (range_output, range_value);
        range_changed = true;
        old_tap_tempo = current_micros + 5010000;
      }
      if (one_shot_switch_state == 3) {
        if ((current_micros - first_range_timer) > 100000) {
          first_range_timer = current_micros;
          range_value -= acc_range;
          acc_range = acc_range << 1;
          if (acc_range > max_acc_range) acc_range = max_acc_range;
          if (range_value < min_range) {
            range_value = min_range;
          }
          Timer1.pwm (range_output, range_value);
          range_changed = true;
          old_tap_tempo = current_micros + 5010000;
        }
      }
      if (one_shot_switch_state == 2) {
        bitWrite(one_shot_switch_state, 1, 0);
      }



      if (digitalRead(start_stop_input) == 0 ) {             //////////    RANGE UP
        bitWrite(start_stop_switch_state, 0, 1);
      }
      else {
        bitWrite(start_stop_switch_state, 0, 0);
      }

      if (start_stop_switch_state == 1) {
        bitWrite(start_stop_switch_state, 1, 1);
        first_range_timer = current_micros;
        acc_range = 1;
        range_value += acc_range;

        if (range_value > max_range) {
          range_value = max_range;
        }
        Timer1.pwm (range_output, range_value);
        range_changed = true;
        old_tap_tempo = current_micros + 5010000;
      }
      if (start_stop_switch_state == 3) {
        if ((current_micros - first_range_timer) > 100000) {
          first_range_timer = current_micros;
          acc_range = acc_range << 1;
          if (acc_range > max_acc_range) acc_range = max_acc_range;
          range_value += acc_range;
          if (range_value > max_range) {
            range_value = max_range;
          }
          Timer1.pwm (range_output, range_value);
          range_changed = true;
          old_tap_tempo = current_micros + 5010000;
        }
      }
      if (start_stop_switch_state == 2) {
        bitWrite(start_stop_switch_state, 1, 0);
      }
    }
  }
  else {
    if (!clock_detect) {                                ///// encoder turned / internal tempo  >>>> chage tempo
      encoderValue += encoder->getValue();
      if (encoderValue != lastEncoderValue) {
        encoder_time_stamp = current_micros;
        if (encoder_first_time) {
          encoder_first_time = false;
          tempo_increment = tempo_increment_min;
        }

        lastEncoderValue = encoderValue;
        if (encoderValue >= 4) {
          encoderValue = 0;
          internal_tempo += tempo_increment;
        }
        else if (encoderValue <= -4) {
          encoderValue = 0;
          internal_tempo -= tempo_increment;
        }
        if (internal_tempo < 500) internal_tempo = 500;
        if (internal_tempo > 4200000000) internal_tempo = 500;
        EEPROM_modified = true;
        EEPROM_counter = current_micros;
        tempo_increment = tempo_increment << 1;
        if (tempo_increment > 4000) tempo_increment = 4000;
        main_tempo = internal_tempo;
      }
      if (current_micros > encoder_time_stamp + 200000) {
        encoder_first_time = true;
      }
    }
    else {                                            ///// encoder turned / external tempo  >>>> clk in multiplier
      encoderValue += encoder->getValue();
      if (encoderValue != lastEncoderValue) {
        lastEncoderValue = encoderValue;
        if (encoderValue >= 4) {
          encoderValue = 0;
          if (no_odd_clocks) {
            no_odd_clock_in_index--;
            if (no_odd_clock_out_index < 0) no_odd_clock_out_index = 0;
            clk_in_mult = no_odd_clocks_array [no_odd_clock_in_index];
          }
          else {
            clk_in_mult--;
          }
        }
        else if (encoderValue <= -4) {
          encoderValue = 0;
          if (no_odd_clocks) {
            no_odd_clock_in_index++;
            if (no_odd_clock_out_index > 8) no_odd_clock_out_index = 8;
            clk_in_mult = no_odd_clocks_array [no_odd_clock_in_index];
          }
          else {
            clk_in_mult++;
          }
        }
        EEPROM_modified = true;
        EEPROM_counter = current_micros;
        if (clk_in_mult > max_clk_in_mult) {
          clk_in_mult = max_clk_in_mult;
        }
        if (clk_in_mult < min_clk_in_mult) {
          clk_in_mult = min_clk_in_mult;
        }
      }
    }
  }
  if (EEPROM_modified) {
    if (current_micros > EEPROM_counter + 3000000) {
      EEPROM_modified = false;
      write_tempo_to_EEPROM ();
      write_clock_in_mult_to_EEPROM ();
    }
  }

}


void read_internal_clock_tap () {
  /// TEMPO TAP
  if (digitalRead(encoder_button) == 0 ) {
    bitWrite(encoder_button_state, 0, 1);
  }
  else {
    bitWrite(encoder_button_state, 0, 0);
  }

  if (encoder_button_state == 1) {                              /// Tap tempo pressed
    bitWrite(encoder_button_state, 1, 1);
    tap_tempo = micros();

    if ( tap_tempo - old_tap_tempo < 3000000) {                 /// if the difference between taps is greater than 5 seconds we discard that diference and start over again
      tap_tempo_array [tap_index] = tap_tempo - old_tap_tempo;
      averaged_taps = 0;
      for (int taps = 0 ; taps <= max_taps ; taps ++) {
        averaged_taps = averaged_taps + tap_tempo_array [taps];
      }
      internal_tempo = averaged_taps / (max_taps + 1);
      main_tempo = internal_tempo;
      EEPROM_modified = true;
      EEPROM_counter = current_micros;
      averaged_taps = 0;
      max_taps ++;
      if (max_taps > 3) max_taps = 3;
      tap_index ++;
      if (tap_index > 3) tap_index = 0;
    }
    else {
      max_taps = 0;
      tap_index = 0;
      averaged_taps = 0;
    }
    old_tap_tempo = tap_tempo;


  }
  if (encoder_button_state == 2) {
    bitWrite(encoder_button_state, 1, 0);
    if (range_changed) {
      range_changed = false;
      write_range_to_EEPROM();
    }
    if (clk_out_mul_changed) {
      clk_out_mul_changed = false;
      write_clock_out_mult_to_EEPROM();
    }
  }
}

void write_tempo_to_EEPROM () {
  /// WRITE TEMPO TO EEPROM TO HAVE THE LAST TEMPO AVAILABLE AFTER POWER CYCLE
  if ( main_tempo != old_main_tempo) {
    old_main_tempo = main_tempo;
    EEPROM.write(0, main_tempo & 0xFF);
    EEPROM.write(1, (main_tempo >> 8) & 0xFF);
    EEPROM.write(2, (main_tempo >> 16) & 0xFF);
    EEPROM.write(3, (main_tempo >> 24) & 0xFF);
  }
}
void write_clock_in_mult_to_EEPROM () {
  /// WRITE Clock in external mult / div TO EEPROM TO HAVE THE LAST value AVAILABLE AFTER POWER CYCLE
  if ( clk_in_mult != old_clk_in_mult) {
    old_clk_in_mult = clk_in_mult;
    EEPROM.write(4, clk_in_mult & 0xFF);
    EEPROM.write(5, (clk_in_mult >> 8) & 0xFF);
    EEPROM.write(6, (clk_in_mult >> 16) & 0xFF);
    EEPROM.write(7, (clk_in_mult >> 24) & 0xFF);
  }
}
void write_range_to_EEPROM () {
  /// WRITE Range TO EEPROM TO HAVE THE LAST Value AFTER POWER CYCLE
  if ( range_value != old_range_value) {
    old_range_value = range_value;
    EEPROM.write(16, range_value & 0xFF);
    EEPROM.write(17, (range_value >> 8) & 0xFF);
    EEPROM.write(18, (range_value >> 16) & 0xFF);
    EEPROM.write(19, (range_value >> 24) & 0xFF);
  }
}

void write_no_odd_gate_mode_to_EEPROM () {
  /// WRITE no_odd_gate_mode TO EEPROM TO HAVE THE LAST Value chosen during POWER CYCLE
  EEPROM.write(9, no_odd_gate_mode);
}

void write_clock_out_mult_to_EEPROM () {
  /// WRITE clock out mult TO EEPROM TO HAVE THE LAST Value after power cycle
  if ( clock_out_mult != old_clock_out_mult) {
    old_clock_out_mult = clock_out_mult;
    EEPROM.write(10, clock_out_mult & 0xFF);
    EEPROM.write(11, (clock_out_mult >> 8) & 0xFF);
    EEPROM.write(12, (clock_out_mult >> 16) & 0xFF);
    EEPROM.write(13, (clock_out_mult >> 24) & 0xFF);
  }
}

void write_no_odd_clocks_to_EEPROM () {
  /// WRITE no_odd_clocks TO EEPROM TO HAVE THE LAST Value chosen during POWER CYCLE
  EEPROM.write(14, no_odd_clocks);
}

void write_no_clock_when_stop_to_EEPROM () {
  EEPROM.write(15, no_clock_out_when_stop);
}

void write_encoder_direction_to_EEPROM () {
  EEPROM.write(20, encoder_direction);
}


void read_start_toggle () {
  //// START STOP TOGGLE CONTROL
  start_stop_estate = !digitalRead (start_stop_input);                             //// Start_stop button

  if ((start_stop_estate == true) && (start_stop_pressed == false) && digitalRead(encoder_button)) {                ///  Start_Stop first pressed
    start_stop_pressed = true;
    start_stop_debounced = false;
    start_stop_counter = current_micros;
    start_stop_down_first = true;
  }

  if ((start_stop_estate == true) && (start_stop_pressed == true)) {                //// STart Stop held
    if (start_stop_debounced == false) {
      if (current_micros > start_stop_counter + start_stop_debounce_time) {
        start_stop_debounced = true;
        start_stop_hold = false;
        start_on = !start_on;

        first_start = true;
        division_counter = -clk_in_mult;
        if (start_on) {
          address_counter = 7;
          next_address_flag = false;
          next_step_flag = false;
          gate_counter_old = current_micros;
          old_micros_mult = current_micros;
          old_clock_out = current_micros;
          old_internal_clock = current_micros;

          if (no_clock_out_when_stop) {
            first_start_2 = false;
            gate_counter_old = 0;
            old_micros_mult = 0;
            old_clock_out = 0;
            old_internal_clock = 0;
          }
          else {
            first_start_2 = true;
          }
        }
      }
    }

    if (!start_stop_hold) {
      if (current_micros > start_stop_counter + hold_time_no_clock_out) {
        Serial.println("CHACHACH");
        no_clock_out_when_stop = !no_clock_out_when_stop;
        write_no_clock_when_stop_to_EEPROM ();
        digitalWrite(clock_out_led, HIGH);
        clock_out_led_always_high = true;
        start_stop_hold = true;
      }
    }
  }

  if ((start_stop_estate == false) && start_stop_pressed && start_stop_debounced) {              /// Start Stop released
    if (start_stop_down_first) {
      start_stop_down_first = false;
      start_stop_counter = current_micros;
    }

    if (current_micros > start_stop_counter + start_stop_debounce_time) {
      start_stop_pressed = false;
      digitalWrite(clock_out_led, LOW);
      clock_out_led_always_high = false;

    }
  }
}


void read_one_shot_reset_input () {
  /// RESET CONTROL

  reset_state = digitalRead(reset_input);
  if ((reset_state == false) && (reset_first == false) ) {                     /// reset_input jack

    reset_first = true;
    if (start_on) {
      address_counter = 7;
      if (one_shot_state) one_shot_start = true;
    }
    else {
      one_shot_start = true;
      address_counter = 0;
      if (counter_active) {
        refresh_mux (address_counter);                                                   //// COUNTER ACTIVE ( address pot minimum)
      }
      else {                                                                             //// COUNTER INACTIVE
        refresh_mux (selected_step);
      }

      division_counter = -clk_in_mult;
    }
  }
  if ((reset_state == true) && (reset_first == true)) {
    reset_first = false;
  }
}

void read_one_shot_reset_toggle () {

  //// ONE SHOT / RESET BUTTON CONTROL
  if ((!digitalRead(one_shot_switch)) && (one_shot_first == false) && digitalRead(encoder_button)) {                     /// reset_input toggle
    one_shot_first = true;
    if (start_on) {
      address_counter = 7;
      if (one_shot_state) one_shot_start = true;
    }
    else {
      one_shot_start = true;
      address_counter = 0;
      if (counter_active) {
        refresh_mux (address_counter);                                                   //// COUNTER ACTIVE ( address pot minimum)
      }
      else {                                                                             //// COUNTER INACTIVE
        refresh_mux (selected_step);
      }
      division_counter = -clk_in_mult;
    }

    //division_counter = -clk_in_mult;
  }
  if ((digitalRead(one_shot_switch)) && (one_shot_first == true)) {
    one_shot_first = false;
  }
}


void read_address () {
  ///// ADDRESS CV/pot
  address_value = analogRead(address_input);
  switch (address_value) {
    case 0 ... 122:
      counter_active = LOW;
      selected_step = 7;
      break;
    case 127 ... 249:
      counter_active = LOW;
      selected_step = 6;
      break;
    case 254 ... 376:
      counter_active = LOW;
      selected_step = 5;
      break;
    case 381 ... 503:
      counter_active = LOW;
      selected_step = 4;
      break;
    case 508 ... 630:
      counter_active = LOW;
      selected_step = 3;
      break;
    case 635 ... 757:
      counter_active = LOW;
      selected_step = 2;
      break;
    case 762 ... 884:
      counter_active = LOW;
      selected_step = 1;
      break;
    case 889 ... 1011:
      counter_active = LOW;
      selected_step = 0;
      break;
    case 1020 ... 1023:
      counter_active = HIGH;
      break;
  }
}

void read_mult_pot () {

  mult_value = analogRead(mult_input);
  switch (mult_value)  {
    case 0 ... 96:                           //// NO GATE
      no_gate = HIGH;
      mult_amount = 1;
      no_gate_or_full = true;
      break;

    case 108 ... 198:                          ///// FULL GATE
      if ( !clock_detect) {
        mult_tempo = internal_tempo;
      }
      else {
        mult_tempo = ext_clock_mult;
      }
      no_gate_or_full = true;
      no_gate = LOW;
      mult_amount = 1;
      break;

    case 210 ... 301:                          ///// HALF GATE
      if ( !clock_detect) {
        mult_tempo = internal_tempo;
      }
      else {
        mult_tempo = ext_clock_mult;
      }
      no_gate = LOW;
      no_gate_or_full = false;
      mult_amount = 1;
      break;

    case  313 ... 403:                        ///// x2
      if ( !clock_detect) {
        mult_tempo = internal_tempo >> 1;
      }
      else {
        mult_tempo = ext_clock_mult >> 1;
      }
      no_gate = LOW;
      no_gate_or_full = false;
      mult_amount = 2;
      break;

    case 415 ... 506:                         ///// x3
      if ( !clock_detect) {
        if (no_odd_gate_mode) {
          mult_tempo = internal_tempo >> 1;
          mult_amount = 2;
        }
        else {
          mult_tempo = internal_tempo / 3;
          mult_amount = 3;
        }
      }
      else {
        if (no_odd_gate_mode) {
          mult_tempo = ext_clock_mult >> 1;
          mult_amount = 2;
        }
        else {
          mult_tempo = ext_clock_mult / 3;
          mult_amount = 3;
        }
      }
      no_gate = LOW;
      no_gate_or_full = false;
      break;

    case 518 ... 608:                           //// x4
      if ( !clock_detect) {
        mult_tempo = internal_tempo >> 2;
      }
      else {
        mult_tempo = ext_clock_mult >> 2;
      }
      no_gate = LOW;
      no_gate_or_full = false;
      mult_amount = 4;
      break;

    case 620 ... 710:                           //// x5
      if ( !clock_detect) {
        if (no_odd_gate_mode) {
          mult_tempo = internal_tempo >> 2;
          mult_amount = 4;
        }
        else {
          mult_tempo = internal_tempo / 5;
          mult_amount = 5;
        }
      }
      else {
        if (no_odd_gate_mode) {
          mult_tempo = ext_clock_mult >> 2;
          mult_amount = 4;
        }
        else {
          mult_tempo = ext_clock_mult / 5;
          mult_amount = 5;
        }
      }
      no_gate = LOW;
      no_gate_or_full = false;
      break;

    case 722 ... 813:                      ///// x6
      if ( !clock_detect) {
        if (no_odd_gate_mode) {
          mult_tempo = internal_tempo >> 2;
          mult_amount = 4;
        }
        else {
          mult_tempo = internal_tempo / 6;
          mult_amount = 6;
        }
      }
      else {
        if (no_odd_gate_mode) {
          mult_tempo = ext_clock_mult >> 2;
          mult_amount = 4;
        }
        else {
          mult_tempo = ext_clock_mult / 6;
          mult_amount = 6;
        }
      }
      no_gate = LOW;
      no_gate_or_full = false;
      break;

    case 825 ... 915:                       ///// x7
      if ( !clock_detect) {
        if (no_odd_gate_mode) {
          mult_tempo = internal_tempo >> 3;
          mult_amount = 8;
        }
        else {
          mult_tempo = internal_tempo / 7;
          mult_amount = 7;
        }
      }
      else {
        if (no_odd_gate_mode) {
          mult_tempo = ext_clock_mult >> 3;
          mult_amount = 8;
        }
        else {
          mult_tempo = ext_clock_mult / 7;
          mult_amount = 7;
        }
      }
      no_gate = LOW;
      no_gate_or_full = false;
      break;

    case 927 ... 1023:                      ///// x8
      if (!clock_detect) {
        mult_tempo = internal_tempo >> 3;
      }
      else {
        mult_tempo = ext_clock_mult >> 3;
      }
      mult_amount = 8;
      no_gate = LOW;
      no_gate_or_full = false;
      break;
  }
  //mult_tempo -= 10;
}


void read_clock () {
  ///////////////////////////////ext
  current_micros = micros();

  if (clock_detect) {
    if ((digitalRead(clock_input)) && (external_clock_first == false)) {                      /// IF THE CLOCK IS HIGH AND IT IS THE FIRST TIME IT IS,
      ext_clock = current_micros - old_external_clock;
      old_external_clock =  current_micros;
      external_clock_first = true;

      if (one_shot_start) {
        one_shot_start = false;
        one_shot_state = true;
        address_counter = 7;
        start_on = true;
        repetition_counter = 0;
        first_one_shot_step = true;
        clock_out_state = LOW;
      }
      if (first_start) {
        first_start = false;
        address_counter = 7;
        repetition_counter = 0;
        clock_out_state = LOW;
      }
      calculate_clock_out ();                     /// count the tics to create the clock out bearing in mind the clock out multiplier, triggering
      if (clock_out_mult > 0) {                 /// CLOCK OUT MULTIPLIER
        old_clock_out = current_micros;
        digitalWrite(clock_out, LOW);
        digitalWrite(clock_out_led, HIGH);
        clock_out_mult_counter = -1;
        clock_flag = HIGH;
        clock_out_state = HIGH;
      }
      else if (clock_out_mult < 0) {            //// CLOCK OUT DIVIDER
        clock_out_div_counter++;
        int clock_out_mult_temp = -clock_out_mult + 1;
        if (clock_out_div_counter >= clock_out_mult_temp) {
          clock_flag = true;
          //
          clock_out_state = HIGH;
          //
          old_clock_out = current_micros;
          digitalWrite(clock_out, LOW);
          digitalWrite(clock_out_led, HIGH);
          clock_out_div_counter = 0;
        }

      }
      else {                                   //// CLOCK OUT NEUTRAL
        old_clock_out = current_micros;
        digitalWrite(clock_out, LOW);
        digitalWrite(clock_out_led, HIGH);
        clock_flag = HIGH;
        clock_out_state = HIGH;
      }




      if (start_on) {                               ////// count tics to modify the clock in, bearing in mind the clk in mult, this triggers the change of address and step
        if (clk_in_mult > 0) {
          ext_clock_mult = ext_clock / (clk_in_mult + 1);
          gate_out_window = ext_clock;
          next_address_flag = true;
          next_step_flag = true;
          old_micros_mult = current_micros;
          gate_counter_old = current_micros;
          next_address_stamp = current_micros;
          repetitions_counter = 0;
        }
        else if (clk_in_mult < 0) {
          ext_clock_mult = ext_clock * (-clk_in_mult + 1);
          gate_out_window = ext_clock_mult;
          division_counter++;
          if (division_counter > (-clk_in_mult)) {
            division_counter = 0;
            next_address_flag = true;
            next_step_flag = true;
            old_micros_mult = current_micros;
            gate_counter_old = current_micros;
            next_address_stamp = current_micros;
          }
        }
        else {
          ext_clock_mult = ext_clock;
          gate_out_window = ext_clock_mult;
          next_address_flag = true;
          next_step_flag = true;
          old_micros_mult = current_micros;
          gate_counter_old = current_micros;
          next_address_stamp = current_micros;
        }
      }
    }
  }
  else {                                                            ///// clock internal
    if (current_micros > old_internal_clock + main_tempo) {
      old_internal_clock = current_micros;
      if (one_shot_start) {
        one_shot_start = false;
        one_shot_state = true;
        address_counter = 7;
        start_on = true;
        repetition_counter = 0;
        first_one_shot_step = true;
        clock_out_state = LOW;
      }
      if (first_start) {
        first_start = false;
        address_counter = 7;
        repetition_counter = 0;
        clock_out_state = LOW;
      }

      calculate_clock_out ();
      if (clock_out_mult > 0) {                 /// CLOCK OUT MULTIPLIER
        old_clock_out = current_micros;
        if (no_clock_out_when_stop && !start_on) {
          digitalWrite(clock_out, HIGH);
        }
        else {
          digitalWrite(clock_out, LOW);
        }
        digitalWrite(clock_out_led, HIGH);
        clock_out_mult_counter = -1;
        clock_flag = HIGH;
        clock_out_state = HIGH;
      }
      else if (clock_out_mult < 0) {            //// CLOCK OUT DIVIDER
        clock_out_div_counter++;
        int clock_out_mult_temp = -clock_out_mult;
        if (clock_out_div_counter >= clock_out_mult_temp) {
          clock_flag = false;
          //
          clock_out_state = !clock_out_state;
          //
          old_clock_out = current_micros;
          if (no_clock_out_when_stop && !start_on) {
            digitalWrite(clock_out, HIGH);
          }
          else {
            digitalWrite(clock_out, LOW);
          }
          digitalWrite(clock_out_led, !clock_out_state);
          clock_out_div_counter = 0;
        }
      }
      else {                                   //// CLOCK OUT NEUTRAL
        old_clock_out = current_micros;
        if (no_clock_out_when_stop && !start_on) {
          digitalWrite(clock_out, HIGH);
        }
        else {
          digitalWrite(clock_out, LOW);
        }
        digitalWrite(clock_out_led, HIGH);
        clock_flag = HIGH;
        clock_out_state = HIGH;
      }
      if (start_on) {
        next_address_flag = true;
        next_step_flag = true;
        old_micros_mult = current_micros;
        gate_counter_old = current_micros;
      }
    }
  }
  if ((!digitalRead(clock_input)) && (external_clock_first)) external_clock_first = 0;

}

void event_control () {

  current_micros = micros();
  if (start_on) {
    if ( current_micros > old_micros_mult + mult_tempo) {                  //// GATE CONTROL
      if (((current_micros <  next_address_stamp  + gate_out_window) && clock_detect) || !clock_detect) {
        old_micros_mult = current_micros;
        gate_counter_old = current_micros;
        next_step_flag = true;
        if (clock_detect) {
          if (clk_in_mult > 0) {
            repetitions_counter++;
            if (repetitions_counter >= mult_amount ) {
              next_address_flag = true;
              repetitions_counter = 0;
            }
          }
        }
      }
    }
  }
  else {
    read_mult_pot ();
  }
}

void check_one_shot_windows () {
  if (one_shot_window) {
    if (current_micros > next_one_shot_window_close) {
      one_shot_window = false;
    }
  }
}

void control_eoc () {
  if (next_eoc) {
    if (eoc_state) {
      if (current_micros > eoc_time) {
        digitalWrite (eoc_out, !eoc_state);
        eoc_state = false;
        eoc_time = current_micros + 10000;
      }
    }
    else {
      if (current_micros > eoc_time) {
        digitalWrite (eoc_out, !eoc_state);
        next_eoc = false;
      }
    }
  }
}

void next_address () {
  if (next_address_flag == true) {
    next_address_flag = false;
    address_counter++;
    address_counter = address_counter & B00000111;
    first_start_2 = false;
    if (counter_active) {
      refresh_mux (address_counter);                                             //// COUNTER ACTIVE ( address pot minimum)
    }
    else {                                                                        //// COUNTER INACTIVE
      refresh_mux (selected_step);
    }
    if (address_counter == 7) {
      if (clock_detect) {
        eoc_time = current_micros + ext_clock_mult - 1000;
      }
      else {
        eoc_time = current_micros + internal_tempo - 1000;
      }
      next_eoc = true;
      eoc_state = true;
    }
  }
}

void next_step () {
  if (next_step_flag) {
    next_step_flag = false;
    gate_counter_old = current_micros;
    old_micros_mult = current_micros;


    gate_delay_counter = current_micros;
    if (!first_start_2) {
      gate_delay_flag = true;
    }

    if (address_counter == 7) {
      if (one_shot_state) {
        repetition_counter ++;
        if (repetition_counter > (mult_amount - 1)) {
          repetition_counter = 0;
          one_shot_state = false;
          next_start_on = true;
          start_on = false;
        }
      }
    }
  }
}

void gate_delay () {
  if (gate_delay_flag) {
    if (current_micros > gate_delay_counter + gate_delay_amount) {
      gate_delay_flag = false;
      read_mult_pot ();
      if (!no_gate) {
        digitalWrite(all_gate_out, LOW);
      }
      else {
        digitalWrite(all_gate_out, HIGH);
      }
    }
  }
}

void gate_to_low_control () {
  /// GATE GOING-LOW CONTROL
  gate_counter = micros();
  if (no_gate_or_full) {
    if ( gate_counter > gate_counter_old + mult_tempo - full_gate_down ) {
      digitalWrite(all_gate_out, HIGH);
      gate_counter_old  = gate_counter;
    }
  }
  else {
    if ( gate_counter > gate_counter_old + (mult_tempo >> 1)) {              /// gate going LOW
      digitalWrite(all_gate_out, HIGH);
      gate_counter_old  = gate_counter;
    }
  }
}





void control_clock_out () {

  if ((start_on  || one_shot_window) || (!start_on && clock_detect)  || (!start_on && !no_clock_out_when_stop && !clock_detect)) {

    if (clock_flag) {
      if (clock_out_mult > 0) {                 /// CLOCK OUT MULTIPLIER
        if (clock_out_state) {
          if (current_micros > old_clock_out + (clock_out_tempo >> 1) ) {
            digitalWrite(clock_out, HIGH);
            if (clock_out_led_always_high) {
              digitalWrite(clock_out_led, HIGH);
            }
            else {
              digitalWrite(clock_out_led, LOW);
            }
            old_clock_out = current_micros;
            clock_out_mult_counter++;
            clock_out_state = LOW;
            if (clock_out_mult_counter >= clock_out_mult) {
              clock_flag = false;
            }
          }
        }
        else {
          if (current_micros > old_clock_out + (clock_out_tempo >> 1) ) {
            digitalWrite(clock_out, LOW);
            digitalWrite(clock_out_led, HIGH);
            old_clock_out = current_micros;
            clock_out_state = HIGH;
          }
        }

      }
      else if (clock_out_mult < 0) {            //// CLOCK OUT DIVIDER

        if (current_micros > old_clock_out + (clock_out_tempo >> 1)) {
          digitalWrite(clock_out, HIGH);
          if (clock_out_led_always_high) {
            digitalWrite(clock_out_led, HIGH);
          }
          else {
            digitalWrite(clock_out_led, LOW);
          }
          old_clock_out = current_micros;
          clock_flag = false;
        }

      }
      else {                                   //// CLOCK OUT NEUTRAL
        if (current_micros > old_clock_out + (clock_out_tempo >> 1)) {
          digitalWrite(clock_out, HIGH);
          if (clock_out_led_always_high) {
            digitalWrite(clock_out_led, HIGH);
          }
          else {
            digitalWrite(clock_out_led, LOW);
          }
          old_clock_out = current_micros + 500;
          clock_flag = false;
        }
      }
    }
  }
  else {
    digitalWrite(clock_out, HIGH);
    if (clock_out_led_always_high) {
      digitalWrite(clock_out_led, HIGH);
    }
    else {
      digitalWrite(clock_out_led, LOW);
    }
  }
}
void calculate_clock_out () {
  if (clock_out_mult > 0) {
    if (!clock_detect) {
      clock_out_tempo = internal_tempo / (clock_out_mult + 1);
    }
    else {
      clock_out_tempo = ext_clock / (clock_out_mult + 1);
    }
  }

  else if (clock_out_mult < 0) {
    int clock_out_mult_temp = -clock_out_mult + 1;
    if (!clock_detect) {
      clock_out_tempo = internal_tempo * clock_out_mult_temp;
    }
    else {
      clock_out_tempo = ext_clock * clock_out_mult_temp;
    }
  }

  else {
    if ( !clock_detect) {
      clock_out_tempo = internal_tempo;
    }
    else {
      clock_out_tempo = ext_clock;
    }
  }
}

void refresh_mux (byte myAddress) {
  bool address1 = bitRead(myAddress, 0);
  bool address2 = bitRead(myAddress, 1);
  bool address3 = bitRead(myAddress, 2);
  digitalWrite(enable_mux, LOW);
  digitalWrite(mux_A_0_pin, address1);
  digitalWrite(mux_A_1_pin, address2);
  digitalWrite(mux_A_2_pin, address3);
  digitalWrite(enable_mux, HIGH);
}


void change_no_odds_clock_out () {
  if (clock_out_mult < -20) {
    clock_out_mult = -32;
    no_odd_clock_out_index = 0;
  }
  else if ((clock_out_mult > -20) && (clock_out_mult < -10)) {
    clock_out_mult = -15;
    no_odd_clock_out_index = 1;
  }
  else if ((clock_out_mult >= -10) && (clock_out_mult < -5)) {
    clock_out_mult = -7;
    no_odd_clock_out_index = 2;
  }
  else if ((clock_out_mult >= -5) && (clock_out_mult < -2)) {
    clock_out_mult = -3;
    no_odd_clock_out_index = 3;
  }
  else if ((clock_out_mult >= -2) && (clock_out_mult < 0)) {
    clock_out_mult = -1;
    no_odd_clock_out_index = 4;
  }
  else if (clock_out_mult == 0) {
    clock_out_mult = 0;
    no_odd_clock_out_index = 5;
  }
  else if ((clock_out_mult > 0) && (clock_out_mult <= 2)) {
    clock_out_mult = 3;
    no_odd_clock_out_index = 6;
  }
  else {
    clock_out_mult = 7;
    no_odd_clock_out_index = 7;
  }
}

void change_no_odds_clock_in () {
  if (clk_in_mult < -20) {
    clk_in_mult = -32;
    no_odd_clock_in_index = 0;
  }
  else if ((clk_in_mult > -20) && (clk_in_mult < -10)) {
    clk_in_mult = -15;
    no_odd_clock_in_index = 1;
  }
  else if ((clk_in_mult >= -10) && (clk_in_mult < -5)) {
    clk_in_mult = -7;
    no_odd_clock_in_index = 2;
  }
  else if ((clk_in_mult >= -5) && (clk_in_mult < -2)) {
    clk_in_mult = -3;
    no_odd_clock_in_index = 3;
  }
  else if ((clk_in_mult >= -2) && (clk_in_mult < 0)) {
    clk_in_mult = -1;
    no_odd_clock_in_index = 4;
  }
  else if (clk_in_mult == 0) {
    clk_in_mult = 0;
    no_odd_clock_in_index = 5;
  }
  else if ((clk_in_mult > 0) && (clk_in_mult <= 2)) {
    clk_in_mult = 3;
    no_odd_clock_in_index = 6;
  }
  else {
    clk_in_mult = 7;
    no_odd_clock_in_index = 7;
  }
}

void initial_blink () {
  for (byte myRepetitions = 0 ; myRepetitions < 2 ; myRepetitions++) {
    for (unsigned int myTime = 0; myTime < blink_time ; myTime++) {
      digitalWrite(enable_mux, LOW);
      delay(1);
    }
    for (unsigned int myTime = 0; myTime < blink_time ; myTime++) {
      for (byte myAddress = 0 ; myAddress < 8 ; myAddress++) {
        bool address1 = bitRead(myAddress, 0);
        bool address2 = bitRead(myAddress, 1);
        bool address3 = bitRead(myAddress, 2);
        digitalWrite(enable_mux, LOW);
        digitalWrite(mux_A_0_pin, address1);
        digitalWrite(mux_A_1_pin, address2);
        digitalWrite(mux_A_2_pin, address3);
        digitalWrite(enable_mux, HIGH);
        delayMicroseconds(200);
      }
    }
  }
}

void confirmation_blink (bool myState) {

  if (myState) {
    for (byte myAddress = 0 ; myAddress < 8 ; myAddress++) {
      bool address1 = bitRead(myAddress, 0);
      bool address2 = bitRead(myAddress, 1);
      bool address3 = bitRead(myAddress, 2);
      digitalWrite(enable_mux, LOW);
      digitalWrite(mux_A_0_pin, address1);
      digitalWrite(mux_A_1_pin, address2);
      digitalWrite(mux_A_2_pin, address3);
      digitalWrite(enable_mux, HIGH);
      delay(75);
    }
  }
  else {
    byte myAddress = 7;
    for (byte myAddress = 7 ; myAddress > 0 ; myAddress--) {
      bool address1 = bitRead(myAddress, 0);
      bool address2 = bitRead(myAddress, 1);
      bool address3 = bitRead(myAddress, 2);
      digitalWrite(enable_mux, LOW);
      digitalWrite(mux_A_0_pin, address1);
      digitalWrite(mux_A_1_pin, address2);
      digitalWrite(mux_A_2_pin, address3);
      digitalWrite(enable_mux, HIGH);
      delay(75);
    }


  }

}

