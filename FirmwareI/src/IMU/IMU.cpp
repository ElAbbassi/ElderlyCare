#include "mbed.h"
#include "L3GD20/L3GD20.h"
#include "LSM303DLHC/LSM303DLHC.h"
#include "IMU.h"

IMU::IMU(PinName sda, PinName scl) :
		giroscopo(sda, scl), accelerometro(sda, scl) {

}

bool IMU::read() {
	giroscopo.read(gyro);
	accelerometro.read(accel, mag);

}
bool IMU::CalcOffset() {
	giroscopo.CalcOffset(gyrooff);
	accelerometro.CalcOffset(acceloff, magoff);
}
bool IMU::calibrate() {
	giroscopo.calibrate(gyro, gyrooff);
	accelerometro.calibrate(accel, mag, acceloff, magoff);
}
bool IMU::getaxis(float * gaxis, float * a_axis, float * maxis) {
	gaxis[0] = gyro[0];
	gaxis[1] = gyro[1];
	gaxis[2] = gyro[2];
	a_axis[0] = accel[0];
	a_axis[1] = accel[1];
	a_axis[2] = accel[2];
	maxis[0] = mag[0];
	maxis[1] = mag[1];
	maxis[2] = mag[2];
}
float IMU::getax() {
	return accel[0];
}
float IMU::getay() {
	return accel[1];
}
float IMU::getaz() {
	return accel[2];
}
float IMU::getgx() {
	return gyro[0];
}
float IMU::getgy() {
	return gyro[1];
}
float IMU::getgz() {
	return gyro[2];
}
float IMU::getmx() {
	return mag[0];
}
float IMU::getmy() {
	return mag[1];
}
float IMU::getmz() {
	return mag[2];
}
bool IMU::getoffsets(float * girooff, float * acceleoff, float * magneoff) {
	girooff[0] = gyrooff[0];
	girooff[1] = gyrooff[1];
	girooff[2] = gyrooff[2];
	acceleoff[0] = acceloff[0];
	acceleoff[1] = acceloff[1];
	acceleoff[2] = acceloff[2];
	magneoff[0] = magoff[0];
	magneoff[1] = magoff[1];
	magneoff[2] = magoff[2];
}

