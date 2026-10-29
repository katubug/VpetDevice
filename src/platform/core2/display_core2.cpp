#include <M5Unified.h>
#include "platform/display.h"

static textdatum_t datum_for(Align align){
    switch (align) {
        case Align::center:  return textdatum_t::middle_center;
        case Align::topleft: return textdatum_t::top_left;
    }
    return textdatum_t::top_left;
}

int display_get_width(){
    return M5.Display.width();
}

int display_get_height(){
    return M5.Display.height();
}

void draw_color(uint32_t color){
    M5.Display.fillScreen(color);
}

void draw_text(int x, int y, int size, uint32_t color, Align align, const char* text){
    M5.Display.setTextColor(color);
    M5.Display.setTextSize(size);
    M5.Display.setTextDatum(datum_for(align));
    M5.Display.drawString(text, x, y);
}