#ifndef BUTTON_H
#define BUTTON_H

class Button
{
private: 
  bool is_pullup {true};
  unsigned long debounce_interval {20}; // miliseconds
  unsigned long last_check {0};
  int previous_state {0};
  int pin {};

public:
  Button(int _pin);
  Button(int _pin, bool isPullup);
  Button(int _pin, bool isPullup, unsigned long debounceIntervalMs);

  bool isClicked();
};

#endif