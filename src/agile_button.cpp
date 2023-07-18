#include "agile_button.h"

#include <Arduino.h>

AgileButton::AgileButton(const uint8_t pin, const uint8_t active_level, const uint8_t pin_mode)
    : pin_(pin), active_level_(active_level) {
  pinMode(pin, pin_mode);
}

void AgileButton::SetDebounceDuration(const uint32_t debounce_duration) {
  debounce_duration_ = debounce_duration;
}

void AgileButton::SetMultipleClickDetectedInterval(const uint32_t interval) {
  multiple_click_detected_interval_ = interval;
}

void AgileButton::SetMaxDetectedClickCount(const uint8_t count) {
  max_click_count_ = count;
}

void AgileButton::SetEventHandler(const OnButtonEvent on_button_event, void* user_parameter) {
  on_button_event_ = on_button_event;
  user_parameter_ = user_parameter;
}

void AgileButton::SetEventHandler(const EventHandler* event_handler, void* user_parameter) {
  event_handler_ = event_handler;
  user_parameter_ = user_parameter;
}

void AgileButton::SetLongPressDetectedInterval(const uint32_t interval) {
  long_press_detected_interval_ = interval;
}

void AgileButton::Tick() {
  const auto now = millis();

  if ((digitalRead(pin_) == active_level_) == debouncing_value_) {
    if (debounce_start_time_ + debounce_duration_ <= now) {
      active_ = debouncing_value_;
    }
  } else {
    debounce_start_time_ = now;
    debouncing_value_ = !debouncing_value_;
  }

  switch (state_) {
    case kIdle: {
      if (active_) {
        state_ = kDown;
        last_down_time_ = now;
        SendEvent({kButtonDown, user_parameter_});
      }
      break;
    }

    case kDown: {
      if (!active_) {
        click_count_++;
        state_ = kCount;
        last_click_time_ = now;
        SendEvent({kButtonUp, user_parameter_});
      } else if (active_ && now - last_down_time_ >= long_press_detected_interval_) {
        state_ = kLongPress;
        last_during_long_press_notification_time_ = now;
        SendEvent({kButtonLongPressBegin, user_parameter_});
      }
      break;
    }

    case kCount: {
      if (active_) {
        state_ = kDown;
        last_down_time_ = now;
        SendEvent({kButtonDown, user_parameter_});

      } else if (click_count_ >= max_click_count_ || now - last_click_time_ >= multiple_click_detected_interval_) {
        state_ = kIdle;
        SendEvent(ButtonClickEvent(user_parameter_, click_count_));
        click_count_ = 0;
      }
      break;
    }

    case kLongPress: {
      if (!active_) {
        state_ = kIdle;
        SendEvent({kButtonUp, user_parameter_});
        SendEvent({kButtonLongPressEnd, user_parameter_});
      } else if (now - last_during_long_press_notification_time_ >= during_long_press_notification_interval_) {
        SendEvent({kButtonDuringLongPress, user_parameter_});
        last_during_long_press_notification_time_ = now;
      }
      break;
    }

    default:
      break;
  }
}

void AgileButton::SendEvent(const Event& event) {
  if (event_handler_ != nullptr) {
    event_handler_->OnButtonEvent(event);
  }

  if (on_button_event_ != nullptr) {
    on_button_event_(event);
  }
}
