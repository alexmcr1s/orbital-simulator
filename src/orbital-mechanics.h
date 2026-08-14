#ifndef ORBITAL_MECHANICS_H
#define ORBITAL_MECHANICS_H

#include "data-structures.h"

// Calculates circular orbital velocity at a given orbital radius
double circularVelocity(double radius);

// Calculates dynamic position of spacecraft
double vectorMagnitude(Vector2D vector);

// Calculates gravitational acceleration at a given position
Vector2D gravitationalAcceleration(Vector2D position);

#endif

