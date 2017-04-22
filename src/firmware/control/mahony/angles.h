// This library converts to and from euler and quaternion units
//    Written by Niraj Raniga - 3/7/17
//    Reference: https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles

double* euler2quat(double yaw, double pitch, double roll);
double* quat2euler(double* q);
