#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include "scenes/eq.c"
#include "scenes/comp.c"
#include "scenes/splash.c"
#include "scenes/debug.c"

uint8_t scene_fb[OLED_FB_BYTES]; // static frame buffer

// top-level scene list
#define SPLASH  0
#define EQ  1
#define COMP 2
#define DEBUG 3

uint8_t active_frame = SPLASH;

void handle_fn_left_press() {
  if (active_frame > 0) active_frame = active_frame - 1;
}

void handle_fn_right_press() {
  if (active_frame < 3) active_frame = active_frame + 1;
}

void listen_for_nav(void) {
    // fn left to go to previous module
    if (!tact1_level) left_press_listener(handle_fn_left_press);

    // fn right to go to next module
    if (!tact1_level) right_press_listener(handle_fn_right_press);
}

void switch_scene(void (*callback)())
{
  memcpy(frame_buffer[back], scene_fb, OLED_FB_BYTES);
  callback();
}

// the worlds most robust menu system
void frame_draw(void)
{
  listen_for_nav();
  switch (active_frame)
  {
    case SPLASH:
      switch_scene(splash_draw);
      break;
    case EQ:
      switch_scene(eq_draw);
      break;
    case COMP:
      switch_scene(comp_draw);
      break;
    case DEBUG:
      switch_scene(debug_draw);
      break;
    default: break; // draw nothing
  }
}

// called on every pass of the main loop's while(1) loop 
// renders one frame when its turn comes every 25ms abt 40fps  
uint32_t next_tick = 0U;
void scene_tick(void)
{
  if ((int32_t)(HAL_GetTick() - next_tick) < 0) return;  // not a new 25 ms frame yet
  
  next_tick += 25U;                                    // advance it 25ms 
  frame_draw();                                    // paint active frame into the back buffer
  oled_push();                                       // pushes the back buffer to the oled
}