#pragma once
#include <cstdint>
#include "game/palette.h"

class RmMain{
    public:
        void update();
        void draw();

    private:
        uint32_t bg_color = c_background_blue;
        uint32_t text_color = c_font_blue;
        const char* label = "Hello!";

};