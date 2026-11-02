#include <M5Unified.h>
#include "platform/input.h"
#include "platform/log.h"
#include "platform/display.h"
#include "game/palette.h"
#include "game/scene_manager.h"
#include "platform/clock.h"

static int frame_count = 0;
static uint64_t fps_timer =  0;
const int TARGET_FPS = 15;
const int FRAME_MS = 1000/TARGET_FPS;

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
    Serial.begin(115200);       //we'll remove this later when we tidy up
  display_init();
  room_goto(Room::rm_initial);

}

void loop() {
  uint64_t frame_began = current_time();
  M5.update(); //this needs to stay at the top of the loop for now, but we can move it later

  scene_update();
  scene_draw();
  display_push();
  // this is for testing FPS on future devices. M5Stack Core2 runs at 18-20fps
     frame_count++;
  if (current_time()-fps_timer >= 1000){
    show_debug_message("frame count is %d", frame_count);
    frame_count = 0;
    fps_timer = current_time();
  }

  //This stays at the end
  uint64_t frame_length = current_time()-frame_began;
  if (frame_length < FRAME_MS){
    delay(FRAME_MS-frame_length);
  }

}
