#include <M5Unified.h>
#include "platform/input.h"
#include "platform/log.h"
#include "platform/display.h"
#include "game/palette.h"

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);       //we'll remove this later when we tidy up

  draw_color(c_background_blue);
  draw_text(10, 10, 2, c_font_blue, Align::topleft, "hello from vscode");
    draw_text(10, 40, 2, c_font_blue, Align::topleft, "tap the screen");
  show_debug_message("core2 alive");
}

void loop() {
  M5.update(); //this needs to stay at the top of the loop for now, but we can move it later

  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    M5.Display.fillCircle(t.x, t.y, 10, TFT_PINK);
    show_debug_message("tap %d,%d", t.x, t.y);
  }

  delay(10);

  if (input_check_pressed(Verb::select)) {
    draw_color(c_background_blue);
    draw_text(display_get_width()/2, display_get_height()/2, 2, c_font_blue, Align::center, "Select");
  }

  if (input_check_pressed(Verb::confirm)) {
    draw_color(c_pink);
    draw_text(display_get_width()/2, display_get_height()/2, 2, c_font_blue, Align::center, "Confirm");
  }

  if (input_check_pressed(Verb::cancel)){
    draw_color(c_yellow);
    draw_text(display_get_width()/2, display_get_height()/2, 2, c_pink, Align::center, "Cancel");
  }
}
