#pragma once

enum class Room{
    rm_initial,
    rm_main,
    rm_menu
};

void room_goto(Room room);
void scene_update();
void scene_draw();