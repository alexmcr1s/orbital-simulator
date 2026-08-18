#include <cmath>
#include "orbital-mechanics.h"
#include "constants.h"

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
    double orbitalEnergy = (speed * speed) / 2.0 - EARTH_MU / radius;

    return orbitalEnergy;
}

double spacecraftSpeed(const Spacecraft& satellite){
    double speed = sqrt(
            satellite.velocity.x * satellite.velocity.x +
            satellite.velocity.y * satellite.velocity.y
        );

    return speed;
}

double specificAngularMomentum(const Spacecraft& satellite) {
    double h = satellite.position.x * satellite.velocity.y 
             - satellite.position.y * satellite.velocity.x;

    return h;
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