#include "mbed.h"
class FreeImu
{
public:

	FreeImu(float SampleFreq,float betal);

		bool Update(float gx, float gy, float gz, float ax, float ay, float az, float mx, float my, float mz,float * qt);
		bool Update(float gx, float gy, float gz, float ax, float ay, float az,float * qt);
		bool getQuaternions(float * q);
		bool getPithRollYaw(float *ypr);
		float invSqrt(float x);
		float getSampleperiod();

private:

		float SampleFreq;
		float beta;
        float q0,q1,q2,q3;
        float alpha;
};
