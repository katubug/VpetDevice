#pragma once
#include <cstdint>
#include "game/palette.h"
#include "game/scenes/scene.h"

class RmMenu: public Scene{
    public:
        void update() override;
        void draw() override;
        void room_start() override;

    private:
        uint32_t bg_color = c_pink;
        uint32_t text_color = c_font_blue;
        const char* label = "This is a menu.";

};