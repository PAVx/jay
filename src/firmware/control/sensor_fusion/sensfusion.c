/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie Firmware
 *
 * Copyright (C) 2011-2012 Bitcraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 */
#include <math.h>
#include <stdint.h>

#include "sensfusion.h"

#define M_PI_F ((double) M_PI)

//#define MADWICK_QUATERNION_IMU
static uint8_t _update_flag = 0;

uint8_t IMUGetFlag(void) {
	return _update_flag;
}

void IMUSetFlag(void) {
	_update_flag = 1;
}

void IMUResetFlag(void) {
	_update_flag = 0;
}

#ifdef MADWICK_QUATERNION_IMU
  #define BETA_DEF     0.01    // 2 * proportional gain
#else // MAHONY_QUATERNION_IMU
	#define TWO_KP_DEF  (2.0 * 15) // 2 * proportional gain
	#define TWO_KI_DEF  (2.0 * 0.000001)//1) // 2 * integral gain
	#define TWO_KD_DEF  (0)//2.0 * 0)
#endif

#ifdef MADWICK_QUATERNION_IMU
  double beta = BETA_DEF;     // 2 * proportional gain (Kp)
#else // MAHONY_QUATERNION_IMU
  double twoKp = TWO_KP_DEF;    // 2 * proportional gain (Kp)
  double twoKi = TWO_KI_DEF;    // 2 * integral gain (Ki)
  double twoKd = TWO_KD_DEF;
  double integralFBx = 0.0;
  double integralFBy = 0.0;
  double integralFBz = 0.0;  // integral error terms scaled by Ki
  double derivativeFBx = 0.0;
  double derivativeFBy = 0.0;
  double derivativeFBz = 0.0;  // derivative error terms scaled by Kd

#endif

double q0 = 1.0;
double q1 = 0.0;
double q2 = 0.0;
double q3 = 0.0;  // quaternion of sensor frame relative to auxiliary frame

static double gravX, gravY, gravZ; // Unit vector in the estimated gravity direction

// The acc in Z for static position (g)
// Set on first update, assuming we are in a static position since the sensors were just calibrates.
// This value will be better the more level the copter is at calibration time
static double baseZacc = 1.0;

static bool isInit;

static bool isCalibrated = false;

static void sensfusion6UpdateQImpl(double gx, double gy, double gz, double ax, double ay, double az, double dt);
static double sensfusion6GetAccZ(const double ax, const double ay, const double az);
static void estimatedGravityDirection(double* gx, double* gy, double* gz);

// TODO: Make math util file
static double invSqrt(double x);

void sensfusion6Init()
{
  if(isInit)
	return;

  isInit = true;
}

bool sensfusion6Test(void)
{
  return isInit;
}

void sensfusion6UpdateQ(double gx, double gy, double gz, double ax, double ay, double az, double dt)
{
  sensfusion6UpdateQImpl(gx, gy, gz, ax, ay, az, dt);
  estimatedGravityDirection(&gravX, &gravY, &gravZ);

  if (!isCalibrated) {
	baseZacc = sensfusion6GetAccZ(ax, ay, az);
	isCalibrated = true;
  }
}

