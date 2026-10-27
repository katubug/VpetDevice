#include <M5Unified.h>
#include "platform/input.h"

void setup() {
  auto cfg = M5.config();
  M5.begin(cfg);
  Serial.begin(115200);

  M5.Display.fillScreen(TFT_BLACK);
  M5.Display.setTextColor(TFT_WHITE);
  M5.Display.setTextSize(2);
  M5.Display.drawString("hello from vscode!", 10, 10);
  M5.Display.drawString("tap the screen", 10, 40);
  Serial.println("core2 alive");
}

void loop() {
  M5.update(); //this needs to stay at the top of the loop for now, but we can move it later

  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    M5.Display.fillCircle(t.x, t.y, 10, TFT_PINK);
    Serial.printf("tap %d,%d\n", t.x, t.y);
  }

  delay(10);

  if (input_check_pressed(Verb::select)) {
    M5.Display.fillScreen(TFT_CADETBLUE);
    Serial.println("select");  
  }

  if (input_check_pressed(Verb::confirm)) {
    M5.Display.fillScreen(TFT_DARKCYAN);
    Serial.println("confirm");
  }

  if (input_check_pressed(Verb::cancel)){
    M5.Display.fillScreen(TFT_BLUEVIOLET);
    Serial.println("cancel");
  }
}
