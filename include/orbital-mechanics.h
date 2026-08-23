#ifndef ORBITAL_MECHANICS_H
#define ORBITAL_MECHANICS_H

#include "data-structures.h"
#include <fstream>

// Starting the satellite exactly right of Earth
void initializeSpacecraft(Spacecraft& satellite, double radius);

// Calculates circular orbital velocity at a given orbital radius
double circularVelocity(double radius);

// Calculates dynamic position of spacecraft
double vectorMagnitude(Vector2D vector);

// Calculates gravitational acceleration at a given position
Vector2D gravitationalAcceleration(Vector2D position);

// Updates state of spacecraft
void updateSpacecraft(Spacecraft& satellite, double dt);

// Speed of spacecraft
double spacecraftSpeed(const Spacecraft& sattelite);

// Specific orbital energy
double specificOrbitalEnergy(const Spacecraft& satellite, double speed, double radius);

// Calculates specific angular momentum for periapsis calculations
// Read-only function, does not ever modify "satellite"
double specificAngularMomentum(const Spacecraft& satellite);

// Eccentricity of orbit
double orbitalEccentricity(double specificEnergy, double angularMomentum);

// Semi-major axis
double semiMajorAxis(double specificEnergy);

// Periapsis radius (lowest orbital point)
double periapsisRadius(double semiMajorAxis, double eccentricity);

// Apoapsis Radius (highest orbital point)
double apoapsisRadius(double semiMajorAxis, double eccentricity);

// Simulates one orbit
SimulationResult simulateOrbit(Spacecraft& satellite, double orbitalPeriod, double dt, std::ofstream& outputFile, double& impactTime);

// Simualtes the escape trajectory
SimulationResult simulateEscape(Spacecraft& satellite, double dt, double escapeLimit, std::ofstream& outputFile);

#endif

