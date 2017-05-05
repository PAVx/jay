/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	March 2017
*/

// angle.h

// Reference: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

#include "angles.h"
#include "system.h"
#include <math.h>

double pitch_level_adjust = 0;
double roll_level_adjust = 0;
bool level_adjust = false;

#define SQUARE(x) (x * x)
#define GYRO_ANGLE_TO_RADIAN (.000000607071044) // GYRO_ANGLE_DEGREE_CONVERSION (0.0000347826) * (pi / 180)
#define RADIAN_TO_DEGREE (57.296) // 180 / pi
#define NEGATIVE_RADIAN_TO_DEGREE (-RADIAN_TO_DEGREE)

#define GYRO_TO_ACCEL_RATIO (0.70)
#define ACCEL_TO_GYRO_RATIO (1 - GYRO_TO_ACCEL_RATIO)

void imu2euler(double* ypr, double accX, double accY, double accZ, double gyroX, double gyroY, double gyroZ, double magX, double magY){
        static double roll_gyro = 0.0;
        static double pitch_gyro = 0.0;
        static double roll_accel = 0.0;
        static double pitch_accel = 0.0;

        double temp = 0.0;

        double accel_vector_magnitude = 0.0;

        roll_gyro += gyroX * GYRO_ANGLE_DEGREE_CONVERSION;
        pitch_gyro += gyroY * GYRO_ANGLE_DEGREE_CONVERSION;

        temp = sin(gyroZ * GYRO_ANGLE_TO_RADIAN);

        roll_gyro -= pitch_gyro * temp;
        pitch_gyro += roll_gyro * temp;

        accel_vector_magnitude = sqrt((SQUARE(accX)) + (SQUARE(accY)) + (SQUARE(accZ)));

        if (abs(accY) < accel_vector_magnitude) {                              
                roll_accel = asin(accY / accel_vector_magnitude) * RADIAN_TO_DEGREE;
        }
        if (abs(accX) < accel_vector_magnitude) {                              
                pitch_accel = asin(accX / accel_vector_magnitude) * NEGATIVE_RADIAN_TO_DEGREE;
        }

        roll_gyro = roll_gyro * GYRO_TO_ACCEL_RATIO + roll_accel * ACCEL_TO_GYRO_RATIO;
        pitch_gyro = pitch_gyro * GYRO_TO_ACCEL_RATIO + pitch_accel * ACCEL_TO_GYRO_RATIO;

        if (level_adjust) {
                roll_level_adjust = roll_gyro * 15;
                pitch_level_adjust = pitch_gyro * 15;
        } else {
                pitch_level_adjust = 0;
                roll_level_adjust = 0;
        }

        ypr[0] = 0.0 * magY * magX;
        ypr[1] = pitch_gyro;
        ypr[2] = roll_gyro;


}

