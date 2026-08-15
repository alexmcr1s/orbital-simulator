#include <cmath>
#include "orbital-mechanics.h"
#include "constants.h"

double circularVelocity(double radius) {
    return sqrt(EARTH_MU / radius);
}

double vectorMagnitude(Vector2D vector) {
    return sqrt(vector.x * vector.x + vector.y * vector.y);
}

Vector2D gravitationalAcceleration(Vector2D position)
{
    double radius = vectorMagnitude(position);

    Vector2D acceleration;

    acceleration.x = - (EARTH_MU / (radius * radius * radius)) * position.x;
    acceleration.y = - (EARTH_MU / (radius * radius * radius)) * position.y;

    return acceleration;
}

void updateSpacecraft(Spacecraft& satellite, double dt) {
    Vector2D acceleration = gravitationalAcceleration(satellite.position);

    satellite.velocity.x += acceleration.x * dt;
    satellite.velocity.y += acceleration.y * dt;

    satellite.position.x += satellite.velocity.x * dt;
    satellite.position.y += satellite.velocity.y * dt;
}