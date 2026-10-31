#include "platform/display.h"
#include "platform/input.h"
#include "game/scenes/rm_initial.h"
#include "game/scene_manager.h"

void RmInitial::update(){  //This is "Step"

    Scene::update();

  if (input_check_pressed(Verb::confirm)) {
    room_goto(Room::rm_main);
  }

}

void RmInitial::draw(){
    Scene::draw();  //event_inherited()
    //then, RmInitial's own stuff:
    draw_color(bg_color);
    draw_text(display_get_width()/2, display_get_height()/2, 2, text_color, Align::center, label);
}