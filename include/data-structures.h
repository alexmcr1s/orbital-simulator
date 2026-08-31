#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H 1

#include <vector>


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

// For rendering purposes (future)
struct SimulationState {
    double time;
    Vector2D position;
    Vector2D velocity;
    double altitude;
    double specificEnergy;
};

struct SimulationOutput {
    SimulationResult result;
    std::vector<SimulationState> states;
    double impactTime;
};

enum class IntegratorType {
    Euler,
    Verlet,
    RK4
};

// Rate of change for position and velocity                   
struct Derivative {
    Vector2D position;
    Vector2D velocity;
};

#endif