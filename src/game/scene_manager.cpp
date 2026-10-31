#include "game/scene_manager.h"
#include "game/scenes/rm_main.h"

static RmMain rm_main;     //creates an instance of the RmMain class, named "rm_main" - that's what i need to use in the rest of this file

static Scene* current_scene = nullptr;

static Scene* scene_for(Room room){
    switch (room)   {
        case Room::rm_main: return &rm_main;    //returns the address of the rm_main instance
    }
    return nullptr;
}

void room_goto(Room room){
    current_scene = scene_for(room);
}

void scene_update() {
    if (current_scene != nullptr){
        current_scene->update();
    }
}

void scene_draw() {
    if (current_scene != nullptr){
        current_scene->draw();
    }
}