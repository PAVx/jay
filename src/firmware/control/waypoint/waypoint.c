/* 
    PAVx -- Pod-Based Autonomous Vehicles 
    Library Created By: Sargis S Yonan
    March 2017
*/ 

// waypoint.c

#include "control/attitude_adjust/attitude_adjust.h"
#include <stdint.h>
#include <math.h>

#define SQUARED(val) (val * val)

#define IN_PROGRESS (1)
#define IDLE (0)

struct Position {
    double altitude;
    double longitude;
    double latitude;
};
typedef struct Position Position_t;

typedef uint8_t Job_t;

// (-) value indicates a fatal error (outside conditions, battery, etc.)
// the system must return to a safe coordinate in this case
// Other wise a remaining distance in km is held
typedef int32_t Error_t;

static Job_t trip_status;
static Position_t origin;

Error_t WaypointMoveTo(Position_t destination) {
    // move to high enough altitude
    // use yaw to change angle to target direction
    Adjust_Yaw(CalculateYawOffsetToDestination(origin, destination));
    // use pitch to move up to max altitude in the direction of the destination
    // move pitch down toward destination while keeping yaw and roll constant
}

// returns total trip distance
double InitializeWaypoint(void) {
    if (sensors->gps) {
        trip_status = IDLE;
        origin.altitude = sensors->gps.altitude;
        origin.longitude = sensors->gps.longitude;
        origin.latitude = sensors->gps.latitude;
    }

    return TotalTripDistance();
}

double GetCurrentPositionVector(double altitude, double longitude, double latitude) {
    return PythagoreansTheorem(altitude, longitude, latitude);
}

double PythagoreansTheorem(double x, double y, double z) {
    return (sqrt(SQUARED(x) + SQUARED(y) + SQUARED(z)));
}

double CalculateYawOffsetToDestination(GPS_t origin, GPS_t destination) {
    return atan((destination.latitude - origin.latitude) - (destination.longitude - origin.latitude));
}

double TotalTripDistance(Position_t a, Position_t b) {
    double altitude_d = 0.0;
    double longitude_d = 0.0;
    double latitude_d = 0.0;

    altitude_d = abs(a.altitude - b.altitude);
    longitude_d = abs(a.longitude - b.longitude);
    latitude_d = abs(a.latitude - b.latitude);

    return sqrt(SQUARED(altitude_d) + SQUARED(longitude_d) + SQUARED(latitude_d));
}
