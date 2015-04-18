/** LSM303DLHC Interface Library
 *
 * base on code by Michael Shimniok http://bot-thoughts.com
 *
 *  and on test program by @tosihisa and 
 *
 *  and on Pololu sample library for LSM303DLHC breakout by ryantm:
 *
 * Copyright (c) 2011 Pololu Corporation. For more information, see
 *
 * http://www.pololu.com/
 * http://forum.pololu.com/
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
#include "mbed.h"
#include "LSM303DLHC.h"

const int addr_acc = 0x32;
const int addr_mag = 0x3c;

enum REG_ADDRS {
	/* --- Mag --- */
	CRA_REG_M = 0x00,
	CRB_REG_M = 0x01,
	MR_REG_M = 0x02,
	OUT_X_M = 0x03,
	OUT_Y_M = 0x05,
	OUT_Z_M = 0x07,
	/* --- Acc --- */
	CTRL_REG1_A = 0x20,
	CTRL_REG4_A = 0x23,
	OUT_X_A = 0x28,
	OUT_Y_A = 0x2A,
	OUT_Z_A = 0x2C,
};

bool LSM303DLHC::write_reg(int addr_i2c, int addr_reg, char v) {
	char data[2] = { addr_reg, v };
	return LSM303DLHC::_LSM303.write(addr_i2c, data, 2) == 0;
}

bool LSM303DLHC::read_reg(int addr_i2c, int addr_reg, char *v) {
	char data = addr_reg;
	bool result = false;

	__disable_irq();
	if ((_LSM303.write(addr_i2c, &data, 1) == 0)
			&& (_LSM303.read(addr_i2c, &data, 1) == 0)) {
		*v = data;
		result = true;
	}
	__enable_irq();
	return result;
}

LSM303DLHC::LSM303DLHC(PinName sda, PinName scl) :
		_LSM303(sda, scl) {
	char reg_v;
	_LSM303.frequency(100000);

	//acelerometro
	reg_v = 0;
//ODR 100Hz , ejes habilitados y dispositivo encendido
	reg_v |= 0x57; /* X/Y/Z axis enable. */
	write_reg(addr_acc, CTRL_REG1_A, reg_v);
//escala de 2G ,little endian y actualizacion continua
	/*write_reg(addr_acc, CTRL_REG4_A, 0x00); //0x08);
	 senseaccel = 1000.0f;*/
	selectsenseaccel('a');

//magnetometro
	//ODR 30Hz y deshabilitar sensor de temperatura
	write_reg(addr_mag, CRA_REG_M, 0x14);
	//escala de 1.3 gauss
	reg_v |= 0x01 << 5; /* +-1.3Gauss */

	selectsensemag('1');
	//modo continuo
	reg_v = 0;
	write_reg(addr_mag, MR_REG_M, reg_v);
}

bool LSM303DLHC::read(float * accel, float*magnetometer) {
	char acc[6], mag[6];

	if (recv(addr_acc, OUT_X_A, acc, 6) && recv(addr_mag, OUT_X_M, mag, 6)) {

		accel[0] = (float((short(acc[1] << 8 | acc[0])) >> 4) / senseaccel);
		accel[1] = (float(((short(acc[3] << 8 | acc[2]))) >> 4) / senseaccel);
		accel[2] = (float(((short(acc[5] << 8 | acc[4]))) >> 4) / senseaccel);

		magnetometer[0] = float(short(mag[0] << 8 | mag[1])) / magxysense;
				magnetometer[1] = float(short(mag[2] << 8 | mag[3])) / magzsense;
				magnetometer[2] = float(short(mag[4] << 8 | mag[5])) / magxysense;
		return true;
	}

	return false;
}
bool LSM303DLHC::CalcOffset(float * acceloff, float *magoff) {
	float tempacc[3], tempmag[3], offmag[3], offacc[3];
	for (int i = 0; i < 60; i++) {
		this->read(tempacc, tempmag);
		offacc[0] += tempacc[0];
		offacc[1] += tempacc[1];
		offacc[2] += tempacc[2];
	}
	acceloff[0] = offacc[0] / 60.0f;
	acceloff[1] = offacc[1] / 60.0f;
	acceloff[2] = offacc[2] / 60.0f - 1;

}
bool LSM303DLHC::calibrate(float * accel, float*magnetometer, float * acceloff,
		float*magnetoff) {

	accel[0] = accel[0] - acceloff[0];
	accel[1] = accel[1] - acceloff[1];
	accel[2] = accel[2] - acceloff[2];

	/*magnetometer[0]= magnetometer[0]-magnetoff[0];
	 magnetometer[1]= magnetometer[1]-magnetoff[1];
	 magnetometer[2]= magnetometer[2]-magnetoff[2];*/
}
bool LSM303DLHC::recv(char sad, char sub, char *buf, int length) {
	if (length > 1)
		sub |= 0x80;

	return _LSM303.write(sad, &sub, 1, true) == 0
			&& _LSM303.read(sad, buf, length) == 0;
}

void LSM303DLHC::selectsenseaccel(char select) {
	switch (select) {
	//2G
	case 'a':
		write_reg(addr_acc, CTRL_REG4_A, 0x00);
		senseaccel = 1000.0f;
		break;
		//4G
	case 'b':
		write_reg(addr_acc, CTRL_REG4_A, 0x10);
		senseaccel = 2000.0f;
		break;

	case 'd':
		//8G
		write_reg(addr_acc, CTRL_REG4_A, 0x20);
		senseaccel = 4000.0f;
		break;
	default:
		write_reg(addr_acc, 0x23, 0x30);
		senseaccel = 1000.0f;
	}

}
void LSM303DLHC::selectsensemag(char select) {
	switch (select) {

	case '1':
		write_reg(addr_mag, CRB_REG_M, 0x20);
		magxysense = 1100.0f;
		magzsense = 980.0f;
		break;

	case '2':
		write_reg(addr_mag, CRB_REG_M, 0x40);
		magxysense = 855.0f;
		magzsense = 760.0f;
		break;

	case '3':
		write_reg(addr_mag, CRB_REG_M, 0x60);
		magxysense = 670.0f;
		magzsense = 600.0f;
		break;
	default:
		write_reg(addr_mag, CRB_REG_M, 0x20);
		magxysense = 1100.0f;
		magzsense = 980.0f;
	}

}
