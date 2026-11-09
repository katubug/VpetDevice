#include "game/scene_manager.h"
#include "game/scenes/rm_main.h"
#include "game/scenes/rm_initial.h"
#include "game/scenes/rm_menu.h"

static RmInitial rm_initial;

static RmMenu rm_menu;

static RmMain rm_main;     //creates an instance of the RmMain class, named "rm_main" - that's what i need to use in the rest of this file

static Scene* current_scene = nullptr;

static Scene* scene_for(Room room){
    switch (room)   {
        case Room::rm_main: return &rm_main;    //returns the address of the rm_main instance
        case Room::rm_initial: return &rm_initial;
        case Room::rm_menu: return &rm_menu;
    }
    return nullptr;
}

void room_goto(Room room){
    if (current_scene != nullptr) current_scene->room_end();    // ends the room we're currently in.
    current_scene = scene_for(room); //sets the room we're entering
    if (current_scene != nullptr) current_scene->room_start();  //resets to the room we entered
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