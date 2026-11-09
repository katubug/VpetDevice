#pragma once
#include "game/scene_manager.h" //for the Room enum

enum class MenuAction{
    open_submenu,
    room_goto
};

struct MenuItem{
    const char* sprite_name;
    MenuAction  action; //what confirm will do
    Room        room;   //only used when action is room_goto
    bool        is_active;
};