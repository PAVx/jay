//*********************************************************************************
// Six Axis Complementary Filter - Platform Independent
//
// Revision: 1.5
//
// Description: Takes gyroscope and accelerometer readings and produces a "fused"
// reading that is more accurate. Relies heavily on floating point arithmetic
// and trigonometry.
//
// This library has been tested to work with the MPU-6050 6 DOF IMU Sensor
// comprising a gyroscope and accelerometer. It should also work with other 6
// DOF IMUs but there are no guarantees. Also, this library should work
// with even 5 DOF IMUs since the gyroscope reading about the Z axis technically
// goes unused with this library at its current revision.
//
//*********************************************************************************

//*********************************************************************************
// Headers
//*********************************************************************************
#include <math.h>
#include "comp_filter.h"

//*********************************************************************************
// Macros and Globals
//*********************************************************************************
#define PI			3.1415926f
#define HALF_PI			1.5707963f
#define TWO_PI			6.2831853f
#define SQ(x)			((x)*(x))

//*********************************************************************************
// Internal Function Prototypes
//*********************************************************************************

//
// Extrapolates angles according to accelerometer readings
//
void CompAccelCalculate(ImuData *filter);

//
// Check to see which quadrant of the unit circle the angle lies in
// and format the angle to lie in the range of 0 to 2*pi
//
float FormatAccelRange(float accelAngle, float accelZ);

//
// Formats the Comp. Angle for faster filter convergence
//
float FormatFastConverge(float compAngle, float accAngle);

//
// Formats the complementary filter angle to always lie within the range of
// 0 to 2*pi
//
float FormatRange0to2PI(float compAngle);

//
// Complementary Filter - This is where the magic happens.
//
float CompFilterProcess(float compAngle, float accelAngle, float omega,
                        float deltaT, float alpha);

//*********************************************************************************
// External Functions
//*********************************************************************************

void CompInit(ImuData *filter, float deltaT, float tau)
{
    // Save value to structure
    filter->deltaT = deltaT;

    // Calculate weighting factor
    filter->alpha = tau/(tau + deltaT);

    // Initialize other structure variables
    filter->compAngleX = 0;
    filter->compAngleY = 0;
    filter->rawAngleX = 0;
    filter->rawAngleY = 0;
    filter->Ax = 0;
    filter->Ay = 0;
    filter->Az = 0;
    filter->Gx = 0;
    filter->Gy = 0;
    filter->Gz = 0;
}


void CompStart(ImuData *filter)
{
    // Calculate accelerometer angles
    CompAccelCalculate(filter);

    // Initialize filter to accel angles
    filter->compAngleX = filter->rawAngleX;
    filter->compAngleY = filter->rawAngleY;
}


void CompUpdate(ImuData *filter)
{
    // Calculate the accelerometer angles
    CompAccelCalculate(filter);

    // Omega is the rotational velocity reported by the gyroscope. Though it seems
    // strange, the rotational velocity about the Y axis must be projected back
    // onto the X axis and then its sense of direction must be inverted in order to
    // acquire positive angles about the X axis. This is shown below with -Gy being
    // passed as a parameter.
    filter->compAngleX = CompFilterProcess(filter->compAngleX, filter->rawAngleX,
                                           -(filter->Gy), filter->deltaT,
                                           filter->alpha);

    // In this case, the rotational velocity about the X axis (Gx) is projected back
    // onto the Y axis and its sense of direction is already correct.
    filter->compAngleY = CompFilterProcess(filter->compAngleY, filter->rawAngleY,
                                           filter->Gx, filter->deltaT,
                                           filter->alpha);
}


void CompAnglesGet(ImuData *filter, float *XAngle, float *YAngle)
{
    // Transfer structure's updated comp. filter's angles
    // Check if valid addresses were passed as well.
    if(XAngle)
    {
        *XAngle = filter->compAngleX;
    }
    if(YAngle)
    {
        *YAngle = filter->compAngleY;
    }
}


void CompAccelUpdate(ImuData *filter, float accelX, float accelY, float accelZ)
{
    // Save values to structure
    filter->Ax = accelX;
    filter->Ay = accelY;
    filter->Az = accelZ;
}


void CompGyroUpdate(ImuData *filter, float gyroX, float gyroY, float gyroZ)
{
    // Save values to structure
    filter->Gx = gyroX;
    filter->Gy = gyroY;
    filter->Gz = gyroZ;
}

//*********************************************************************************
// Internal Functions
//*********************************************************************************

void CompAccelCalculate(ImuData *filter)
{
    // Angle made by X axis acceleration vector relative to ground
    filter->rawAngleX = atan2f(filter->Ax, sqrtf(SQ(filter->Ay) + SQ(filter->Az)));

    // Angle made by Y axis acceleration vector relative to ground
    filter->rawAngleY = atan2f(filter->Ay, sqrtf(SQ(filter->Ax) + SQ(filter->Az)));

    // Format the accel. angles to lie in the range of 0 to 2*pi
    filter->rawAngleX = FormatAccelRange(filter->rawAngleX, filter->Az);
    filter->rawAngleY = FormatAccelRange(filter->rawAngleY, filter->Az);
}

float FormatAccelRange(float accelAngle, float accelZ)
{
    if(accelZ < 0.0f)
    {
        // Angle lies in Quadrant 2 or Quadrant 3 of
        // the unit circle
        accelAngle = PI - accelAngle;
    }
    else if(accelZ > 0.0f && accelAngle < 0.0f)
    {
        // Angle lies in Quadrant 4 of the unit circle
        accelAngle = TWO_PI + accelAngle;
    }

    // If both of the previous conditions were not satisfied, then
    // the angle must lie in Quadrant 1 and nothing more needs
    // to be done.

    return accelAngle;
}

float FormatFastConverge(float compAngle, float accAngle)
{
    // Work with comp. angles that are closest in distance to the accelerometer angle
    // on the unit circle. This allows for significantly faster filter convergence.
    if(compAngle > accAngle + PI)
    {
        compAngle = compAngle - TWO_PI;
    }
    else if(accAngle > compAngle + PI)
    {
        compAngle = compAngle + TWO_PI;
    }

    return compAngle;
}

float FormatRange0to2PI(float compAngle)
{
    while(compAngle >= TWO_PI)
    {
        compAngle = compAngle - TWO_PI;
    }

    while(compAngle < 0.0f)
    {
        compAngle = compAngle + TWO_PI;
    }

    return compAngle;
}

float CompFilterProcess(float compAngle, float accelAngle, float omega, float deltaT,
                  float alpha)
{
    float gyroAngle;

    // Speed up filter convergence
    compAngle = FormatFastConverge(compAngle, accelAngle);

    // Integrate the gyroscope's angular velocity reading to get an angle
    gyroAngle = compAngle + omega*deltaT;

    // Weighting is applied to the gyroscope's angular position and
    // accelerometer's angular position and they are put together to form one
    // angle, the complimentary filter angle.
    compAngle = alpha*gyroAngle + (1.0f - alpha)*accelAngle;

    // Format the Comp. Angle to lie in the range of 0 to 2*pi
    compAngle = FormatRange0to2PI(compAngle);

    return compAngle;
}
