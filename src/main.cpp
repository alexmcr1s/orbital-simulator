#include <iostream>
#include "data-structures.h"
#include "orbital-mechanics.h"
#include "constants.h"

using namespace std;

int main() {
    double altitudeKm;

    cout << "Enter altitude of satellite in kilometers: " << endl;
    cin >> altitudeKm;
    
    double altitude = altitudeKm * 1000;
    double radius = EARTH_RADIUS + altitude;

    Spacecraft satellite;

    // Start satellite directly to the right of Earth.
    satellite.position.x = radius;
    satellite.position.y = 0;

    // For a circular orbit, velocity is perpendicular to the radius vector.
    satellite.velocity.x = 0;
    satellite.velocity.y = circularVelocity(radius);

    Vector2D acceleration =
        gravitationalAcceleration(satellite.position);

    cout << "Position: "
         << satellite.position.x << ", "
         << satellite.position.y << endl;

    cout << "Velocity: "
         << satellite.velocity.x << ", "
         << satellite.velocity.y << endl;

    cout << "Acceleration: "
         << acceleration.x << ", "
         << acceleration.y << endl;

    return 0;
}