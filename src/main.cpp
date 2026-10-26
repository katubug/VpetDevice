#include <M5Unified.h>

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
  M5.update();

  auto t = M5.Touch.getDetail();
  if (t.wasPressed()) {
    M5.Display.fillCircle(t.x, t.y, 10, TFT_PINK);
    Serial.printf("tap %d,%d\n", t.x, t.y);
  }

  delay(10);
}
