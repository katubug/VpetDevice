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