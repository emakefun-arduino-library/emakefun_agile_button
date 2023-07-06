#include <Arduino.h>
#include <stdint.h>

/**
 * @brief A class representing a button.
 */
class Button {
 public:
  /**
   * @brief Function type for the button down event.
   * @param user_parameter A user-defined parameter passed to the button down event handler.
   */
  using OnButtonDown = void(void* user_parameter);

  /**
   * @brief Function type for the button up event.
   * @param user_parameter A user-defined parameter passed to the button up event handler.
   */
  using OnButtonUp = void(void* user_parameter);

  /**
   * @brief Function type for the click event.
   * @param user_parameter A user-defined parameter passed to the click event handler.
   * @param click_count The number of clicks detected.
   */
  using OnClick = void(void* user_parameter, uint8_t click_count);

  /**
   * @brief Function type for the long press begin event.
   * @param user_parameter A user-defined parameter passed to the long press begin event handler.
   */
  using OnLongPressBegin = void(void* user_parameter);

  /**
   * @brief Function type for the during long press event.
   * @param user_parameter A user-defined parameter passed to the during long press event handler.
   */
  using OnDuringLongPress = void(void* user_parameter);

  /**
   * @brief Function type for the long press end event.
   * @param user_parameter A user-defined parameter passed to the long press end event handler.
   */
  using OnLongPressEnd = void(void* user_parameter);

  /**
   * @brief Constructs a Button object.
   * @param pin The pin number to which the button is connected.
   * @param active_level The active level of the button (defaults to LOW).
   * @param pin_mode The pin mode of the button (defaults to INPUT_PULLUP).
   */
  explicit Button(const uint8_t pin, const uint8_t active_level = LOW, const uint8_t pin_mode = INPUT_PULLUP);

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

  /**
   * @brief Sets the interval for long press detection.
   * @param interval The interval for long press detection in milliseconds.
   */
  void SetLongPressDetectedInterval(const uint32_t interval);

  /**
   * @brief Sets the callback function for the button down event.
   * @param on_button_down The function to be called when the button is pressed down.
   * @param user_parameter A user-defined parameter passed to the button down event handler (defaults to nullptr).
   */
  void SetOnButtonDown(const OnButtonDown on_button_down, void* const user_parameter = nullptr);

  /**
   * @brief Sets the callback function for the button up event.
   * @param on_button_up The function to be called when the button is released up.
   * @param user_parameter A user-defined parameter passed to the button up event handler (defaults to nullptr).
   */
  void SetOnButtonUp(const OnButtonUp on_button_up, void* const user_parameter = nullptr);

  /**
   * @brief Sets the callback function for the click event.
   * @param on_click The function to be called when a click is detected.
   * @param user_parameter A user-defined parameter passed to the click event handler (defaults to nullptr).
   * @param max_detected_click_count The maximum number of clicks to detect (defaults to 1).
   */
  void SetOnClick(const OnClick on_click, void* const user_parameter = nullptr, const uint8_t max_detected_click_count = 1);

  /**
   * @brief Sets the callback function for the long press begin event.
   * @param on_long_press_begin The function to be called when a long press begins.
   * @param user_parameter A user-defined parameter passed to the long press begin event handler (defaults to nullptr).
   */
  void SetOnLongPressBegin(const OnLongPressBegin on_long_press_begin, void* const user_parameter = nullptr);

  /**
   * @brief Sets the callback function for the during long press event.
   * @param on_during_long_press The function to be called during a long press.
   * @param user_parameter A user-defined parameter passed to the during long press event handler (defaults to nullptr).
   * @param during_long_press_notification_interval The interval between during long press notifications in milliseconds
   * (defaults to 0).
   */
  void SetOnDuringLongPress(const OnDuringLongPress on_during_long_press,
                            void* const user_parameter = nullptr,
                            const uint32_t during_long_press_notification_interval = 0);

  /**
   * @brief Sets the callback function for the long press end event.
   * @param on_long_press_end The function to be called when a long press ends.
   * @param user_parameter A user-defined parameter passed to the long press end event handler (defaults to nullptr).
   */
  void SetOnLongPressEnd(const OnLongPressEnd on_long_press_end, void* const user_parameter = nullptr);

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

  /**
   * @brief Performs button debouncing.
   * @param value The current level of the button (HIGH or LOW)
   * @param now The current time in milliseconds.
   * @return Debounced button level.
   */
  uint8_t Debounce(const uint8_t value, const uint32_t now);

  const uint8_t pin_ = 0;            /**< The pin number to which the button is connected. */
  const uint8_t active_level_ = LOW; /**< The active level of the button. */

  State state_ = kIdle;          /**< The current state of the button. */
  uint32_t last_down_time_ = 0;  /**< The timestamp when the button was pressed down. */
  uint32_t last_click_time_ = 0; /**< The timestamp when the last click was detected. */
  uint32_t last_during_long_press_notification_time_ =
      0; /**< The timestamp when the last during long press notification was sent. */
  uint32_t during_long_press_notification_interval_ = 0; /**< The interval between during long press notifications. */
  uint32_t long_press_detected_interval_ = 800;          /**< The interval for long press detection. */
  uint32_t multiple_click_detected_interval_ = 400;      /**< The interval for multiple click detection. */
  uint8_t click_count_ = 0;                              /**< The number of clicks detected. */
  uint8_t last_pin_level_ = LOW;                         /**< The last level of the button pin. */
  uint8_t last_debounced_pin_level_ = LOW;               /**< The last debounced level of the button pin. */
  uint32_t last_debounce_time_ = 0;                      /**< The timestamp when the last debounce occurred. */
  uint8_t debounce_duration_ = 50;                       /**< The duration for button debouncing in milliseconds. */
  uint8_t max_click_count_ = 1;                          /**< The maximum number of clicks to detect. */

  OnButtonDown* on_button_down_ = nullptr;              /**< Pointer to the button down event handler function. */
  OnButtonUp* on_button_up_ = nullptr;                  /**< Pointer to the button up event handler function. */
  OnClick* on_click_ = nullptr;                         /**< Pointer to the click event handler function. */
  OnLongPressBegin* on_long_press_begin_ = nullptr;     /**< Pointer to the long press begin event handler function. */
  OnDuringLongPress* on_during_long_press_ = nullptr;   /**< Pointer to the during long press event handler function. */
  OnLongPressEnd* on_long_press_end_ = nullptr;         /**< Pointer to the long press end event handler function. */
  void* on_button_down_user_parameter_ = nullptr;       /**< User-defined parameter for the button down event handler. */
  void* on_button_up_user_parameter_ = nullptr;         /**< User-defined parameter for the button up event handler. */
  void* on_click_user_parameter_ = nullptr;             /**< User-defined parameter for the click event handler. */
  void* on_long_press_begin_user_parameter_ = nullptr;  /**< User-defined parameter for the long press begin event handler. */
  void* on_during_long_press_user_parameter_ = nullptr; /**< User-defined parameter for the during long press event handler. */
  void* on_long_press_end_user_parameter_ = nullptr;    /**< User-defined parameter for the long press end event handler. */
};

/**
 * @brief a simple example
 * @example button.ino
 */