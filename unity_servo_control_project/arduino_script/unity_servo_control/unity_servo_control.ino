#include <Servo.h>

constexpr int SERVO_PIN {9};

Servo servo {};

void setup()
{
  Serial.begin(9600);
  servo.attach(SERVO_PIN);
}

void loop()
{
  if (Serial.available())
  {
    String input {Serial.readStringUntil('\n')};
    input.trim();

    int angle {input.toInt()};
    servo.write(constrain(angle, 0, 180));
  }
}