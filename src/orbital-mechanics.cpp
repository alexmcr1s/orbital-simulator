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

double semiMajorAxis(double specificEnergy) {
    return -EARTH_MU / (2.0 * specificEnergy);
}

double periapsisRadius(double semiMajorAxis, double eccentricity) {
    return semiMajorAxis * (1.0 - eccentricity);
}

double apoapsisRadius(double semiMajorAxis, double eccentricity) {
    return semiMajorAxis * (1.0 + eccentricity);
}

SimulationResult simulateOrbit(Spacecraft& satellite, double orbitalPeriod, double dt, std::ofstream& outputFile, double& impactTime) {
    double simTime = 0.0;

    while (simTime < orbitalPeriod) {
        double remainingTime = orbitalPeriod - simTime;
        double currentDt = dt;

        if (remainingTime < dt) { currentDt = remainingTime; }

        updateSpacecraft(satellite, currentDt);
        simTime += currentDt;

        double currentRadius = sqrt(
            satellite.position.x * satellite.position.x +
            satellite.position.y * satellite.position.y
        );

        if (currentRadius <= EARTH_RADIUS) {
            impactTime = simTime;
            return SimulationResult::Impact;
        }

        double altitudeKm = (currentRadius - EARTH_RADIUS) / 1000.0;

        outputFile << simTime << ","
                   << satellite.position.x << ","
                   << satellite.position.y << ","
                   << altitudeKm << "\n";
    }
    return SimulationResult::Orbit;
}

SimulationResult simulateEscape(Spacecraft& satellite, double dt, double escapeLimit, std::ofstream& outputFile) {
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

        outputFile << simTime << ","
                   << satellite.position.x << ","
                   << satellite.position.y << ","
                   << altitudeKm << "\n";
    }
    return SimulationResult::Escape;
}