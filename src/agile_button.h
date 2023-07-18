#pragma once

#include <Arduino.h>
#include <stdint.h>

/**
 * @brief A class representing a button.
 */
class AgileButton {
 public:
  enum EventType {
    kButtonDown,
    kButtonUp,
    kButtonClick,
    kButtonLongPressBegin,
    kButtonDuringLongPress,
    kButtonLongPressEnd,
  };

  struct Event {
    EventType event_type;
    void* user_parameter;
  };

  struct ButtonClickEvent : public Event {
    ButtonClickEvent(void* user_parameter, uint8_t click_count)
        : Event({kButtonClick, user_parameter}), click_count(click_count) {
    }
    uint8_t click_count;
  };

  using OnButtonEvent = void(const Event& event);

  class EventHandler {
   public:
    virtual void OnButtonEvent(const Event& event) = 0;
  };

  /**
   * @brief Constructs a AgileButton object.
   * @param pin The pin number to which the button is connected.
   * @param active_level The active level of the button (defaults to LOW).
   * @param pin_mode The pin mode of the button (defaults to INPUT_PULLUP).
   */
  explicit AgileButton(const uint8_t pin, const uint8_t active_level = LOW, const uint8_t pin_mode = INPUT_PULLUP);

  virtual ~AgileButton() = default;

  /**
   * @brief Sets the duration for button debouncing.
   * @param debounce_duration The duration for button debouncing in milliseconds.
   */
  void SetDebounceDuration(const uint32_t debounce_duration);

  /**
   * @brief Sets the interval for multiple click detection.
   * @param interval The interval for multiple click detection in milliseconds.
   */
  void SetMultipleClickDetectedInterval(const uint32_t interval);

  void SetMaxDetectedClickCount(const uint8_t count);

  void SetEventHandler(const OnButtonEvent on_event, void* user_parameter = nullptr);

  void SetEventHandler(const EventHandler* event_handler, void* user_parameter = nullptr);

  /**
   * @brief Sets the interval for long press detection.
   * @param interval The interval for long press detection in milliseconds.
   */
  void SetLongPressDetectedInterval(const uint32_t interval);
  /**
   * @brief Performs a button tick.
   * This function should be called periodically in the main loop to perform button state detection and event handling.
   */
  void Tick();

 private:
  /**
   * @brief Enum representing the possible states of the button.
   */
  enum State : uint8_t {
    kIdle,         /**< Button is in idle state */
    kDown,         /**< Button is pressed down */
    kUp,           /**< Button is released up */
    kCount,        /**< Button click count state */
    kLongPress,    /**< Button long press state */
    kLongPressEnd, /**< Button long press end state */
  };

  void SendEvent(const Event& event);

  const uint8_t pin_ = 0;            /**< The pin number to which the button is connected. */
  const uint8_t active_level_ = LOW; /**< The active level of the button. */
  bool active_ = false;
  State state_ = kIdle;          /**< The current state of the button. */
  uint32_t last_down_time_ = 0;  /**< The timestamp when the button was pressed down. */
  uint32_t last_click_time_ = 0; /**< The timestamp when the last click was detected. */
  uint32_t last_during_long_press_notification_time_ =
      0; /**< The timestamp when the last during long press notification was sent. */
  uint32_t during_long_press_notification_interval_ = 100; /**< The interval between during long press notifications. */
  uint32_t long_press_detected_interval_ = 800;            /**< The interval for long press detection. */
  uint32_t multiple_click_detected_interval_ = 400;        /**< The interval for multiple click detection. */
  uint8_t click_count_ = 0;                                /**< The number of clicks detected. */
  uint32_t debounce_start_time_ = 0;
  bool debouncing_value_ = false;
  uint8_t debounce_duration_ = 50; /**< The duration for button debouncing in milliseconds. */
  uint8_t max_click_count_ = 1;    /**< The maximum number of clicks to detect. */

  void* user_parameter_ = nullptr;
  OnButtonEvent* on_button_event_ = nullptr;
  EventHandler* event_handler_ = nullptr;
};

namespace {
inline uint8_t GetButtonClickCount(const AgileButton::Event& event) {
  return reinterpret_cast<const AgileButton::ButtonClickEvent&>(event).click_count;
}
}  // namespace

/**
 * @brief a simple example
 * @example button.ino
 */