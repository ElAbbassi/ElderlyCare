#include "mbed.h"
#include "L3GD20/L3GD20.h"
#include "LSM303DLHC/LSM303DLHC.h"

class IMU
{
public:

	IMU(PinName sda, PinName scl);

	bool read();
	bool CalcOffset();
	bool calibrate();
	bool getaxis(float * gaxis,float * a_axis,float * maxis);
	bool getoffsets(float * goff,float * aoff,float * moff);
	float getax();
	float getay();
	float getaz();
	float getgx();
	float getgy();
	float getgz();
	float getmx();
	float getmy();
	float getmz();
	L3GD20 giroscopo;
	LSM303DLHC accelerometro;

private:


	float  accel[3];
	float  mag[3];
	float  gyro[3];
    float  gyrooff[3];
    float  acceloff[3];
    float  magoff[3];

};

