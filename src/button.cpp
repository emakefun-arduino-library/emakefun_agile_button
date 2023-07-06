#include "button.h"

#include <Arduino.h>

Button::Button(const uint8_t pin, const uint8_t active_level, const uint8_t pin_mode) : pin_(pin), active_level_(active_level) {
  pinMode(pin, pin_mode);
  last_pin_level_ = last_debounced_pin_level_ = active_level == HIGH ? LOW : HIGH;
}

void Button::SetDebounceDuration(const uint32_t debounce_duration) {
  debounce_duration_ = debounce_duration;
}

void Button::SetMultipleClickDetectedInterval(const uint32_t interval) {
  multiple_click_detected_interval_ = interval;
}

void Button::SetLongPressDetectedInterval(const uint32_t interval) {
  long_press_detected_interval_ = interval;
}

void Button::SetOnButtonDown(const OnButtonDown on_button_down, void* const user_parameter) {
  on_button_down_ = on_button_down;
  on_button_down_user_parameter_ = user_parameter;
}

void Button::SetOnButtonUp(const OnButtonUp on_button_up, void* const user_parameter) {
  on_button_up_ = on_button_up;
  on_button_up_user_parameter_ = user_parameter;
}

void Button::SetOnClick(const OnClick on_click, void* const user_parameter, const uint8_t max_detected_click_count = 1) {
  on_click_ = on_click;
  on_button_down_user_parameter_ = user_parameter;
  max_click_count_ = max(1, max_detected_click_count);
}

void Button::SetOnLongPressBegin(const OnLongPressBegin on_long_press_begin, void* const user_parameter) {
  on_long_press_begin_ = on_long_press_begin;
  on_long_press_begin_user_parameter_ = user_parameter;
}

void Button::SetOnDuringLongPress(const OnDuringLongPress on_during_long_press,
                                  void* const user_parameter,
                                  const uint32_t during_long_press_notification_interval) {
  on_during_long_press_ = on_during_long_press;
  on_during_long_press_user_parameter_ = user_parameter;
  during_long_press_notification_interval_ = during_long_press_notification_interval;
}

void Button::SetOnLongPressEnd(const OnLongPressEnd on_long_press_end, void* const user_parameter) {
  on_long_press_end_ = on_long_press_end;
  on_long_press_begin_user_parameter_ = user_parameter;
}

void Button::Tick() {
  const auto now = millis();
  const bool active = Debounce(digitalRead(pin_), now) == active_level_;

  switch (state_) {
    case kIdle: {
      if (active) {
        state_ = kDown;
        last_down_time_ = now;
        if (on_button_down_ != nullptr) {
          on_button_down_(on_button_down_user_parameter_);
        }
      }
      break;
    }

    case kDown: {
      if (!active) {
        click_count_++;
        state_ = kCount;
        last_click_time_ = now;
        if (on_button_up_ != nullptr) {
          on_button_up_(on_button_up_user_parameter_);
        }
      } else if (active && now - last_down_time_ >= long_press_detected_interval_) {
        state_ = kLongPress;
        last_during_long_press_notification_time_ = now;
        if (on_long_press_begin_ != nullptr) {
          on_long_press_begin_(on_long_press_begin_user_parameter_);
        }
      }
      break;
    }

    case kCount: {
      if (active) {
        state_ = kDown;
        last_down_time_ = now;
        if (on_button_down_ != nullptr) {
          on_button_down_(on_button_down_user_parameter_);
        }
      } else if (click_count_ >= max_click_count_ || now - last_click_time_ >= multiple_click_detected_interval_) {
        state_ = kIdle;
        if (on_click_ != nullptr) {
          on_click_(on_click_user_parameter_, click_count_);
        }
        click_count_ = 0;
      }
      break;
    }

    case kLongPress: {
      if (!active) {
        state_ = kIdle;
        if (on_button_up_ != nullptr) {
          on_button_up_(on_button_up_user_parameter_);
        }
        if (on_long_press_end_ != nullptr) {
          on_long_press_end_(on_long_press_begin_user_parameter_);
        }
      } else if (now - last_during_long_press_notification_time_ >= during_long_press_notification_interval_) {
        if (on_during_long_press_ != nullptr) {
          on_during_long_press_(on_during_long_press_user_parameter_);
        }
        last_during_long_press_notification_time_ = now;
      }
      break;
    }

    default:
      break;
  }
}

uint8_t Button::Debounce(const uint8_t value, const uint32_t now) {
  if (value == last_pin_level_) {
    if (now - last_debounce_time_ >= debounce_duration_) {
      last_debounced_pin_level_ = value;
    }
  } else {
    last_debounce_time_ = now;
    last_pin_level_ = value;
  }

  return last_debounced_pin_level_;
}
