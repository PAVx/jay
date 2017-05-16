#ifndef BARO_P2A_H
#define BARO_P2A_H

/* UNCOMMENT THE FOLLOWING LINE FOR DEBUGGING */
#define BAR_DEBUG_MODE

#define BAR_ADC_RESOLUTION 1024

// V_OUT = V_S(0.002421 * P - 0.00842) + TOTAL_ERROR
#define BAR_VOLTAGE_SOURCE 5
#define BAR_PRESSURE_MULTIPLIER (double)0.00842
#define BAR_PRESSURE_OFFSET (double)0.00842
#define BAR_TOTAL_ERROR 0// Pressure Error * Temperature Factor * V_S

// BAROMETRIC EQUATION
#define BAR_BASE_PRESSURE 101.325//kPA
#define BAR_UNIVERSAL_GAS_CONSTANT (double)8.3144598
#define BAR_GRAVITY_ACCELERATION (double)9.80665//m/s^2
#define BAR_MOLAR_MASS_AIR (double)0.0289644//kg*mol

/* Function Prototypes */

// Converts ADC value to voltage (Volts)
double adc_to_voltage(uint16_t adc);

// Converts voltage (Volts) to pressure (kPA)
double voltage_to_pressure(double voltage);

// Converts pressure (kPA), with respect to base temperature (K), to altitude (m)
double pressure_to_altitude(double pressure, double temp);

// Chain conversion of ADC, with respect to base temperature (K), to altitude (m)
double get_altitude(uint16_t adc, double temp);

#endif