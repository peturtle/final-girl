#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t last_enc1_pos = 64;
uint8_t last_enc2_pos = 64; 
uint8_t last_enc3_pos = 64;

bool state_fetched = false;

#define LOWSHELF 0
#define HIGHPASS12 1 
#define HIGHPASS24 2
#define LOWPASS12 3
#define LOWPASS24 4
#define HIGHSHELF 5
#define NOTCH 6
#define BELL 7

static const char filter_type_labels[8][16] = {
  [0] = "LOW SHELF",
  [1] = "HIGH PASS 12",
  [2] = "HIGH PASS 24",
  [3] = "LOW PASS 12",
  [4] = "LOW PASS 24",
  [5] = "HIGH SHELF",
  [6] = "NOTCH",
  [7] = "BELL"
 };

static const uint8_t allowed_filter_types[8][3] = {
  [0] = { LOWSHELF, HIGHPASS12, HIGHPASS24},
  [1] = { BELL, NOTCH },
  [2] = { BELL, NOTCH },
  [3] = { BELL, NOTCH },
  [4] = { BELL, NOTCH },
  [5] = { BELL, NOTCH },
  [6] = { BELL, NOTCH },
  [7] = { HIGHSHELF, LOWPASS12, LOWPASS24}
};

struct Band {
  uint16_t freq;
  uint16_t q;
  int8_t lvl;
  uint8_t filter_type;
};

#define FREQ_LOW_LIMIT 10
#define FREQ_HI_LIMIT 22000
#define Q_LOW_LIMIT 10
#define Q_HI_LIMIT 1800
#define LVL_LOW_LIMIT -15
#define LVL_HI_LIMIT 15

static const uint16_t default_band_freqs[8] = {10, 40, 80, 100, 400, 800, 1000, 10000};

struct Band bands[8] = {
  [0] = { default_band_freqs[0], 10, 0, LOWSHELF},
  [1] = { default_band_freqs[1], 10, 0, BELL},
  [2] = { default_band_freqs[2], 10, 0, BELL},
  [3] = { default_band_freqs[3], 10, 0, BELL},
  [4] = { default_band_freqs[4], 10, 0, BELL},
  [5] = { default_band_freqs[5], 10, 0, BELL},
  [6] = { default_band_freqs[6], 10, 0, BELL},
  [7] = { default_band_freqs[7], 10, 0, HIGHSHELF}
};

uint8_t selected_band = 0;

bool overview = false;
int overview_range[2] = {FREQ_LOW_LIMIT, FREQ_HI_LIMIT}; // scroll moves the window and zoom tightens it

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

  struct Band b ={ freq, q, lvl, bands[selected_band].filter_type };
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

void cycle_filter_type(uint8_t *out) {
  uint8_t i = 0;
  uint8_t size = (selected_band == 0 || selected_band == 7 ? 2 : 1);
  uint8_t types[3];
  memcpy(types, allowed_filter_types[selected_band], 8);

  while (i < size && types[i] != bands[selected_band].filter_type) ++i;
  if (i == size) *out = types[0];
  else *out = types[i+1];
}

void handle_left_press() {
  if (selected_band > 0) selected_band = selected_band - 1;
}

void handle_right_press() {
  if (selected_band < 7) selected_band = selected_band + 1;
}

void handle_enter_press() {
  uint8_t ft;
  cycle_filter_type(&ft);
  struct Band b = {
    default_band_freqs[selected_band], 
    bands[selected_band].q, 
    bands[selected_band].lvl, 
    ft
  };
  bands[selected_band] = b;
}

// fn+enc button should set the value back to default
void handle_fn_enc1_press() {
  struct Band b = {
    default_band_freqs[selected_band], 
    bands[selected_band].q, 
    bands[selected_band].lvl, 
    bands[selected_band].filter_type
  };
  bands[selected_band] = b;
}

void handle_fn_enc2_press() {
  struct Band b = {
    bands[selected_band].freq, 
    10, 
    bands[selected_band].lvl, 
    bands[selected_band].filter_type
  };
  bands[selected_band] = b;
}

void handle_fn_enc3_press() {
  struct Band b = { 
    bands[selected_band].freq, 
    bands[selected_band].q, 
    0, 
    bands[selected_band].filter_type 
  };
  bands[selected_band] = b;
}

void listen_for_control_inputs(void) {
  // left to go to previous band
  left_press_listener(handle_left_press);
  // right to go to next band
  right_press_listener(handle_right_press);

  // enter toggles second screen 
  enter_press_listener(handle_enter_press);

  //clear values w fn enc press
  if (!tact1_level) enc1_press_listener(handle_fn_enc1_press);
  if (!tact1_level) enc2_press_listener(handle_fn_enc2_press);
  if (!tact1_level) enc3_press_listener(handle_fn_enc3_press);
}

// draw the band data values
void band_draw() {
  if (!state_fetched) fetch_all_band_state();
  set_values_from_encoders();

  struct Band b = bands[selected_band];
  char label[16];

  gfx_text(108U, 8U, "15 DB", LIGHT, SMALL);
  gfx_text(104U, 48U, "-15 DB", LIGHT, SMALL);
  gfx_fill_rect(0U, 31, OLED_WIDTH, 1U, true);
  gfx_fill_rect((OLED_WIDTH/2), 8U, 1U, DISPLAY_AREA_HEIGHT, true);

  // spect_draw(band range);
  // curve_draw(band range);

  snprintf(label, sizeof label, "%d HZ", b.freq);
  gfx_text(1U,  1U, label , DARK, SMALL);
  
  // the value of Q is stored as a whole number for simplicity
  float q_float = (float)b.q * 0.01;
  int whole = (int)q_float;
  int decimal = (int)((q_float - whole) * 100);
  snprintf(label, sizeof label, "Q: %d.%d", whole, decimal);
  gfx_text(OLED_WIDTH/2-13U,  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "%d DB", b.lvl);
  gfx_text(OLED_WIDTH-(strlen(label) * 4),  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "BAND %d/8", selected_band+1);
  gfx_text((OLED_WIDTH-(strlen(label)*8))/2 - 6, OLED_HEIGHT-7U, label, LIGHT, LARGE);

  int filter_label_width = strlen(filter_type_labels[b.filter_type]) * 4;
  gfx_fill_rect(OLED_WIDTH-(filter_label_width + 2), OLED_HEIGHT-7U, filter_label_width + 1, 8, true);
  gfx_text(OLED_WIDTH-(filter_label_width + 1), 
    OLED_HEIGHT-6U, filter_type_labels[b.filter_type], DARK, SMALL
  );
}

void overview_draw(void) {
  // spect_draw(overview_range);
  // curve_draw(overview_range);
  gfx_text(79U, OLED_HEIGHT-7U, "OVERVIEW", LIGHT, LARGE);
}

// // draw the spectrum visualizer scoped to freq range
// void spect_draw(band_range) {

// }
// // draw the EQ curve scoped to freq range
// void curve_draw(band_range) {

// }

void eq_draw(void) {
  listen_for_control_inputs();
  if (overview) {
    overview_draw();
  } else {
    band_draw();
  }
  gfx_text(1U, OLED_HEIGHT-7U, "EQ", LIGHT, LARGE);
}