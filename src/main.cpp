#include <M5Unified.h>
#include "platform/input.h"
#include "platform/log.h"
#include "platform/display.h"
#include "game/palette.h"
#include "game/scenes/rm_main.h"

RmMain rm_main;     //creates an instance of the RmMain class, named "rm_main" - that's what i need to use in the rest of this file

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  display_init();
  Serial.begin(115200);       //we'll remove this later when we tidy up

}

void loop() {
  M5.update(); //this needs to stay at the top of the loop for now, but we can move it later

  rm_main.update();
  rm_main.draw();
  display_push();

}
