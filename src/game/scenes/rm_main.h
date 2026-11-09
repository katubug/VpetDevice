#pragma once
#include <cstdint>
#include "game/palette.h"
#include "game/scenes/scene.h"

class RmMain: public Scene{
    public:
        void update() override;
        void draw() override;
        void room_start() override;
        void room_end() override;

    private:
        uint32_t bg_color = c_background_blue;
        uint32_t text_color = c_font_blue;
        const char* label = "You started the game!";
        int spr_chobo_base = -1;
        int spr_chobo_walk = -1;
        int spr_bg_kitchen = -1;

};

