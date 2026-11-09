#include "game/menu.h"
#include "platform/display.h"
#include "platform/input.h"
#include "game/scenes/rm_menu.h"
#include "game/scene_manager.h"

void RmMenu::update(){  //This is "Step"

    Scene::update();

  if (input_check_pressed(Verb::cancel)) {
    room_goto(Room::rm_main);
  }

}

void RmMenu::draw(){
    Scene::draw();  //event_inherited()
    //then, RmMenu's own stuff:
    draw_color(bg_color);
    draw_text(display_get_width()/2, display_get_height()/2, 2, text_color, Align::center, label);
}

void RmMenu::room_start(){
    //nothing here for now
}