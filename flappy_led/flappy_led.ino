// In this project using Arduino Uno we achieved Flappy Bird like movement effect on LEDs.

#include "Button.h"

Button btn {12};

const int LED_PINS[] {11, 10, 9, 6, 5, 3};
constexpr int LED_COUNT {sizeof(LED_PINS) / sizeof(int)};

float lightPosition {0.0f};
float lightVelocity {0.0f};
float gravity {-50.0f};
float jumpForce {20.0f};
float brightDistance {0.9f};

int calculatePwm(float position, float light) // Assumes max pwm value as 255
{
  float distance {abs(position - light)};
  return constrain((-255 / brightDistance) * distance + 255, 0, 255);
}

void setup() 
{
  Serial.begin(9600);

  for (int i {0}; i < LED_COUNT; ++i)
    pinMode(LED_PINS[i], OUTPUT);
}

void loop() 
{
  // Calculating elapsed time between two loop function calls
  static unsigned long previous_time_micros {};
  unsigned long current_time_micros {micros()};
  float delta_time {(current_time_micros - previous_time_micros) / 1000000.0f};
  previous_time_micros = current_time_micros;
  // ********************************************************

  if (btn.isClicked())
  {
    lightVelocity = jumpForce;
  }

  lightVelocity += gravity * delta_time;
  lightPosition += lightVelocity * delta_time;

  if (lightPosition < 0.0f)
  {
    lightPosition = 0.0f;
    lightVelocity = 0.0f;
  }
  else if (lightPosition > LED_COUNT - 1.0f)
  {
    lightPosition = LED_COUNT - 1.0f;
    lightVelocity = 0.0f;
  }

  for (int i {0}; i < LED_COUNT; ++i)
    analogWrite(LED_PINS[i], calculatePwm(i, lightPosition));
}

