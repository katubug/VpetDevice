#pragma once
#include <cstdint>  //library needed for using uint32_t, a type name big enough to hold a hex color

enum class Align {
    topleft,
    center
};

int display_get_width();
int display_get_height();
void draw_color(uint32_t color);
void draw_text(int x, int y, int size, uint32_t color, Align align, const char* text);
int sprite_add(const char* name); //returns a sprite index, -1 if it failed
void draw_sprite(int sprite, int subimg, int x, int y); //subimg = frame number

void display_init();
void display_push();
void sprite_delete(int sprite);