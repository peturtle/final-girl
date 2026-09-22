#include "main.h"
#include <stdio.h>
#include <stdlib.h>

uint8_t last_enc1_pos = 64;
uint8_t last_enc2_pos = 64; 
uint8_t last_enc3_pos = 64;

bool state_fetched = false;

struct Band {
  uint16_t freq;
  uint16_t q;
  int8_t lvl;
};

#define FREQ_LOW_LIMIT 10
#define FREQ_HI_LIMIT 22000
#define Q_LOW_LIMIT 10
#define Q_HI_LIMIT 1800
#define LVL_LOW_LIMIT -20
#define LVL_HI_LIMIT 20

static const uint16_t default_band_freqs[8] = {20, 45, 60, 120, 200, 500, 1000, 10000};

struct Band bands[8] = {
  [0] = { default_band_freqs[0], 10, 0},
  [1] = { default_band_freqs[1], 10, 0},
  [2] = { default_band_freqs[2], 10, 0},
  [3] = { default_band_freqs[3], 10, 0},
  [4] = { default_band_freqs[4], 10, 0},
  [5] = { default_band_freqs[5], 10, 0},
  [6] = { default_band_freqs[6], 10, 0},
  [7] = { default_band_freqs[7], 10, 0}
};

int selected_band = 0;

void fetch_all_band_state(void) {
  // THIS IS WHERE WE POPULATE BANDS STRUCT FROM SAVED MEMORY
  state_fetched = true;
}

void increment_selected_band_vars(int16_t freq_i, int16_t q_i, int8_t lvl_i) {
  uint16_t freq;
  uint16_t q;
  int8_t lvl;
  if (
    !(bands[selected_band].freq + freq_i < FREQ_LOW_LIMIT) &&
    !(bands[selected_band].freq + freq_i > FREQ_HI_LIMIT) &&
    !(bands[selected_band].q + q_i < Q_LOW_LIMIT) &&
    !(bands[selected_band].q + q_i > Q_HI_LIMIT) &&
    !(bands[selected_band].lvl + lvl_i < LVL_LOW_LIMIT) &&
    !(bands[selected_band].lvl + lvl_i > LVL_HI_LIMIT)
  ) {
      freq = bands[selected_band].freq + freq_i;
      q = bands[selected_band].q + q_i;
      lvl = bands[selected_band].lvl + lvl_i;
  } else {
    freq = bands[selected_band].freq;
    q = bands[selected_band].q;
    lvl = bands[selected_band].lvl;

    if (bands[selected_band].freq + freq_i < FREQ_LOW_LIMIT) freq = FREQ_LOW_LIMIT;
    else if (bands[selected_band].freq + freq_i > FREQ_HI_LIMIT) freq = FREQ_HI_LIMIT;
    else if (bands[selected_band].q + q_i < Q_LOW_LIMIT) q = Q_LOW_LIMIT;
    else if (bands[selected_band].q + q_i > Q_HI_LIMIT) q = Q_HI_LIMIT; 
    else if (bands[selected_band].lvl + lvl_i < LVL_LOW_LIMIT) lvl = LVL_LOW_LIMIT; 
    else if (bands[selected_band].lvl + lvl_i > LVL_HI_LIMIT) lvl = LVL_HI_LIMIT;
  }

  struct Band b ={ freq, q, lvl };
  bands[selected_band] = b;

  last_enc1_pos = enc1_position >> 1U;
  last_enc2_pos = enc2_position >> 1U;
  last_enc3_pos = enc3_position >> 1U;
}

