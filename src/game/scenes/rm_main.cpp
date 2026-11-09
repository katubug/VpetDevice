#include "platform/display.h"
#include "platform/input.h"
#include "game/scenes/rm_main.h"
#include "game/scene_manager.h"
#include "platform/log.h"
#include "platform/clock.h"
#include "game/config.h"

void RmMain::update(){  //This is "Step"

    Scene::update();

    if (input_check_pressed(Verb::select)) {
    room_goto(Room::rm_menu);
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
    draw_sprite(spr_bg_kitchen, 0, 0, 0);
    int anim_frame = current_time()/ANIM_FRAME_MS;
    draw_sprite(spr_chobo_walk, anim_frame, display_get_width()/2, PET_GROUND_Y);
    draw_text(display_get_width()/2, display_get_height()/2-30, 2, text_color, Align::center, label);
}

void RmMain::room_start(){  //this resets the variables to the values in rm_main.h, we'll fix this later 
    bg_color = c_background_blue;
    label = "You started the game!";
    if (spr_chobo_walk == -1){
      spr_chobo_walk = sprite_add("spr_chobo_walk");
    }
    if (spr_bg_kitchen == -1){
      spr_bg_kitchen = sprite_add("spr_bg_kitchen");
    }
}

void RmMain::room_end(){
    sprite_delete(spr_chobo_walk);
    spr_chobo_walk = -1;
    sprite_delete(spr_bg_kitchen);
    spr_bg_kitchen = -1;
    show_debug_message("ending rm_main");
}