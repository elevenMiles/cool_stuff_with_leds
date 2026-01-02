#include "Button.h"
#include <Arduino.h>

Button::Button(int _pin) : pin {_pin}
{
  pinMode(pin, (is_pullup ? INPUT_PULLUP : INPUT));
}

Button::Button(int _pin, bool isPullup) : pin {_pin}, is_pullup {isPullup}
{
  pinMode(pin, (is_pullup ? INPUT_PULLUP : INPUT));
}

Button::Button(int _pin, bool isPullup, unsigned long debounceIntervalMs) 
  : pin {_pin}, is_pullup {isPullup}, debounce_interval {debounceIntervalMs}
{
  pinMode(pin, (is_pullup ? INPUT_PULLUP : INPUT));
}

bool Button::isClicked()
{
  bool clicked {false};
  unsigned long current_time {millis()};
  if (current_time - last_check > debounce_interval)
  {
    last_check = current_time;
    int raw {digitalRead(pin)};
    int current_state {is_pullup ? !raw : raw};
    clicked = (current_state == 1 && previous_state == 0);
    previous_state = current_state;
  }
  return clicked;
}