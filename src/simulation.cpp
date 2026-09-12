#include "simulation.h"

#include "constants.h"
#include "orbital-mechanics.h"

#include <cmath>

SimulationReport runSimulation(const SimulationConfig& config) {
    double altitude = config.altitudeKm * 1000;
    double radius = EARTH_RADIUS + altitude;

    Spacecraft satellite;
    initializeSpacecraft(satellite, radius);

    double launchSpeed = circularVelocity(radius) * config.velocityMultiplier;
    double launchAngleRad = config.launchAngleDeg * PI / 180.0;

    satellite.velocity.x = launchSpeed * cos(launchAngleRad);
    satellite.velocity.y = launchSpeed * sin(launchAngleRad);

    SimulationReport report;
    report.initialSpacecraft = satellite;

    // Initial orbital parameters
    report.initialSpeed = spacecraftSpeed(satellite);
    report.initialEnergy = specificOrbitalEnergy(report.initialSpeed, radius);
    report.angularMomentum = specificAngularMomentum(satellite);

    report.eccentricity = orbitalEccentricity(report.initialEnergy, report.angularMomentum);
    Vector2D eVector = eccentricityVector(satellite);
    report.eVecMagnitude = sqrt(eVector.x * eVector.x + eVector.y * eVector.y);

    double periapsisAngleRad = atan2(eVector.y, eVector.x);
    report.periapsisAngleDeg = periapsisAngleRad * 180.0 / PI;
    report.hasDefinedPeriapsisDirection = report.eccentricity >= ECCENTRICITY_TOLERANCE;

    report.semiMajorAxisVal = semiMajorAxis(report.initialEnergy);

    // Classify trajectory
    if (report.initialEnergy < 0) {
        if (report.eccentricity < ECCENTRICITY_TOLERANCE) {
            report.trajectoryType = TrajectoryType::Circular;
        } else {
            report.trajectoryType = TrajectoryType::Elliptical;
        }
    } else if (report.initialEnergy > 0) {
        report.trajectoryType = TrajectoryType::Hyperbolic;
    } else {
        report.trajectoryType = TrajectoryType::Parabolic;
    }

    report.periapsisAltitudeKm = 0.0;
    report.apoapsisAltitudeKm = 0.0;
    report.orbitalPeriod = 0.0;
    report.impactPercent = 0.0;

    // Run appropriate simulation
    switch (report.trajectoryType) {
        case TrajectoryType::Circular:
        case TrajectoryType::Elliptical: {
            double periapsisRadiusVal = periapsisRadius(report.semiMajorAxisVal, report.eccentricity);
            report.periapsisAltitudeKm = (periapsisRadiusVal - EARTH_RADIUS) / 1000.0;

            double apoapsisRadiusVal = apoapsisRadius(report.semiMajorAxisVal, report.eccentricity);
            report.apoapsisAltitudeKm = (apoapsisRadiusVal - EARTH_RADIUS) / 1000.0;

            report.orbitalPeriod = (2.0 * PI) * sqrt((report.semiMajorAxisVal * report.semiMajorAxisVal * report.semiMajorAxisVal) / EARTH_MU);

            report.simulation = simulateOrbit(satellite, report.orbitalPeriod, config.timeStep,
                                              config.integrator, config.numberOfOrbits, report.initialEnergy);
            break;
        }

        case TrajectoryType::Parabolic:
        case TrajectoryType::Hyperbolic:
            report.simulation = simulateEscape(satellite, config.timeStep, config.escapeLimit, config.integrator);
            break;
    }

    // Calculate final specific energy
    double finalRadius = sqrt(
        satellite.position.x * satellite.position.x +
        satellite.position.y * satellite.position.y
    );

    double finalSpeed = spacecraftSpeed(satellite);
    report.finalEnergy = specificOrbitalEnergy(finalSpeed, finalRadius);
    report.energyError = report.finalEnergy - report.initialEnergy;
    report.relativeEnergyError = std::abs(report.energyError / report.initialEnergy) * 100.0;

    if (report.simulation.result == SimulationResult::Impact) {
        report.impactPercent = (report.simulation.impactTime / report.orbitalPeriod) * 100.0;
    }

    report.positionError = sqrt(
        (satellite.position.x - report.initialSpacecraft.position.x) *
        (satellite.position.x - report.initialSpacecraft.position.x) +
        (satellite.position.y - report.initialSpacecraft.position.y) *
        (satellite.position.y - report.initialSpacecraft.position.y)
    );
    report.finalSpacecraft = satellite;

    return report;
}
