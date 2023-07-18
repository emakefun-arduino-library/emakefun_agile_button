#include <Arduino.h>

#include "agile_button.h"

#define BUTTON_PIN (5)

AgileButton button(BUTTON_PIN, LOW, INPUT_PULLUP);

void OnButtonEvent(const AgileButton::Event& event) {
  switch (event.event_type) {
    case AgileButton::kButtonDown: {
      Serial.println(F("OnButtonDown"));
      break;
    }
    case AgileButton::kButtonUp: {
      Serial.println(F("OnButtonUp"));
      break;
    }
    case AgileButton::kButtonClick: {
      Serial.print(F("OnButtonClick, click count: "));
      Serial.println(GetButtonClickCount(event));
      break;
    }
    case AgileButton::kButtonLongPressBegin: {
      Serial.println(F("OnButtonLongPressBegin"));
      break;
    }
    case AgileButton::kButtonDuringLongPress: {
      Serial.println(F("OnButtonDuringLongPress"));
      break;
    }
    case AgileButton::kButtonLongPressEnd: {
      Serial.println(F("OnButtonLongPressEnd"));
      break;
    }
    default: {
      break;
    }
  }
}

void setup() {
  Serial.begin(115200);
  button.SetEventHandler(OnButtonEvent);
  button.SetMaxDetectedClickCount(2);
}

void loop() {
  button.Tick();
}