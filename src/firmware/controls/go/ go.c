// go.c

Success_t DeliverTo(Position_t source_destination, Position_t final_destination) {

    Error_t error = 0;

    error = InitializeWaypoint();

    LogMessage("BEGINNING TRIP AT: %d:%d:%d\n", CurrentHour(), CurrentMinute(), CurrentSecond());
    LogMessage("TOTAL TRIP DISTANCE: %f METERS\n", error);

    GotoOnError(DELIVERY_FAILURE, ret);

    while (ret != PACKAGE_DELIVERED) {
        error = WaypointMoveTo(final_destination);
        if (error < 0) {
            GoHome(source_destination);
            return FAILURE;
        }
        // TODO: <error> meters remaining
    }

DELIVERY_FAILURE:
    return DELIVERY_FAILED;
}

Success_t GoHome(Position_t _home_coordinates) {
    error

    InitializeWaypoint();
    while (error > RETURN_HOME_THRESHOLD_METERS) {
        error = WaypointMoveTo(source_destination);
    }
}

Success_t ReportLocation(void)
