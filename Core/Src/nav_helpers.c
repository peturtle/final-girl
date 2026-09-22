#include "main.h"

bool l_p = false; 
bool r_p = false;
bool enc1_p = false;
bool enc2_p = false;
bool enc3_p = false;

void left_press_listener(void (*callback)()) {
    if (!tact2_level && !l_p) callback();
    l_p = !tact2_level;
}

void right_press_listener(void (*callback)()) {
    if (!tact3_level && !r_p) callback();
    r_p = !tact3_level;
}

void enc1_press_listener(void (*callback)()) {
    if (!enc1_sw_level && !enc1_p) callback();
    enc1_p = !enc1_sw_level;
}
void enc2_press_listener(void (*callback)()) {
    if (!enc2_sw_level && !enc2_p) callback();
    enc2_p = !enc2_sw_level;
}
void enc3_press_listener(void (*callback)()) {
    if (!enc3_sw_level && !enc3_p) callback();
    enc3_p = !enc3_sw_level;
}