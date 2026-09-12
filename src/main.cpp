#include "constants.h"
#include "orbital-mechanics.h"
#include "simulation.h"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace std;

namespace {

IntegratorType readIntegrator() {
    string intType;
    cout << "Select Integrator Type ('E' - Euler, 'V' - Verlet, 'R' - RK4): ";
    cin >> intType;

    while (intType != "E" && intType != "V" && intType != "R") {
        cout << "Please enter valid selection ('E', 'V', 'R'): ";
        cin >> intType;
    }

    if (intType == "E") { return IntegratorType::Euler; }
    if (intType == "V") { return IntegratorType::Verlet; }
    return IntegratorType::RK4;
}

SimulationConfig readSimulationConfig() {
    SimulationConfig config;

    cout << "Enter altitude of satellite in kilometers: " << endl;
    cin >> config.altitudeKm;
    cout << "Enter velocity multiplier: " << endl;
    cin >> config.velocityMultiplier;
    cout << "Enter launch angle in degrees: " << endl;
    cin >> config.launchAngleDeg;
    config.integrator = readIntegrator();
    cout << "Enter number of orbits to simulate: ";
    cin >> config.numberOfOrbits;

    config.timeStep = dt;
    config.escapeLimit = ESCAPE_LIMIT;
    return config;
}

void writeCsvOutput(const SimulationConfig& config, const SimulationReport& report) {
    ofstream outputFile("orbit.csv");
    outputFile << "time,x,y,altitude,specific_energy,energy_error\n";

    ofstream metadataFile("simulation_metadata.csv");
    metadataFile << "trajectory_type,simulation_result,integrator,"
                 << "periapsis_altitude_km,apoapsis_altitude_km,periapsis_direction_deg\n";

    metadataFile << trajectoryTypeToString(report.trajectoryType) << ","
                 << simulationResultToString(report.simulation.result) << ","
                 << integratorTypeToString(config.integrator) << ","
                 << report.periapsisAltitudeKm << "," << report.apoapsisAltitudeKm << ","
                 << report.periapsisAngleDeg << "\n";

    outputFile << std::setprecision(15);
    for (const SimulationState& state : report.simulation.states) {
        outputFile << state.time << "," << state.position.x << ","
                   << state.position.y << "," << state.altitude << ","
                   << state.specificEnergy << ", " << state.energyError << "\n";
    }
}

void printSimulationReport(const SimulationConfig& config, const SimulationReport& report) {
    cout << fixed << setprecision(9);
    cout << "Initial Specific Energy:                 " << report.initialEnergy << " J/kg" << endl;
    cout << "Final Specific Energy:                   " << report.finalEnergy << " J/kg" << endl;
    cout << "Final Energy Error:                            " << report.energyError << " J/kg" << endl;
    cout << scientific << setprecision(9);
    cout << "Relative Energy Error:                   " << report.relativeEnergyError << endl;

    cout << fixed << setprecision(3);
    cout << "\n-----------------------------------------------------------------" << endl;
    cout << "\n=== Initial State ===\n" << endl;
    cout << "Position:                                " << report.initialSpacecraft.position.x << ", "
         << report.initialSpacecraft.position.y << " m" << endl;
    cout << "Velocity:                                " << report.initialSpacecraft.velocity.x << ", "
         << report.initialSpacecraft.velocity.y << " m/s\n" << endl;
    cout << "-----------------------------------------------------------------" << endl;

    cout << "\n=== Orbital Parameters ===\n" << endl;
    cout << "Initial Specific Energy:                         " << report.initialEnergy << " J/kg" << endl;
    cout << "Specific Angular Momentum:               " << report.angularMomentum << " m^2/s\n" << endl;
    cout << "Eccentricity:                            " << report.eccentricity << endl;
    cout << "Eccentricity Vector Magnitude:            " << report.eVecMagnitude << "\n" << endl;
    cout << "Semi-major axis:                         " << report.semiMajorAxisVal / 1000.0 << " km\n" << endl;

    switch (report.trajectoryType) {
        case TrajectoryType::Circular:
            cout << "Trajectory Type:                         Circular" << endl;
            cout << "SimulationResult:                        Orbit\n" << endl;
            cout << "Periapsis altitude:                      " << report.periapsisAltitudeKm << " km" << endl;
            cout << "Apoapsis altitude:                       " << report.apoapsisAltitudeKm << " km\n" << endl;
            cout << "Orbital Period:                          " << report.orbitalPeriod << " s" << endl;
            break;
        case TrajectoryType::Elliptical:
            cout << "Trajectory Type:                         Elliptical" << endl;
            if (report.simulation.result == SimulationResult::Orbit) {
                cout << "Simulation Result:                       Orbit\n" << endl;
            } else if (report.simulation.result == SimulationResult::Impact) {
                cout << "Simulation Result:                       Impact\n" << endl;
            }
            cout << "Periapsis altitude:                      " << report.periapsisAltitudeKm << " km" << endl;
            cout << "Periapsis Direction:                     " << report.periapsisAngleDeg << " degrees" << endl;
            cout << "Apoapsis altitude:                       " << report.apoapsisAltitudeKm << " km\n" << endl;
            cout << "Orbital Period:                          " << report.orbitalPeriod << " s" << endl;
            break;
        case TrajectoryType::Parabolic:
            cout << "Trajectory Type:                         Parabolic" << endl;
            cout << "Simulation Result:                       Escape\n" << endl;
            cout << "Periapsis Direction:                     " << report.periapsisAngleDeg << " degrees" << endl;
            break;
        case TrajectoryType::Hyperbolic:
            cout << "Trajectory Type:                         Hyperbolic" << endl;
            cout << "Simulation Result:                       Escape\n" << endl;
            cout << "Periapsis Direction:                     " << report.periapsisAngleDeg << " degrees" << endl;
            break;
    }

    cout << "\n-----------------------------------------------------------------" << endl;
    cout << "\n=== Final State ===\n" << endl;
    cout << "Final Position:                          " << report.finalSpacecraft.position.x << ", "
         << report.finalSpacecraft.position.y << " m" << endl;
    if (report.simulation.result == SimulationResult::Orbit) {
        cout << fixed << setprecision(9);
        cout << "Position Error:                          " << report.positionError << " m" << endl;
    }
    if (report.simulation.result == SimulationResult::Impact) {
        cout << "Impact Time:                        " << report.simulation.impactTime << " s" << endl;
        cout << "Orbital Period Elapsed:             " << report.impactPercent << " %" << endl;
    }
    cout << "Integrator:                              " << integratorTypeToString(config.integrator) << endl;
    cout << "\n-----------------------------------------------------------------\n" << endl;
}

} // namespace

int main() {
    SimulationConfig config = readSimulationConfig();
    SimulationReport report = runSimulation(config);
    writeCsvOutput(config, report);
    printSimulationReport(config, report);
    return 0;
}
