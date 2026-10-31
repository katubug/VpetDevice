#include <M5Unified.h>
#include "platform/display.h"

static M5Canvas canvas(&M5.Display);     //offscreen copy of the screen to reduce flickering

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
    canvas.fillScreen(color);
}

void draw_text(int x, int y, int size, uint32_t color, Align align, const char* text){
    canvas.setTextColor(color);
    canvas.setTextSize(size);
    canvas.setTextDatum(datum_for(align));
    canvas.drawString(text, x, y);
}

void display_init(){
    canvas.createSprite(M5.Display.width(), M5.Display.height());
}

void display_push(){
    canvas.pushSprite(0, 0);
}