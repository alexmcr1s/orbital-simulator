#ifndef SIMULATION_H
#define SIMULATION_H

#include "constants.h"
#include "data-structures.h"

#include <cstddef>

enum class SimulationRunStatus {
    Completed,
    InvalidConfiguration,
    StepLimitReached
};

enum class SimulationValidationError {
    None,
    InvalidAltitude,
    InvalidVelocityMultiplier,
    InvalidLaunchAngle,
    InvalidIntegrator,
    InvalidOrbitCount,
    InvalidTimeStep,
    InvalidEscapeLimit,
    InvalidMaxIntegrationSteps,
    InvalidStateSampleStride
};

// Inputs required to run one simulation. A console UI, GUI, or test can build
// this structure without depending on any input/output code.
struct SimulationConfig {
    double altitudeKm = 0.0;
    double velocityMultiplier = 1.0;
    double launchAngleDeg = 0.0;
    IntegratorType integrator = IntegratorType::Euler;
    int numberOfOrbits = 1;
    double timeStep = dt;
    double escapeLimit = ESCAPE_LIMIT;
    std::size_t maxIntegrationSteps = 1'000'000;
    std::size_t stateSampleStride = 1;
};

// Values calculated for a completed simulation. Presentation layers can use
// this report to render a console summary, GUI, CSV file, or other format.
struct SimulationReport {
    SimulationRunStatus status = SimulationRunStatus::Completed;
    SimulationValidationError validationError = SimulationValidationError::None;
    Spacecraft initialSpacecraft;
    Spacecraft finalSpacecraft;
    SimulationOutput simulation;

    TrajectoryType trajectoryType;
    double initialSpeed;
    double initialEnergy;
    double angularMomentum;
    double eccentricity;
    double eVecMagnitude;
    double periapsisAngleDeg;
    bool hasDefinedPeriapsisDirection;
    double semiMajorAxisVal;
    double periapsisAltitudeKm;
    double apoapsisAltitudeKm;
    double orbitalPeriod;
    double finalEnergy;
    double energyError;
    double relativeEnergyError;
    double positionError;
    double impactPercent;
};

// Runs the physics and produces data only; it performs no console or file I/O.
SimulationReport runSimulation(const SimulationConfig& config);

#endif
