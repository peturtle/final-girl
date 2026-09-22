#include "main.h"

void  splash_draw(void)
{
  gfx_text(1U,  1U, "FINAL GIRL V0.0", DARK, SMALL);

  gfx_text(7U, 15U, "PRESS FN+RIGHT OR FN+LEFT", LIGHT, SMALL);
  gfx_text(7U, 22U, "TO NAVIGATE BETWEEN MODULES", LIGHT, SMALL);
  gfx_text(7U, 30U, "HAVE FUN <3", LIGHT, LARGE);
  

  gfx_text(1U, OLED_HEIGHT-7U, "TRASH ELECTRONICS", LIGHT, LARGE);
}