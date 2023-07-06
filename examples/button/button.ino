#include <Arduino.h>

#include "button.h"

#define BUTTON_PIN (5)

Button button(BUTTON_PIN, LOW, INPUT_PULLUP);

void setup() {
  Serial.begin(115200);
  button.SetOnClick(
      [](void* user_parameter, uint8_t click_count) {
        Serial.print(F("OnClick:"));
        Serial.println(click_count);
      },
      nullptr,
      2);

  button.SetOnButtonDown([](void*) { Serial.println(F("OnButtonDown")); });

  button.SetOnButtonUp([](void*) { Serial.println(F("OnButtonUp")); });

  button.SetOnLongPressBegin([](void*) { Serial.println(F("OnLongPressBegin")); });

  button.SetOnLongPressEnd([](void*) { Serial.println(F("OnLongPressEnd")); });

  button.SetOnDuringLongPress([](void*) { Serial.println(F("OnDuringLongPress")); }, nullptr, 100);
}

void loop() {
  button.Tick();
}