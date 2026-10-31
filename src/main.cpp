#include <M5Unified.h>
#include "platform/input.h"
#include "platform/log.h"
#include "platform/display.h"
#include "game/palette.h"
#include "game/scene_manager.h"



void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  display_init();
  Serial.begin(115200);       //we'll remove this later when we tidy up
  room_goto(Room::rm_main);

}

void loop() {
  M5.update(); //this needs to stay at the top of the loop for now, but we can move it later

  scene_update();
  scene_draw();
  display_push();

}
