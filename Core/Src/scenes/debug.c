void debug_draw() {

      gfx_text(1U, OLED_HEIGHT-7U, "DEBUG", LIGHT, LARGE);

  uint8_t enc1 = enc1_position >> 1U;
  uint8_t enc2 = enc2_position >> 1U;
  uint8_t enc3 = enc3_position >> 1U;
  uint8_t enc4 = enc4_position >> 1U;

  char result[64];

//   PRINT ENC VALUES
  snprintf(result, sizeof result, "ENC 1: %d", enc1);
  gfx_text(1U,  10U, result, LIGHT, SMALL);

  snprintf(result, sizeof result, "ENC 2: %d", enc2);
  gfx_text(1U,  16U, result, LIGHT, SMALL);

  snprintf(result, sizeof result, "ENC 3: %d", enc3);
  gfx_text(1U,  21U, result, LIGHT, SMALL);

  snprintf(result, sizeof result, "ENC 4: %d", enc4);
  gfx_text(1U,  27U, result, LIGHT, SMALL);
}