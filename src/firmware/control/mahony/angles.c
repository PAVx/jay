/*
	PAVx -- Pod-Based Autonomous Vehicles
	Library Created By: Niraj Raniga
	March 2017
*/

// angle.h

// Reference: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

#include "angles.h"
#include <math.h>

double* imu2euler(double accX, double accY, double accZ, double magX, double magY){
        double ypr[3] = {0,0,0};
        ypr[YAW_ANGLE] = (atan2f(magY,magX)*180.0)/M_PI;
        ypr[PITCH_ANGLE] = (atan2(accX, sqrt(accY*accY + accZ*accZ))*180.0)/M_PI;
        ypr[ROLL_ANGLE] = (atan2(-accY, accZ)*180.0)/M_PI;
        return &ypr;
}

double* euler2quat(double yaw, double pitch, double roll){

        double q[4] = {0,0,0,0};

        double cos_phi_2    = cos(yaw * 0.5);
        double cos_theta_2  = cos(theta * 0.5);
        double cos_psi_2    = cos(psi * 0.5);
        double sin_phi_2    = sin(yaw * 0.5);
        double sin_theta_2  = sin(theta * 0.5);
        double sin_psi_2    = sin(psi * 0.5);

        q[0] = cos_phi_2*cos_theta_2*cos_psi_2 + sin_phi_2*sin_theta_2*sin_psi_2
        q[1] = sin_phi_2*cos_theta_2*cos_psi_2 - cos_phi_2*sin_theta_2*sin_psi_2
        q[2] = cos_phi_2*sin_theta_2*cos_psi_2 + sin_phi_2*cos_theta_2*sin_psi_2
        q[3] = cos_phi_2*cos_theta_2*sin_psi_2 - sin_phi_2*sin_theta_2*cos_psi_2

        return q;
}

double* quat2euler(double* q){

        double ypr[3] = {0,0,0};

        ypr[0] = atan2(2*(q[0]*q[1] + q[2]*q[3]), 1-2*(q[1]*q[1] + q[2]*q[2]));
        ypr[2] = atan2(2*(q[0]*q[3] + q[1]*q[2]), 1-2*(q[2]*q[2] + q[3]*q[3]));

        double t0 = +2.0 * (q[0] * q[2] - q[3] * q[1];
        t0 = t0 > 1.0 ? 1.0 : t0;
        t0 = t0 < -1.0 ? -1.0 : t0;
        ypr[1] = asin(t0);
}