#ifdef MADWICK_QUATERNION_IMU
// Implementation of Madgwick's IMU and AHRS algorithms.
// See: http://www.x-io.co.uk/open-source-ahrs-with-x-imu
//
// Date     Author          Notes
// 29/09/2011 SOH Madgwick    Initial release
// 02/10/2011 SOH Madgwick  Optimised for reduced CPU load
static void sensfusion6UpdateQImpl(double gx, double gy, double gz, double ax, double ay, double az, double dt)
{
  double recipNorm;
  double s0, s1, s2, s3;
  double qDot1, qDot2, qDot3, qDot4;
  double _2q0, _2q1, _2q2, _2q3, _4q0, _4q1, _4q2 ,_8q1, _8q2, q0q0, q1q1, q2q2, q3q3;

  // Rate of change of quaternion from gyroscope
  qDot1 = 0.5 * (-q1 * gx - q2 * gy - q3 * gz);
  qDot2 = 0.5 * (q0 * gx + q2 * gz - q3 * gy);
  qDot3 = 0.5 * (q0 * gy - q1 * gz + q3 * gx);
  qDot4 = 0.5 * (q0 * gz + q1 * gy - q2 * gx);

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0f) && (ay == 0.0f) && (az == 0.0f)))
  {
	// Normalise accelerometer measurement
	recipNorm = invSqrt(ax * ax + ay * ay + az * az);
	ax *= recipNorm;
	ay *= recipNorm;
	az *= recipNorm;

	// Auxiliary variables to avoid repeated arithmetic
	_2q0 = 2.0 * q0;
	_2q1 = 2.0 * q1;
	_2q2 = 2.0 * q2;
	_2q3 = 2.0 * q3;
	_4q0 = 4.0 * q0;
	_4q1 = 4.0 * q1;
	_4q2 = 4.0 * q2;
	_8q1 = 8.0 * q1;
	_8q2 = 8.0 * q2;
	q0q0 = q0 * q0;
	q1q1 = q1 * q1;
	q2q2 = q2 * q2;
	q3q3 = q3 * q3;

	// Gradient decent algorithm corrective step
	s0 = _4q0 * q2q2 + _2q2 * ax + _4q0 * q1q1 - _2q1 * ay;
	s1 = _4q1 * q3q3 - _2q3 * ax + 4.0 * q0q0 * q1 - _2q0 * ay - _4q1 + _8q1 * q1q1 + _8q1 * q2q2 + _4q1 * az;
	s2 = 4.0 * q0q0 * q2 + _2q0 * ax + _4q2 * q3q3 - _2q3 * ay - _4q2 + _8q2 * q1q1 + _8q2 * q2q2 + _4q2 * az;
	s3 = 4.0 * q1q1 * q3 - _2q1 * ax + 4.0 * q2q2 * q3 - _2q2 * ay;
	recipNorm = invSqrt(s0 * s0 + s1 * s1 + s2 * s2 + s3 * s3); // normalise step magnitude
	s0 *= recipNorm;
	s1 *= recipNorm;
	s2 *= recipNorm;
	s3 *= recipNorm;

	// Apply feedback step
	qDot1 -= beta * s0;
	qDot2 -= beta * s1;
	qDot3 -= beta * s2;
	qDot4 -= beta * s3;
  }

  // Integrate rate of change of quaternion to yield quaternion
  q0 += qDot1 * dt;
  q1 += qDot2 * dt;
  q2 += qDot3 * dt;
  q3 += qDot4 * dt;

  // Normalise quaternion
  recipNorm = invSqrt(q0*q0 + q1*q1 + q2*q2 + q3*q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;
}
#else // MAHONY_QUATERNION_IMU
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/open-source-ahrs-with-x-imu
//
// Date     Author      Notes
// 29/09/2011 SOH Madgwick    Initial release
// 02/10/2011 SOH Madgwick  Optimised for reduced CPU load
static void sensfusion6UpdateQImpl(double gx, double gy, double gz, double ax, double ay, double az, double dt)
{
	#if TWO_KD_DEF > 0
		static double last_halfex = 0.0;
		static double last_halfey = 0.0;
		static double last_halfez = 0.0;
	#endif
  double recipNorm = 0.0;
  double halfvx = 0.0;
  double halfvy = 0.0;
  double halfvz = 0.0;
  double halfex = 0.0;
  double halfey = 0.0;
  double halfez = 0.0;
  double qa = 0.0;
  double qb = 0.0;
  double qc = 0.0;

  gx = gx * M_PI_F / 180;
  gy = gy * M_PI_F / 180;
  gz = gz * M_PI_F / 180;

  // Compute feedback only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
  if(!((ax == 0.0) && (ay == 0.0) && (az == 0.0)))
  {
	// Normalise accelerometer measurement
	recipNorm = invSqrt(ax * ax + ay * ay + az * az);
	ax *= recipNorm;
	ay *= recipNorm;
	az *= recipNorm;

	// Estimated direction of gravity and vector perpendicular to magnetic flux
	halfvx = q1 * q3 - q0 * q2;
	halfvy = q0 * q1 + q2 * q3;
	halfvz = q0 * q0 - 0.5 + q3 * q3;

	// Error is sum of cross product between estimated and measured direction of gravity
	halfex = (ay * halfvz - az * halfvy);
	halfey = (az * halfvx - ax * halfvz);
	halfez = (ax * halfvy - ay * halfvx);

	#if TWO_KD_DEF > 0
	  derivativeFBx = (twoKd * halfex) * (twoKd * ((halfex - last_halfex) / dt));
	  derivativeFBy = (twoKd * halfey) * (twoKd * ((halfey - last_halfey) / dt));
	  derivativeFBz = (twoKd * halfez) * (twoKd * ((halfez - last_halfez) / dt));

	  if (derivativeFBx > 50 && derivativeFBy > 50 && derivativeFBz > 50) {
		  gx += derivativeFBx;
		  gy += derivativeFBy;
		  gz += derivativeFBz;
		}
	#else
	  derivativeFBx = 0.0;
	  derivativeFBy = 0.0;
	  derivativeFBz = 0.0;
	#endif



	// Compute and apply integral feedback if enabled
	if(twoKi > 0.0)
	{
	  integralFBx += twoKi * halfex * dt;  // integral error scaled by Ki
	  integralFBy += twoKi * halfey * dt;
	  integralFBz += twoKi * halfez * dt;
	  gx += integralFBx;  // apply integral feedback
	  gy += integralFBy;
	  gz += integralFBz;
	}
	else
	{
	  integralFBx = 0.0; // prevent integral windup
	  integralFBy = 0.0;
	  integralFBz = 0.0;
	}

	// Apply proportional feedback
	gx += twoKp * halfex;
	gy += twoKp * halfey;
	gz += twoKp * halfez;
  }

  // Integrate rate of change of quaternion
  gx *= (0.5 * dt);   // pre-multiply common factors
  gy *= (0.5 * dt);
  gz *= (0.5 * dt);
  qa = q0;
  qb = q1;
  qc = q2;
  q0 += (-qb * gx - qc * gy - q3 * gz);
  q1 += (qa * gx + qc * gz - q3 * gy);
  q2 += (qa * gy - qb * gz + q3 * gx);
  q3 += (qa * gz + qb * gy - qc * gx);

  // Normalise quaternion
  recipNorm = invSqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
  q0 *= recipNorm;
  q1 *= recipNorm;
  q2 *= recipNorm;
  q3 *= recipNorm;

	#if TWO_KD_DEF > 0
  	last_halfex = halfex;
	last_halfey = halfey;
	last_halfez = halfez;
	#endif
}
#endif

