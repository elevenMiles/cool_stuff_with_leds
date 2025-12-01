#include "FixedVector.h"
#include "Light.h"
#include "Button.h"

constexpr int maxBrightness {255};
const float speed {8.0f};
const float fadeDistance {0.8f};

constexpr int BUTTON_PIN_0 {2};
constexpr int BUTTON_PIN_1 {12};

int ledPins[] {3, 5, 6, 9, 10, 11};
constexpr int ledCount {sizeof(ledPins) / sizeof(int)};

Button btnLeft {BUTTON_PIN_0};
Button btnRight {BUTTON_PIN_1};

// Set max size to 100. Assuming there will be no light objects exist more than 100 at the same time.
FixedVector<100, Light> lights;

void setup() 
{ 
  Serial.begin(9600);

  pinMode(BUTTON_PIN_0, INPUT_PULLUP);
  pinMode(BUTTON_PIN_1, INPUT_PULLUP);

  for (int i {0}; i < ledCount; ++i)
  {
    pinMode(ledPins[i], OUTPUT);
  }
}

// sets each LED’s brightness by distance to the virtual light
int calculatePwm(float position, float light)
{
  float distance {abs(position - light)};
  return constrain((-maxBrightness / fadeDistance) * distance + maxBrightness, 0, maxBrightness);
}

void loop() 
{
  // Calculating elapsed time between two loop function calls
  static unsigned long previous_time_micros {};
  unsigned long current_time_micros {micros()};
  float delta_time {(current_time_micros - previous_time_micros) / 1000000.0f}; // Converting miliseconds to seconds by dividing 1000.
  previous_time_micros = current_time_micros;
  // ********************************************************

  if (btnLeft.isClicked())
    lights.add(Light{0.0f, speed}); // Set position to leftmost LED's position. set velocity direction to the right.

  if (btnRight.isClicked())
    lights.add(Light{ledCount - 1.0f, -speed}); // Set position to rightmost LED's position. set velocity direction to the left.

  for (int i {0}; i < lights.len(); ++i)
  {
    lights[i].movePosition(delta_time);
    float currentPosition = lights[i].position;
    if (currentPosition < -1.0f || currentPosition > ledCount)
      lights.remove(i);
  }

  for (int led {0}; led < ledCount; ++led)
  {
    int maxPwm {0};
    for (int point {0}; point < lights.len(); ++point)
    {
      int currentPwm {calculatePwm(led, lights[point].position)};
      if (currentPwm > maxPwm) 
        maxPwm = currentPwm;
    }
    analogWrite(ledPins[led], maxPwm);
  }
}