#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H 1


// A 2D vector used for both position and velocity
struct Vector2D {
    double x;
    double y;
};

// Current state of spacecraft
struct Spacecraft {
    Vector2D position;
    Vector2D velocity;
};

#endif