void sensfusion6GetEulerRPY(double* roll, double* pitch, double* yaw)
{
  double gx = gravX;
  double gy = gravY;
  double gz = gravZ;

  if (gx>1) gx=1;
  if (gx<-1) gx=-1;

  *yaw = atan2(2*(q0*q3 + q1*q2), q0*q0 + q1*q1 - q2*q2 - q3*q3) * 180 / M_PI_F;
  *pitch = asin(gx) * 180 / M_PI_F; //Pitch seems to be inverted
  *roll = atan2(gy, gz) * 180 / M_PI_F;
}

double sensfusion6GetAccZWithoutGravity(const double ax, const double ay, const double az)
{
  return sensfusion6GetAccZ(ax, ay, az) - baseZacc;
}

double sensfusion6GetInvThrustCompensationForTilt()
{
  // Return the z component of the estimated gravity direction
  // (0, 0, 1) dot G
  return gravZ;
}

//---------------------------------------------------------------------------------------------------
// Fast inverse square-root
// See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
double invSqrt(double x)
{
  /*
  double halfx = 0.5 * x;
  double y = x;
  int64_t i = (int64_t)y;
  i = 0x5f3759df - (i>>1);
  y = (double)i;
  y = y * (1.5f - (halfx * y * y));
  return y;
  */

  return (1 / sqrt(x));
}

static double sensfusion6GetAccZ(const double ax, const double ay, const double az)
{
  // return vertical acceleration
  // (A dot G) / |G|,  (|G| = 1) -> (A dot G)
  return (ax * gravX + ay * gravY + az * gravZ);
}

static void estimatedGravityDirection(double* gx, double* gy, double* gz)
{
  *gx = 2 * (q1 * q3 - q0 * q2);
  *gy = 2 * (q0 * q1 + q2 * q3);
  *gz = q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3;
}
