#ifndef ORBITAL_MECHANICS_H
#define ORBITAL_MECHANICS_H

#include "data-structures.h"
#include <fstream>
#include <vector>
#include <string>

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

// Velocity Verlet approach
void updateSpacecraftVerlet(Spacecraft& satellite, double dt);

// RK4 Approach
void updateSpacecraftRK4(Spacecraft& satellite, double dt);

// Speed of spacecraft
double spacecraftSpeed(const Spacecraft& sattelite);

// Specific orbital energy
double specificOrbitalEnergy(const Spacecraft& satellite, double speed, double radius);

// Calculates specific angular momentum for periapsis calculations
// Read-only function, does not ever modify "satellite"
double specificAngularMomentum(const Spacecraft& satellite);

// Eccentricity of orbit
double orbitalEccentricity(double specificEnergy, double angularMomentum);

// Vector for eccentricity orientation
Vector2D eccentricityVector(const Spacecraft& satellite);

// Semi-major axis
double semiMajorAxis(double specificEnergy);

// Periapsis radius (lowest orbital point)
double periapsisRadius(double semiMajorAxis, double eccentricity);

// Apoapsis Radius (highest orbital point)
double apoapsisRadius(double semiMajorAxis, double eccentricity);

// Simulates one orbit
SimulationOutput simulateOrbit(Spacecraft& satellite, double orbitalPeriod, double dt, IntegratorType integrator);

// Simualtes the escape trajectory
SimulationOutput simulateEscape(Spacecraft& satellite, double dt, double escapeLimit, IntegratorType integrator);

// Helper for derivatives 
Derivative evaluateDerivative(const Spacecraft& satellite);

// Helpers for metadata
std::string trajectoryTypeToString(TrajectoryType type);

std::string simulationResultToString(SimulationResult result);

std::string integratorTypeToString(IntegratorType integrator);

#endif

