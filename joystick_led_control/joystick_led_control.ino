constexpr int LED_PIN_0 {3};
constexpr int LED_PIN_1 {5};
constexpr int LED_PIN_2 {6};
constexpr int LED_PIN_3 {9};
constexpr int LED_PIN_4 {10};

constexpr int VRX {A0};

float speed {5.0f};
float lightPosition {0.0f};

int calculatePwm(float position, float light)
{
  float distance {abs(position - light)};
  return constrain((-255 / 0.5f) * distance + 255, 0, 255);
}

void setup() 
{
  pinMode(LED_PIN_0, OUTPUT);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
}

void loop() 
{
  // Calculating elapsed time between two loop function calls
  static unsigned long previous_time_micros {};
  unsigned long current_time_micros {micros()};
  float delta_time {(current_time_micros - previous_time_micros) / 1000000.0f};
  previous_time_micros = current_time_micros;
  // ********************************************************

  int rawX = analogRead(VRX);

  float direction = 0.0f;
  if (rawX > 1000)
    direction = 1.0f;
  else if (rawX < 10)
    direction = -1.0f;

  lightPosition += direction * speed * delta_time;

  if (lightPosition < 0.0f) lightPosition = 0.0f;
  else if (lightPosition > 4.0f) lightPosition = 4.0f;

  analogWrite(LED_PIN_0, calculatePwm(0.0f, lightPosition));
  analogWrite(LED_PIN_1, calculatePwm(1.0f, lightPosition));
  analogWrite(LED_PIN_2, calculatePwm(2.0f, lightPosition));
  analogWrite(LED_PIN_3, calculatePwm(3.0f, lightPosition));
  analogWrite(LED_PIN_4, calculatePwm(4.0f, lightPosition));
}


