#pragma once

enum class Room{
    rm_main
};

void room_goto(Room room);
void scene_update();
void scene_draw();