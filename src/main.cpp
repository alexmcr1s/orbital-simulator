#include "data-structures.h"             
#include "orbital-mechanics.h"           
#include "constants.h"
#include <iostream>                     // Input/Output                   
#include <cmath>                        // Math functions
#include <iomanip>                      // More math
#include <fstream>                      // File output

using namespace std;

int main() {
     // Initializing data & creating output file
     double altitudeKm;

     cout << "Enter altitude of satellite in kilometers: " << endl;
     cin >> altitudeKm;

     ofstream outputFile("orbit.csv");
     outputFile << "time,x,y,altitude\n";

     double altitude = altitudeKm * 1000;
     double radius = EARTH_RADIUS + altitude;
     double orbitalPeriod = (2 * PI) * sqrt((radius * radius * radius) / EARTH_MU);
     int step = 0;
     int totalSteps = round(orbitalPeriod / dt);
     double simTime = 0.0;

     Spacecraft satellite;

     // Start satellite directly to the right of Earth.
     initializeSpacecraft(satellite, radius);

     // For a circular orbit, velocity is perpendicular to the radius vector.
     // Multiplier added to circular valocity to simulate elipse rather than perfect circle
     satellite.velocity.x = 0;
     satellite.velocity.y = circularVelocity(radius) * 1.0885;                   // Multiplier

     // Gathering spacecraft data
     double speed = spacecraftSpeed(satellite);
     double specificEnergy = specificOrbitalEnergy(satellite, speed, radius);

     // Decimal precision for results
     cout << fixed << setprecision(3);

     // Output results
     cout << "Position: "
          << satellite.position.x << ", "
          << satellite.position.y << endl;

     cout << "Velocity: "
          << satellite.velocity.x << ", "
          << satellite.velocity.y << endl;

     while (simTime < orbitalPeriod) {
          double remainingTime = orbitalPeriod - simTime;
          double currentDt = dt;

          if (remainingTime < dt) {
               currentDt = remainingTime;
          }

          updateSpacecraft(satellite, currentDt);
          simTime += currentDt;

          double r = sqrt(
               satellite.position.x * satellite.position.x +
               satellite.position.y * satellite.position.y
          );

          double alt = r - EARTH_RADIUS;
          double altKm = alt / 1000.0;

          outputFile << simTime << ","
           << satellite.position.x << ","
           << satellite.position.y << ","
           << altKm << "\n";
          
          if (step % 100 == 0) {
               cout << "Position: " << satellite.position.x
               << ", " << satellite.position.y << endl;

               cout << "Altitude: " << altKm << endl;
          }

          step++;
     }

     outputFile.close();

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
     cout << "Specific Orbital Energy: "
          << specificEnergy << endl;

     double angularMomentum = specificAngularMomentum(satellite);
     cout << "Specific Angular Momentum: " << angularMomentum << endl;

     double eccentricity = orbitalEccentricity(specificEnergy, angularMomentum);
     cout << "Orbital Eccentricity: " << eccentricity << endl;

     return 0;
}