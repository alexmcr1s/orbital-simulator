#ifndef CONSTANTS_H
#define CONSTANTS_H

// **Denotes configuration settings rather than physical constants

// Units: m^3/s^2
constexpr double EARTH_MU = 3.986004418e14;

// Units: m
constexpr double EARTH_RADIUS = 6371000.0;

// Euler constant for passage of time in secs
constexpr double dt = 0.1;

// Pi
constexpr double PI = 3.14159265358979323846;

// Eccentricity check
constexpr double ECCENTRICITY_TOLERANCE = 1e-6;

// **Distance limit for escape simulation (100,000 km in meters)
constexpr double ESCAPE_LIMIT = 100000.0 * 1000.0;

#endif