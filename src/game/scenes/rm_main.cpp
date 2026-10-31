#include "platform/display.h"
#include "platform/input.h"
#include "game/scenes/rm_main.h"
#include "game/scene_manager.h"

void RmMain::update(){  //This is "Step"

    Scene::update();

    if (input_check_pressed(Verb::select)) {
    bg_color = c_background_blue;
    label = "Select";
  }

  if (input_check_pressed(Verb::confirm)) {
    bg_color = c_pink;
    label = "Confirm";
  }

  if (input_check_pressed(Verb::cancel)){
    room_goto(Room::rm_initial);
  }

}

void RmMain::draw(){
    Scene::draw();  //event_inherited()
    //then, RmMain's own stuff:
    draw_color(bg_color);
    draw_text(display_get_width()/2, display_get_height()/2, 2, text_color, Align::center, label);
}