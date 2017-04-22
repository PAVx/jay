//=====================================================================================================
// MahonyAHRS.h
//=====================================================================================================
//
// Madgwick's implementation of Mayhony's AHRS algorithm.
// See: http://www.x-io.co.uk/node/8#open_source_ahrs_and_imu_algorithms
//
// Date			  Author			  Notes
// 29/09/2011	SOH Madgwick  Initial release
// 02/10/2011	SOH Madgwick	Optimised for reduced CPU load
// 03/07/17		Niraj Raniga	Reworked for PAVx
//
//=====================================================================================================
#ifndef MahonyAHRS_h
#define MahonyAHRS_h

//----------------------------------------------------------------------------------------------------
// Variable declaration

#define sampleFreq	512.0f			// sample frequency in Hz
#define TWOKP	(2.0f * 0.5f)	// 2 * proportional gain
#define TWOKI	(2.0f * 0.0f)	// 2 * integral gain

extern volatile float q0, q1, q2, q3;	// quaternion of sensor frame relative to auxiliary frame

//---------------------------------------------------------------------------------------------------
// Function declarations

void MahonyAHRSupdate(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz);
float invSqrt(float x);

#endif
//=====================================================================================================
// End of file
//=====================================================================================================
