#ifndef LIGHT_H
#define LIGHT_H

class Light
{
public:
  float position {};
  float velocity {};

  Light();
  Light(float _position, float _velocity);

  void movePosition(float delta_time);
};

#endif