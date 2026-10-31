#pragma once
#include <cstdint>
#include "game/palette.h"
#include "game/scenes/scene.h"

class RmInitial: public Scene{
    public:
        void update() override;
        void draw() override;

    private:
        uint32_t bg_color = c_yellow;
        uint32_t text_color = c_font_blue;
        const char* label = "Press B to Start.";

};