void set_values_from_encoders(void) {
  uint8_t enc1 = enc1_position >> 1U;
  uint8_t enc2 = enc2_position >> 1U;
  uint8_t enc3 = enc3_position >> 1U;

  // if encoder loops arount to 0 or 127 do nothing
  if (
    enc1 >= last_enc1_pos + 100 ||
    enc1 <= last_enc1_pos - 100 || 
    enc2 >= last_enc2_pos + 100 ||
    enc2 <= last_enc2_pos - 100 || 
    enc3 >= last_enc3_pos + 100 ||
    enc3 <= last_enc3_pos - 100
  ) increment_selected_band_vars(0,0,0);

  // fn + turn encoder steps by very large number or goes to max
  else if (!tact1_level && enc1 >= last_enc1_pos + 2) increment_selected_band_vars(1000, 0, 0);
  else if (!tact1_level && enc1 <= last_enc1_pos - 2) increment_selected_band_vars(-1000, 0, 0);
  else if (!tact1_level && enc2 >= last_enc2_pos + 2) increment_selected_band_vars(0, 1000, 0);
  else if (!tact1_level && enc2 <= last_enc2_pos - 2) increment_selected_band_vars(0, -1000, 0);
  else if (!tact1_level && enc3 >= last_enc3_pos + 2) increment_selected_band_vars(0, 0, 20);
  else if (!tact1_level && enc3 <= last_enc3_pos - 2) increment_selected_band_vars(0, 0, -20);

  // a single physical click on the encoder corresponds to an encoder value change of 2
  // so this should increas and lower the value by 1 unit
  else if (enc1 >= last_enc1_pos + 2 && enc1 < last_enc1_pos + 5) increment_selected_band_vars(1, 0, 0);
  else if (enc1 <= last_enc1_pos - 2 && enc1 > last_enc1_pos - 5) increment_selected_band_vars(-1, 0, 0);
  else if (enc2 >= last_enc2_pos + 2 && enc2 < last_enc2_pos + 5) increment_selected_band_vars(0, 1, 0);
  else if (enc2 <= last_enc2_pos - 2 && enc2 > last_enc2_pos - 5) increment_selected_band_vars(0, -1, 0);
  else if (enc3 >= last_enc3_pos + 2 && enc3 < last_enc3_pos + 5) increment_selected_band_vars(0, 0, 1);
  else if (enc3 <= last_enc3_pos - 2 && enc3 > last_enc3_pos - 5) increment_selected_band_vars(0, 0, -1);

  // when turning the encoder faster, a value of 5 seems to be comfortable to step the value by a larger increment
  else if (enc1 >= last_enc1_pos + 5) increment_selected_band_vars(100, 0, 0);
  else if (enc1 <= last_enc1_pos - 5) increment_selected_band_vars(-100, 0, 0);
  else if (enc2 >= last_enc2_pos + 5) increment_selected_band_vars(0, 100, 0);
  else if (enc2 <= last_enc2_pos - 5) increment_selected_band_vars(0, -100, 0);
  else if (enc3 >= last_enc3_pos + 5) increment_selected_band_vars(0, 0, 5);
  else if (enc3 <= last_enc3_pos - 5) increment_selected_band_vars(0, 0, -5);
}

void handle_left_press() {
  if (selected_band > 0) selected_band = selected_band - 1;
}

void handle_right_press() {
  if (selected_band < 7) selected_band = selected_band + 1;
}

void listen_for_band_nav(void) {
    // left to go to previous band
    left_press_listener(handle_left_press);
    // right to go to next band
    right_press_listener(handle_right_press);
}

// fn+enc button should set the value back to default
void handle_fn_enc1_press() {
      struct Band b = {default_band_freqs[selected_band], bands[selected_band].q, bands[selected_band].lvl };
      bands[selected_band] = b;
}

void handle_fn_enc2_press() {
      struct Band b = { bands[selected_band].freq, 10, bands[selected_band].lvl };
      bands[selected_band] = b;
}

void handle_fn_enc3_press() {
      struct Band b = { bands[selected_band].freq, bands[selected_band].q, 0 };
      bands[selected_band] = b;
}

void listen_for_fn_enc_presses(void) {
    if (!tact1_level) enc1_press_listener(handle_fn_enc1_press);
    if (!tact1_level) enc2_press_listener(handle_fn_enc2_press);
    if (!tact1_level) enc3_press_listener(handle_fn_enc3_press);
}

// draw the band data values
void band_draw() {
  if (!state_fetched) fetch_all_band_state();
  set_values_from_encoders();
  listen_for_band_nav();
  listen_for_fn_enc_presses();

  struct Band b = bands[selected_band];
  char label[16];

  snprintf(label, sizeof label, "%d HZ", b.freq);
  gfx_text(1U,  1U, label , DARK, SMALL);

  // the value of Q is stored as a whole number for simplicity
  float q_float = (float)b.q * 0.01;
  int whole = (int)q_float;
  int decimal = (int)((q_float - whole) * 100);
  snprintf(label, sizeof label, "Q: %d.%d", whole, decimal);
  gfx_text(OLED_WIDTH/2-13U,  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "%d DB", b.lvl);
  gfx_text(OLED_WIDTH-26U,  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "BAND %d/8", selected_band+1);
  gfx_text(79U, OLED_HEIGHT-7U, label, LIGHT, LARGE);
}

// // draw the spectrum visualizer scoped to band
// void spect_draw(void) {

// }
// // draw the EQ curve scoped to band
// void curve_draw(void) {

// }

void eq_draw(void) {
  band_draw();
  // spect_draw();
  // curve_draw();

  // STATIC COMPONENTS
  gfx_text(108U, 8U, "20 DB", LIGHT, SMALL);
    gfx_text(104U, 48U, "-20 DB", LIGHT, SMALL);
  gfx_text(1U, OLED_HEIGHT-7U, "EQ", LIGHT, LARGE);
  gfx_fill_rect(0U, 31, OLED_WIDTH, 1U, true);
  gfx_fill_rect((OLED_WIDTH/2), 8U, 1U, DISPLAY_AREA_HEIGHT, true);
}