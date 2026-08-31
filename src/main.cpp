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
     outputFile << "time,x,y,altitude,specific_energy\n";
     ofstream metadataFile("simulation_metadata.csv");
     metadataFile
          << "trajectory_type,"
          << "simulation_result,"
          << "integrator,"
          << "periapsis_altitude_km,"
          << "apoapsis_altitude_km,"
          << "periapsis_direction_deg\n";

     // Get altitude, velocity multiplier, and launch angle from user
     double altitudeKm;

     cout << "Enter altitude of satellite in kilometers: " << endl;
     cin >> altitudeKm;

     double altitude = altitudeKm * 1000;
     double radius = EARTH_RADIUS + altitude;

     double velocityMultiplier = 1.0;

     cout << "Enter velocity multiplier: " << endl;
     cin >> velocityMultiplier;

     double launchAngleDeg = 0.0;

     cout << "Enter launch angle in degrees: " << endl;
     cin >> launchAngleDeg;

     IntegratorType integrator;
     string intType;

     cout << "Select Integrator Type ('E' - Euler, 'V' - Verlet, 'R' - RK4): ";
     cin >> intType;

     while (intType != "E" && intType != "V" && intType != "R") {
          cout << "Please enter valid selection ('E', 'V', 'R'): ";
          cin >> intType;
     }

     if (intType == "E")      { integrator = IntegratorType::Euler; }
     else if (intType == "V") { integrator = IntegratorType::Verlet; }
     else                     { integrator = IntegratorType::RK4; }

     int numberOfOrbits;

     cout << "Enter number of orbits to simulate: ";
     cin >> numberOfOrbits;

     // Initialize spacecraft position
     Spacecraft satellite;
     initializeSpacecraft(satellite, radius);

     // Initialize spacecraft velocity
     double launchSpeed = circularVelocity(radius) * velocityMultiplier;
     double launchAngleRad = launchAngleDeg * PI / 180.0;

     satellite.velocity.x = launchSpeed * cos(launchAngleRad);
     satellite.velocity.y = launchSpeed * sin(launchAngleRad);

     // Save initial state before simulation changes satellite
     double initialX = satellite.position.x;
     double initialY = satellite.position.y;
     double initialVelocityX = satellite.velocity.x;
     double initialVelocityY = satellite.velocity.y;

     // Initial orbital parameters
     double initialSpeed = spacecraftSpeed(satellite);
     double initialEnergy = specificOrbitalEnergy(satellite, initialSpeed, radius);
     double angularMomentum = specificAngularMomentum(satellite);

     double eccentricity = orbitalEccentricity(initialEnergy, angularMomentum);
     Vector2D eVector = eccentricityVector(satellite);
     double eVecMagnitude = sqrt(eVector.x * eVector.x + eVector.y * eVector.y);

     double periapsisAngleRad = atan2(eVector.y, eVector.x);
     double periapsisAngleDeg = periapsisAngleRad * 180.0 / PI;
     bool hasDefinedPeriapsisDirection = eccentricity >= ECCENTRICITY_TOLERANCE;

     double semiMajorAxisVal = semiMajorAxis(initialEnergy);

     // Classify trajectory
     TrajectoryType trajectoryType;

     if (initialEnergy < 0) {
          if (eccentricity < ECCENTRICITY_TOLERANCE) {
               trajectoryType = TrajectoryType::Circular;
          }
          else {
               trajectoryType = TrajectoryType::Elliptical;
          }
     }
     else if (initialEnergy > 0) {
          trajectoryType = TrajectoryType::Hyperbolic;
     }
     else {
           trajectoryType = TrajectoryType::Parabolic;
     }

     double periapsisAltitudeKm = 0.0;
     double apoapsisAltitudeKm = 0.0;
     double orbitalPeriod = 0.0;

     SimulationOutput simulation;
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

               simulation = simulateOrbit(satellite, orbitalPeriod, dt, integrator, numberOfOrbits);
               break;
          }

          case TrajectoryType::Parabolic:
          case TrajectoryType::Hyperbolic:
               simulation = simulateEscape(satellite, dt, ESCAPE_LIMIT, integrator);
               break;
    }

     // Calculate final specific energy
     double finalRadius = sqrt(
          satellite.position.x * satellite.position.x +
          satellite.position.y * satellite.position.y
     );

     double finalSpeed = spacecraftSpeed(satellite);
     double finalEnergy = specificOrbitalEnergy(satellite, finalSpeed, finalRadius);
     double energyError = finalEnergy - initialEnergy;
     double relativeEnergyError = abs(energyError / initialEnergy);

     if (simulation.result == SimulationResult::Impact) {
          impactPercent = (simulation.impactTime / orbitalPeriod) * 100.0;
     }

     string periapsisAltitudeOutput = "N/A";
     string apoapsisAltitudeOutput = "N/A";
     string periapsisDirectionOutput = "N/A";

     if (trajectoryType == TrajectoryType::Circular || trajectoryType == TrajectoryType::Elliptical) {
          periapsisAltitudeOutput = to_string(periapsisAltitudeKm);
          apoapsisAltitudeOutput = to_string(apoapsisAltitudeKm);
     }

     if (hasDefinedPeriapsisDirection) {
     periapsisDirectionOutput = to_string(periapsisAngleDeg);
     }

     // Final position difference
     double error = sqrt(
          (satellite.position.x - initialX) *
          (satellite.position.x - initialX)
          +
          (satellite.position.y - initialY) *
          (satellite.position.y - initialY)
     );

     cout << fixed << setprecision(9);

     cout << "Initial Specific Energy:                 "
          << initialEnergy << " J/kg" << endl;

     cout << "Final Specific Energy:                   "
          << finalEnergy << " J/kg" << endl;

     cout << "Energy Error:                            "
          << energyError << " J/kg" << endl;

     cout << scientific << setprecision(9);
     
     cout << "Relative Energy Error:                   "
          << relativeEnergyError << endl;

     metadataFile
          << trajectoryTypeToString(trajectoryType) << ","
          << simulationResultToString(simulation.result) << ","
          << integratorTypeToString(integrator) << ","
          << periapsisAltitudeKm << ","
          << apoapsisAltitudeKm << ","
          << periapsisAngleDeg << "\n";

     metadataFile.close();

     for (const SimulationState& state : simulation.states) {
          outputFile << state.time << "," << state.position.x << ","
                     << state.position.y << "," << state.altitude << ","
                     << state.specificEnergy << "\n";
     }

     outputFile.close();

     cout << fixed << setprecision(3);

     // Initial state
     cout << "\n-----------------------------------------------------------------" << endl;

     cout << "\n=== Initial State ===\n" << endl;

     cout << "Position:                                "
          << initialX << ", "
          << initialY << " m" << endl;

     cout << "Velocity:                                "
          << initialVelocityX << ", "
          << initialVelocityY << " m/s\n" << endl;

     cout << "-----------------------------------------------------------------" << endl;

     // Orbital parameters
     cout << "\n=== Orbital Parameters ===\n" << endl;

     cout << "Initial Specific Energy:                         "
          << initialEnergy
          << " J/kg" << endl;

     cout << "Specific Angular Momentum:               "
          << angularMomentum
          << " m^2/s\n" << endl;

     cout << "Eccentricity:                            "
          << eccentricity << endl;

     // !! Test line
     cout << "Eccentricity Vector Magnitude:            "
          << eVecMagnitude << "\n" << endl;

     cout << "Semi-major axis:                         "
          << semiMajorAxisVal / 1000.0
          << " km\n" << endl;

     switch (trajectoryType) {

          case TrajectoryType::Circular:
               cout << "Trajectory Type:                         Circular" << endl;

               cout << "SimulationResult:                        Orbit\n" << endl;

               cout << "Periapsis altitude:                      " << periapsisAltitudeKm << " km" << endl;

               cout << "Apoapsis altitude:                       " << apoapsisAltitudeKm << " km\n" << endl;

               cout << "Orbital Period:                          " << orbitalPeriod << " s" << endl;

               break;

          case TrajectoryType::Elliptical:
               cout << "Trajectory Type:                         Elliptical" << endl;

               if (simulation.result == SimulationResult::Orbit) {
                    cout << "Simulation Result:                       Orbit\n" << endl;
               }
               else if (simulation.result == SimulationResult::Impact) {
                    cout << "Simulation Result:                       Impact\n" << endl;
               }

               cout << "Periapsis altitude:                      " << periapsisAltitudeKm << " km" << endl;

               cout << "Periapsis Direction:                     " << periapsisAngleDeg << " degrees" << endl;

               cout << "Apoapsis altitude:                       " << apoapsisAltitudeKm << " km\n" << endl;

               cout << "Orbital Period:                          " << orbitalPeriod << " s" << endl;

               break;

          case TrajectoryType::Parabolic:
               cout << "Trajectory Type:                         Parabolic" << endl;

               cout << "Simulation Result:                       Escape\n" << endl;

               cout << "Periapsis Direction:                     " << periapsisAngleDeg << " degrees" << endl;

               break;

          case TrajectoryType::Hyperbolic:
               cout << "Trajectory Type:                         Hyperbolic" << endl;

               cout << "Simulation Result:                       Escape\n" << endl;

               cout << "Periapsis Direction:                     " << periapsisAngleDeg << " degrees" << endl;

               break;
     }

     cout << "\n-----------------------------------------------------------------" << endl;

     // Final state
     cout << "\n=== Final State ===\n" << endl;

     cout << "Final Position:                          " << satellite.position.x << ", " << satellite.position.y << " m" << endl;
     
     // Only meaningful for closed orbits
     if (simulation.result == SimulationResult::Orbit) {
          cout << fixed << setprecision(9);
          cout << "Position Error:                          " << error << " m" << endl;
     }

     // Only meaningful for impacts
     if (simulation.result == SimulationResult::Impact) {
          cout << "Impact Time:                        " << simulation.impactTime << " s" << endl;
          cout << "Orbital Period Elapsed:             " << impactPercent << " %" << endl;
     }

     cout << "Integrator:                              ";

     switch (integrator) {
          case IntegratorType::Euler:
               cout << "Euler";
               break;

          case IntegratorType::Verlet:
               cout << "Verlet";
               break;

          case IntegratorType::RK4:
               cout << "RK4";
               break;
     }

cout << endl;

     cout << "\n-----------------------------------------------------------------\n" << endl;


     return 0;
}