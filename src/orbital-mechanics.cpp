#include "orbital-mechanics.h"
#include "constants.h"

#include <cmath>
#include <fstream>


void initializeSpacecraft(Spacecraft& satellite, double radius) {
    satellite.position.x = radius;
    satellite.position.y = 0;
}

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

void updateSpacecraftVerlet(Spacecraft& satellite, double dt) {
    Vector2D oldAcceleration = gravitationalAcceleration(satellite.position);

    satellite.position.x += satellite.velocity.x * dt + 0.5 * oldAcceleration.x * dt * dt;
    satellite.position.y += satellite.velocity.y * dt + 0.5 * oldAcceleration.y * dt * dt;

    Vector2D newAcceleration = gravitationalAcceleration(satellite.position);

    satellite.velocity.x += 0.5 * (oldAcceleration.x + newAcceleration.x) * dt;
    satellite.velocity.y += 0.5 * (oldAcceleration.y + newAcceleration.y) * dt;
}

void updateSpacecraftRK4(Spacecraft& satellite, double dt) {
    Derivative k1 = evaluateDerivative(satellite);

    Spacecraft temp = satellite;

    temp.position.x += k1.position.x * dt * 0.5;
    temp.position.y += k1.position.y * dt * 0.5;

    temp.velocity.x += k1.velocity.x * dt * 0.5;
    temp.velocity.y += k1.velocity.y * dt * 0.5;

    Derivative k2 = evaluateDerivative(temp);

    temp = satellite;

    temp.position.x += k2.position.x * dt * 0.5;
    temp.position.y += k2.position.y * dt * 0.5;

    temp.velocity.x += k2.velocity.x * dt * 0.5;
    temp.velocity.y += k2.velocity.y * dt * 0.5;

    Derivative k3 = evaluateDerivative(temp);

    temp = satellite;

    temp.position.x += k3.position.x * dt;
    temp.position.y += k3.position.y * dt;

    temp.velocity.x += k3.velocity.x * dt;
    temp.velocity.y += k3.velocity.y * dt;

    Derivative k4 = evaluateDerivative(temp);

    satellite.position.x +=
                (dt / 6.0) * (k1.position.x + 2.0 * k2.position.x
                + 2.0 * k3.position.x + k4.position.x);

    satellite.position.y +=
                (dt / 6.0) * (k1.position.y + 2.0 * k2.position.y
                + 2.0 * k3.position.y + k4.position.y);
    
    satellite.velocity.x +=
                (dt / 6.0) * (k1.velocity.x + 2.0 * k2.velocity.x
                + 2.0 * k3.velocity.x + k4.velocity.x);

    satellite.velocity.y +=
                (dt / 6.0) * (k1.velocity.y + 2.0 * k2.velocity.y
                + 2.0 * k3.velocity.y + k4.velocity.y);
}

double specificOrbitalEnergy(const Spacecraft& satellite, double speed, double radius) {
    return (speed * speed) / 2.0 - EARTH_MU / radius;
}

double spacecraftSpeed(const Spacecraft& satellite){
    return sqrt(
            satellite.velocity.x * satellite.velocity.x +
            satellite.velocity.y * satellite.velocity.y
        );
}

double specificAngularMomentum(const Spacecraft& satellite) {
    return satellite.position.x * satellite.velocity.y 
             - satellite.position.y * satellite.velocity.x;
}

double orbitalEccentricity(double specificEnergy, double angularMomentum) {
    double eccentricitySquared =
        1 + (2 * specificEnergy * angularMomentum * angularMomentum)
        / (EARTH_MU * EARTH_MU);

    if (eccentricitySquared < 0 && eccentricitySquared > -1e-10) {
        eccentricitySquared = 0;
    }

    return sqrt(eccentricitySquared);
}

Vector2D eccentricityVector(const Spacecraft& satellite) {
    double r = sqrt(
        satellite.position.x * satellite.position.x +
        satellite.position.y * satellite.position.y
    );

    double vSquared =
        satellite.velocity.x * satellite.velocity.x +
        satellite.velocity.y * satellite.velocity.y;

    double rDotV =
        satellite.position.x * satellite.velocity.x +
        satellite.position.y * satellite.velocity.y;

    Vector2D eVector;

    eVector.x =
        ((vSquared - EARTH_MU / r) * satellite.position.x
        - rDotV * satellite.velocity.x)
        / EARTH_MU;

    eVector.y = 
        ((vSquared - EARTH_MU / r) * satellite.position.y
        - rDotV * satellite.velocity.y)
        / EARTH_MU;

    return eVector;
}

