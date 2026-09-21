#include "simulation.h"

#include "constants.h"
#include "orbital-mechanics.h"

#include <cmath>
#include <limits>

namespace {

SimulationValidationError validateConfig(const SimulationConfig& config) {
    if (!std::isfinite(config.altitudeKm) || config.altitudeKm < 0.0) {
        return SimulationValidationError::InvalidAltitude;
    }
    double radius = EARTH_RADIUS + config.altitudeKm * 1000.0;
    if (!std::isfinite(radius)) {
        return SimulationValidationError::InvalidAltitude;
    }
    if (!std::isfinite(config.velocityMultiplier) || config.velocityMultiplier < 0.0) {
        return SimulationValidationError::InvalidVelocityMultiplier;
    }
    if (!std::isfinite(circularVelocity(radius) * config.velocityMultiplier)) {
        return SimulationValidationError::InvalidVelocityMultiplier;
    }
    if (!std::isfinite(config.launchAngleDeg)) {
        return SimulationValidationError::InvalidLaunchAngle;
    }
    switch (config.integrator) {
        case IntegratorType::Euler:
        case IntegratorType::Verlet:
        case IntegratorType::RK4:
            break;
        default:
            return SimulationValidationError::InvalidIntegrator;
    }
    if (config.numberOfOrbits <= 0) {
        return SimulationValidationError::InvalidOrbitCount;
    }
    if (!std::isfinite(config.timeStep) || config.timeStep <= 0.0) {
        return SimulationValidationError::InvalidTimeStep;
    }
    if (!std::isfinite(config.escapeLimit) || config.escapeLimit <= EARTH_RADIUS) {
        return SimulationValidationError::InvalidEscapeLimit;
    }
    if (config.maxIntegrationSteps == 0) {
        return SimulationValidationError::InvalidMaxIntegrationSteps;
    }
    if (config.stateSampleStride == 0) {
        return SimulationValidationError::InvalidStateSampleStride;
    }

    return SimulationValidationError::None;
}

} // namespace

SimulationReport runSimulation(const SimulationConfig& config) {
    SimulationReport report{};
    report.validationError = validateConfig(config);
    if (report.validationError != SimulationValidationError::None) {
        report.status = SimulationRunStatus::InvalidConfiguration;
        return report;
    }

    double altitude = config.altitudeKm * 1000;
    double radius = EARTH_RADIUS + altitude;

    Spacecraft satellite;
    initializeSpacecraft(satellite, radius);

    double launchSpeed = circularVelocity(radius) * config.velocityMultiplier;
    double launchAngleRad = config.launchAngleDeg * PI / 180.0;

    satellite.velocity.x = launchSpeed * cos(launchAngleRad);
    satellite.velocity.y = launchSpeed * sin(launchAngleRad);

    report.initialSpacecraft = satellite;

    // Initial orbital parameters
    report.initialSpeed = spacecraftSpeed(satellite);
    report.initialEnergy = specificOrbitalEnergy(report.initialSpeed, radius);
    report.angularMomentum = specificAngularMomentum(satellite);

    report.eccentricity = orbitalEccentricity(report.initialEnergy, report.angularMomentum);
    Vector2D eVector = eccentricityVector(satellite);
    report.eVecMagnitude = sqrt(eVector.x * eVector.x + eVector.y * eVector.y);

    report.hasDefinedPeriapsisDirection = report.eccentricity >= ECCENTRICITY_TOLERANCE;
    report.periapsisAngleDeg = report.hasDefinedPeriapsisDirection
        ? atan2(eVector.y, eVector.x) * 180.0 / PI
        : std::numeric_limits<double>::quiet_NaN();

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
                                              config.integrator, config.numberOfOrbits, report.initialEnergy,
                                              config.maxIntegrationSteps, config.stateSampleStride);
            break;
        }

        case TrajectoryType::Parabolic:
        case TrajectoryType::Hyperbolic:
            report.simulation = simulateEscape(satellite, config.timeStep, config.escapeLimit, config.integrator,
                                               config.maxIntegrationSteps, config.stateSampleStride);
            break;
    }

    if (report.simulation.reachedStepLimit) {
        report.status = SimulationRunStatus::StepLimitReached;
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
