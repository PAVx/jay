
#ifndef _COMP_FILTER_H
#define _COMP_FILTER_H

//*********************************************************************************
// Headers
//*********************************************************************************
#include <stdint.h>

//*********************************************************************************
// Macros and Globals
//*********************************************************************************

#define DEG_TO_RAD_RATIO    0.0174532f
#define RAD_TO_DEG_RATIO    57.2957795f

typedef struct
{
    //
    // The time delta between updates. deltaT = 1/(sampling frequency)
    //
    float deltaT;

    //
    // Weighting factor
    //
    float alpha;

    //
    // The most recent accelerometer readings.
    //
    float Ax, Ay, Az;

    //
    // The most recent gyroscope readings.
    //
    float Gx, Gy, Gz;

    //
    // Accelerometer angles in relation to the X and Y axes.
    //
    float rawAngleX, rawAngleY;

    //
    // Comp. filter angle output in relation to the X and Y axes.
    //
    float compAngleX, compAngleY;
}
ImuData;

//*********************************************************************************
// Prototypes
//*********************************************************************************

//
// Comp Filter Initialize
// Description:
//      Initializes the complementary filter.
// Parameters:
// filter - A SixAxis instantiation.
// deltaT - The time delta update period expressed in seconds. This value
//      should be set to how often the filter is updated with new values,
//      which should be on a regular interval. The smaller this value is,
//      the less noise there will be on the comp. filter's output.
// tau - Max allowable time until gyro drifts too far and comp. filter
//      shifts its weight to the accelerometer expressed in seconds. This
//      value is usually based on the drift rate of the gyro. For example,
//      if the gyro drifts at a rate of 0.5 degrees per second and the
//      tolerance of max allowable drift is 1 degree, then tau should be set
//      to 2 seconds as it will take 2 seconds to drift 1 degree. The larger
//      this value is, the less noise there will be on the comp. filter's
//      output. In exchange, it will drift further from the correct angular
//      position.
// Returns:
//      Nothing.
//
void CompInit(ImuData *filter, float deltaT, float tau);

//
// Complementary Filter Start
// Description:
//      Should be called once before CompUpdate can be called at the next interval.
//      CompAccelUpdate must be called before this function.
//      This function helps the filter to converge faster. If this function is not
//      called, the filter will still converge, but it will take longer initially.
// Parameters:
//      filter - A SixAxis instantiation.
// Returns:
//      Nothing.
//
void CompStart(ImuData *filter);

//
// Complementary Filter Update
// Description:
//      Must be called on a regular interval specified by deltaT.
// Parameters:
//      filter - A SixAxis instantiation.
// Returns:
//      Nothing.
//
void CompUpdate(ImuData *filter);

//
// Complementary Filter Angles Get
// Description:
//      Acquires the angles in radians relative to ground along the positive
//      X and Y axes.
// Parameters:
//      filter - A SixAxis instantiation.
//      XAngle - Address of a float to store the angle relative to the X axis into.
//          The number 0 can be passed as a parameter if this angle is not
//          needed.
//      YAngle - Address of a float to store the angle relative to the Y axis into.
//          The number 0 can be passed as a parameter if this angle is not
//          needed.
// Returns:
//      Nothing.
//
void CompAnglesGet(ImuData *filter, float *XAngle, float *YAngle);

//
// Complementary Filter Accelerometer Update
// Description:
//      Updates the comp. filter with new accelerometer values.
// Parameters:
//      filter - A SixAxis instantiation.
//      accelX - Acceleration vector along X axis expressed in m/s^2
//      accelY - Acceleration vector along Y axis expressed in m/s^2
//      accelZ - Acceleration vector along Z axis expressed in m/s^2
// Returns:
//      Nothing.
//
void CompAccelUpdate(ImuData *filter, float accelX, float accelY,
                            float accelZ);

//
// Complementary Filter Gyroscope Update
// Description:
//      Updates the comp. filter with new gyroscope values.
// Parameters:
//      filter - A SixAxis instantiation.
//      gyroX - Angular velocity around X axis expressed in rad/s
//      gyroY - Angular velocity around Y axis expressed in rad/s
//      gyroZ - Angular velocity around Z axis expressed in rad/s
// Returns:
//      Nothing.
//
void CompGyroUpdate(ImuData *filter, float gyroX, float gyroY, float gyroZ);

//
// Complementary Filter Degrees to Radians
// Description:
//      Converts degrees to radians.
// Parameters:
//      degrees - Value in degrees.
// Returns:
//      A value in radians.
//
inline float CompDegreesToRadians(float degrees){ return degrees*DEG_TO_RAD_RATIO; }

//
// Complementary Filter Radians to Degrees
// Description:
//      Converts radians to degrees.
// Parameters:
//      radians - Value in radians.
// Returns:
//      A value in degrees.
//
inline float CompRadiansToDegrees(float radians){ return radians*RAD_TO_DEG_RATIO; }

#endif  // _COMP_FILTER_H