double semiMajorAxis(double specificEnergy) {
    return -EARTH_MU / (2.0 * specificEnergy);
}

double periapsisRadius(double semiMajorAxis, double eccentricity) {
    return semiMajorAxis * (1.0 - eccentricity);
}

double apoapsisRadius(double semiMajorAxis, double eccentricity) {
    return semiMajorAxis * (1.0 + eccentricity);
}

SimulationOutput simulateOrbit(Spacecraft& satellite, double orbitalPeriod, double dt, IntegratorType integrator, int numberOfOrbits) {
    SimulationOutput output;
    output.impactTime = -1.0;
    double simTime = 0.0;
    double simDuration = orbitalPeriod * numberOfOrbits;

    while (simTime < simDuration) {
        double remainingTime = simDuration - simTime;
        double currentDt = dt;

        if (remainingTime < dt) { currentDt = remainingTime; }

        switch (integrator) {
            case IntegratorType::Euler:
                updateSpacecraft(satellite, currentDt);
                break;
            case IntegratorType::Verlet:
                updateSpacecraftVerlet(satellite, currentDt);
                break;
            case IntegratorType::RK4:
                updateSpacecraftRK4(satellite, currentDt);
                break;
        }

        simTime += currentDt;

        double currentRadius = sqrt(
            satellite.position.x * satellite.position.x +
            satellite.position.y * satellite.position.y
        );

        double altitudeKm = (currentRadius - EARTH_RADIUS) / 1000.0;

        SimulationState state;

        state.time = simTime;
        state.position = satellite.position;
        state.velocity = satellite.velocity;
        state.altitude = altitudeKm;

        output.states.push_back(state);

        if (currentRadius <= EARTH_RADIUS) {
            output.impactTime = simTime;
            output.result = SimulationResult::Impact;

            return output;
        }
    }
    
    output.result = SimulationResult::Orbit;

    return output;
}

SimulationOutput simulateEscape(Spacecraft& satellite, double dt, double escapeLimit, IntegratorType integrator) {
    SimulationOutput output;
    output.result = SimulationResult::Escape;
    double simTime = 0.0;

    double currentRadius = sqrt(
        satellite.position.x * satellite.position.x +
        satellite.position.y * satellite.position.y
    );

    while (currentRadius < escapeLimit) {
        updateSpacecraft(satellite, dt);
        simTime += dt;

        currentRadius = sqrt(satellite.position.x * satellite.position.x + satellite.position.y * satellite.position.y);

        double altitudeKm = (currentRadius - EARTH_RADIUS) / 1000.0;

        SimulationState state;

        state.time = simTime;
        state.position = satellite.position;
        state.velocity = satellite.velocity;
        state.altitude = altitudeKm;

        output.states.push_back(state);
    }

    return output;
}

Derivative evaluateDerivative(const Spacecraft& satellite) {
    Derivative derivative;

    derivative.position = satellite.velocity;
    derivative.velocity = gravitationalAcceleration(satellite.position);

    return derivative;
}

std::string trajectoryTypeToString(TrajectoryType type) {
    switch (type) {
        case TrajectoryType::Circular:
            return "Circular";

        case TrajectoryType::Elliptical:
            return "Elliptical";

        case TrajectoryType::Parabolic:
            return "Parabolic";

        case TrajectoryType::Hyperbolic:
            return "Hyperbolic";
    }

    return "Unknown";
}

std::string simulationResultToString(SimulationResult result) {
    switch (result) {
        case SimulationResult::Orbit:
            return "Orbit";

        case SimulationResult::Impact:
            return "Impact";

        case SimulationResult::Escape:
            return "Escape";
    }

    return "Unknown";
}

std::string integratorTypeToString(IntegratorType integrator) {
    switch (integrator) {
        case IntegratorType::Euler:
            return "Euler";

        case IntegratorType::Verlet:
            return "Verlet";

        case IntegratorType::RK4:
            return "RK4";
    }

    return "Unknown";
}