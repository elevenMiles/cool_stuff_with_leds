#include "Light.h"

Light::Light()
{
}

Light::Light(float _position, float _velocity) : position {_position}, velocity {_velocity}
{
}

void Light::movePosition(float delta_time)
{
  position += velocity * delta_time;
}