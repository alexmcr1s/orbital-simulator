#include <iostream>
#include "data-structures.h"
#include "orbital-mechanics.h"
#include "constants.h"
#include <cmath>

using namespace std;

int main() {
    double altitudeKm;

    // User Input
    cout << "Enter altitude of satellite in kilometers: " << endl;
    cin >> altitudeKm;

    double altitude = altitudeKm * 1000;
    double radius = EARTH_RADIUS + altitude;
    double orbitalPeriod = (2 * PI) * sqrt((radius * radius * radius) / EARTH_MU);
    int step = 0;
    int totalSteps = orbitalPeriod / dt;

    Spacecraft satellite;

    // Start satellite directly to the right of Earth.
    satellite.position.x = radius;
    satellite.position.y = 0;

    // For a circular orbit, velocity is perpendicular to the radius vector.
    satellite.velocity.x = 0;
    satellite.velocity.y = circularVelocity(radius);

     // Output results
     cout << "Position: "
          << satellite.position.x << ", "
          << satellite.position.y << endl;

     cout << "Velocity: "
          << satellite.velocity.x << ", "
          << satellite.velocity.y << endl;

     while (step < totalSteps) {
          updateSpacecraft(satellite, dt);

          double r = sqrt(
               satellite.position.x * satellite.position.x +
               satellite.position.y * satellite.position.y
          );

          double alt = r - EARTH_RADIUS;
          double altKm = alt / 1000.0;
          
          if (step % 100 == 0) {
               cout << "Position: " << satellite.position.x
               << ", " << satellite.position.y << endl;

               cout << "Altitude: " << altKm << endl;
          }

          step++;
     }

     cout << "Final Position: " << satellite.position.x
          << ", " << satellite.position.y << endl;

          double error = sqrt(
               (satellite.position.x - radius) *
               (satellite.position.x - radius)
               +
               satellite.position.y *
               satellite.position.y
          );

     double errorKm = error / 1000.0;

     cout << "Error: " << error << endl;

     return 0;
}