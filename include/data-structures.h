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

// Enum for trajectory
enum class TrajectoryType {
    Circular,
    Elliptical,
    Parabolic,
    Hyperbolic
};

// Enum for result
enum class SimulationResult {
    Orbit,
    Impact,
    Escape
};

#endif