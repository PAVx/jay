// This library converts to and from euler and quaternion units
//    Written by Niraj Raniga - 3/7/17
//    Reference: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

#include "angles.h"
#include <math.h>

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
