#include "data-structures.h"             
#include "orbital-mechanics.h"           
#include "constants.h"

#include <iostream>                     // Input/Output                   
#include <cmath>                        // Math functions
#include <iomanip>                      // More math
#include <fstream>                      // File output

using namespace std;

int main() {

     // Prepare CSV
     ofstream outputFile("orbit.csv");
     outputFile << "time,x,y,altitude\n";

     // Get altitude & radius
     double altitudeKm;

     cout << "Enter altitude of satellite in kilometers: " << endl;
     cin >> altitudeKm;

     double altitude = altitudeKm * 1000;
     double radius = EARTH_RADIUS + altitude;

     // Initialize spacecrafrt position
     Spacecraft satellite;
     initializeSpacecraft(satellite, radius);

     satellite.velocity.x = 0;
     satellite.velocity.y = circularVelocity(radius) * 0.7;        //  * Multiplier

     // Save initial state before simulation changes satellite
     double initialX = satellite.position.x;
     double initialY = satellite.position.y;
     double initialVelocityX = satellite.velocity.x;
     double initialVelocityY = satellite.velocity.y;

     // Initial orbital parameters
     double speed = spacecraftSpeed(satellite);

     double specificEnergy = specificOrbitalEnergy(satellite, speed, radius);

     double angularMomentum = specificAngularMomentum(satellite);

     double eccentricity = orbitalEccentricity(specificEnergy, angularMomentum);
 
     double semiMajorAxisVal = semiMajorAxis(specificEnergy);

     // Classify trajectory
     TrajectoryType trajectoryType;

     if (specificEnergy < 0) {
          if (eccentricity < ECCENTRICITY_TOLERANCE) {
               trajectoryType = TrajectoryType::Circular;
          }
          else {
               trajectoryType = TrajectoryType::Elliptical;
          }
     }
     else if (specificEnergy > 0) {
          trajectoryType = TrajectoryType::Hyperbolic;
     }
     else {
           trajectoryType = TrajectoryType::Parabolic;
     }

     // Values only valid for bound orbits
     double periapsisAltitudeKm = 0.0;
     double apoapsisAltitudeKm = 0.0;
     double orbitalPeriod = 0.0;
     SimulationResult result;
     double impactTime = -1.0;
     double impactPercent = 0.0;
     

     // Run appropriate simulation
     switch (trajectoryType) {
          case TrajectoryType::Circular:
          case TrajectoryType::Elliptical: {
               double periapsisRadiusVal = periapsisRadius(semiMajorAxisVal, eccentricity);
               periapsisAltitudeKm = (periapsisRadiusVal - EARTH_RADIUS) / 1000.0;

               double apoapsisRadiusVal = apoapsisRadius(semiMajorAxisVal, eccentricity);
               apoapsisAltitudeKm = (apoapsisRadiusVal - EARTH_RADIUS) / 1000.0;

               orbitalPeriod = (2.0 * PI) * sqrt((semiMajorAxisVal * semiMajorAxisVal * semiMajorAxisVal) / EARTH_MU);

               result = simulateOrbit(satellite, orbitalPeriod, dt, outputFile, impactTime);
               break;
          }

          case TrajectoryType::Parabolic:
          case TrajectoryType::Hyperbolic:
               result = simulateEscape(satellite, dt, ESCAPE_LIMIT, outputFile);
               break;
    }

     outputFile.close();

     if (impactPercent != -1) {
          impactPercent = (impactTime / orbitalPeriod) * 100;
     }

     // Final position difference
     double error = sqrt(
          (satellite.position.x - initialX) *
          (satellite.position.x - initialX)
          +
          (satellite.position.y - initialY) *
          (satellite.position.y - initialY)
     );

     cout << fixed << setprecision(3);

     // Initial state
     cout << "\n=== Initial State ===" << endl;

     cout << "Position: "
          << initialX << ", "
          << initialY << " m" << endl;

     cout << "Velocity: "
          << initialVelocityX << ", "
          << initialVelocityY << " m/s" << endl;

     // Orbital parameters
     cout << "\n=== Orbital Parameters ===" << endl;

     cout << "Specific Energy: "
          << specificEnergy
          << " J/kg" << endl;

     cout << "Specific Angular Momentum: "
          << angularMomentum
          << " m^2/s" << endl;

     cout << "Eccentricity: "
          << eccentricity << endl;

     cout << "Semi-major axis: "
          << semiMajorAxisVal / 1000.0
          << " km" << endl;

     switch (trajectoryType) {

          case TrajectoryType::Circular:
               cout << "Trajectory Type: Circular Orbit" << endl;
               cout << "SimulationResult: Orbit" << endl;
               cout << "Periapsis altitude: " << periapsisAltitudeKm << " km" << endl;
               cout << "Apoapsis altitude: " << apoapsisAltitudeKm << " km" << endl;
               cout << "Orbital Period: " << orbitalPeriod << " s" << endl;
               break;

          case TrajectoryType::Elliptical:
               cout << "Trajectory Type: Elliptical Orbit" << endl;

               if (result == SimulationResult::Orbit) {
                    cout << "SimulationResult: Orbit" << endl;
                    cout << "Periapsis altitude: " << periapsisAltitudeKm << " km" << endl;
                    cout << "Apoapsis altitude: " << apoapsisAltitudeKm << " km" << endl;
                    cout << "Orbital Period: " << orbitalPeriod << " s" << endl;
                    break;
               } 
               
               else if (result == SimulationResult::Impact) {
                    cout << "Simulation Result: Impact" << endl;
                    break;
               }

          case TrajectoryType::Parabolic:
               cout << "Trajectory Type: Parabolic" << endl;
               cout << "Simulation Result: Escape" << endl;
               break;

          case TrajectoryType::Hyperbolic:
               cout << "Trajectory Type: Hyperbolic" << endl;
               cout << "Simulation Result: Escape" << endl;
               break;
     }

     // Final state
     cout << "\n=== Final State ===" << endl;

     cout << "Final Position: " << satellite.position.x << ", " << satellite.position.y << " m" << endl;

    // Only meaningful for closed orbits
    if (trajectoryType == TrajectoryType::Circular || trajectoryType == TrajectoryType::Elliptical) {
          cout << "Position Error: "
               << error << " m" << endl;
     }

     if (result == SimulationResult::Impact) {
          cout << "Impact Time: " << impactTime << " s" << endl;
          cout << "Orbital Period Elapsed: " << impactPercent << " %" << endl;
     }

     return 0;
}