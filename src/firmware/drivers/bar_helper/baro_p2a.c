#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "baro_p2a.h"

// Global variables
double altitude = 0;

double adc_to_voltage(uint16_t adc){
    double volt = (((double) adc) / BAR_ADC_RESOLUTION) * BAR_VOLTAGE_SOURCE;
    return volt;
}

double voltage_to_pressure(double voltage){
    double pressure = (((voltage - BAR_TOTAL_ERROR) / BAR_VOLTAGE_SOURCE) + BAR_PRESSURE_OFFSET) / BAR_PRESSURE_MULTIPLIER;
    return pressure;
}

double pressure_to_altitude(double pressure, double temp){
    double altitude = ((log(pressure  / BAR_BASE_PRESSURE)) * (double)BAR_UNIVERSAL_GAS_CONSTANT * temp)
        / (-1 * (double)BAR_GRAVITY_ACCELERATION * (double)BAR_MOLAR_MASS_AIR);
    return altitude;
}

double get_altitude(uint16_t adc, double temp){
#ifdef BAR_DEBUG_MODE
    printf("ADC: %d\n", adc);
#endif
    // Convert ADC to corresponding voltage
    double volt = adc_to_voltage(adc);
#ifdef BAR_DEBUG_MODE
    printf("Voltage: %lf V\n", volt);
#endif
    // Convert voltage to corresponding pressure in kPa
    double pressure = voltage_to_pressure(volt);
#ifdef BAR_DEBUG_MODE
    printf("Pressure: %lf kPA\n", pressure);
#endif
    // Convert atmospheric pressure to altitude in m
    double altitude = pressure_to_altitude(pressure, temp);
#ifdef BAR_DEBUG_MODE
    printf("Altitude: %lf m\n", altitude);
#endif
    return altitude;
}

int main (int argc, char** argv){
    // Announce the arguments
    printf("The argument(s) are: ");
    int i;
    for (i=1; i<argc; i++){
        printf("%s ", argv[i]);
        printf("\n");
    }
    // Pull the value from the arguments
    int adc = atoi(argv[1]);
    adc = (uint16_t) adc;
    double temp = 275; // fake temperature
    // Main Function
    altitude = get_altitude(adc, temp);
    printf("Calculated Altitude: %lf\n", altitude);
}
