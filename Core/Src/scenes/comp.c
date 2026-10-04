#include "main.h"

void  comp_draw(void) {
  // CONTROLS
  char label[16];

  snprintf(label, sizeof label, "THR:%d DB", -20);
  gfx_text(1U,  1U, label , DARK, SMALL);
  
  snprintf(label, sizeof label, "RAT:%d:%d", 2, 1);
  gfx_text(OLED_WIDTH/2-13U,  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "OUT:%d DB", 0);
  gfx_text(OLED_WIDTH-(strlen(label) * 4),  1U, label, DARK, SMALL);

  snprintf(label, sizeof label, "PAGE %d/2", 1);
  gfx_text((OLED_WIDTH-48), OLED_HEIGHT-7U, label, LIGHT, LARGE);


  // STATIC COMPONENTS
  gfx_text(1U, OLED_HEIGHT-7U, "COMP", LIGHT, LARGE);

  gfx_fill_rect(26, 10, 84, 10, true);
  gfx_fill_rect(27, 11, 82, 8, false);
  gfx_text(OLED_WIDTH - 15, 13, "IN", LIGHT, SMALL);

  gfx_fill_rect(26, 22, 84, 10, true);
  gfx_fill_rect(27, 23, 82, 8, false);
  gfx_text(OLED_WIDTH - 15, 25, "GR", LIGHT, SMALL);

  gfx_fill_rect(26, 34, 84, 10, true);
  gfx_fill_rect(27, 35, 82, 8, false);
  gfx_text(OLED_WIDTH - 15, 37, "OUT", LIGHT, SMALL);

  // METERS
  gfx_fill_rect(26, 12, 68, 6, true); // input meter
  gfx_fill_rect(68, 24, 34, 6, true); // gain reduction meter
  gfx_fill_rect(26, 36, 80, 6, true); // output meter

  // PEAKS
  gfx_text(1, 13, "-7.16", LIGHT, SMALL);
  gfx_text(1, 25, "-24.24", LIGHT, SMALL);
  // if peaking invert the color or maybe flash?
  gfx_fill_rect(0, 36, 22, 7, true);
  gfx_text(1, 37, "+6.57", DARK, SMALL);

  // REDLINING
  gfx_fill_rect(OLED_WIDTH-26, 13, 5, 4, false);
  gfx_fill_rect(OLED_WIDTH-26, 25, 5, 4, false);
  gfx_fill_rect(OLED_WIDTH-26, 37, 5, 4, false);
  gfx_fill_rect(OLED_WIDTH-26, 10, 1U, 10, true); // 0 DB
  gfx_fill_rect(OLED_WIDTH-26, 22, 1U, 10, true); // 0 DB
  gfx_fill_rect(OLED_WIDTH-26, 34, 1U, 12, true); // 0 DB
  gfx_fill_rect(OLED_WIDTH/2, 44, 1U, 2, true); // -24 DB
  gfx_fill_rect(26, 44, 1U, 2, true); // -60 DB
  gfx_text(OLED_WIDTH - 27, 47, "0 DB", LIGHT, SMALL);
  gfx_text(OLED_WIDTH/2-7, 47, "-24", LIGHT, SMALL);
  gfx_text(19, 47, "-60", LIGHT, SMALL);
}