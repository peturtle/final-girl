// called once to set a static image in the scene buffer
void static_components(void)
{
  gfx_clear(0x00);  
  gfx_fill_rect(0U, 0U, OLED_WIDTH, 7U, true);
  gfx_fill_rect(0U, OLED_HEIGHT-10U, OLED_WIDTH, 1U, true);
  // copy the back buffer into the scene buffer
  memcpy(scene_fb, frame_buffer[back], OLED_FB_BYTES); 
}