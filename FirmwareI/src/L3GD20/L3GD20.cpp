/**
 * Copyright (c) 2011 Pololu Corporation.  For more information, see
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
#include "L3GD20.h"

// Defines ////////////////////////////////////////////////////////////////

// The Arduino two-wire interface uses a 7-bit number for the address, 
// and sets the last bit correctly based on reads and writes
// mbed I2C libraries take the 7-bit address shifted left 1 bit
// #define GYR_ADDRESS (0xD2 >> 1)
#define GYR_ADDRESS 0xD6

// Public Methods //////////////////////////////////////////////////////////////

// Constructor
L3GD20::L3GD20(PinName sda, PinName scl) :
		_L3GD20(sda, scl) {
	//Frecuencia de operacion del I2C
	_L3GD20.frequency(100000);

	//ODR 380Hz y habilitar ejes     
	write_reg(GYR_ADDRESS, L3GD20_CTRL_REG1, 0xBF);

	// Escala de 2000dps y actualizacion continua de datos
	/*write_reg(GYR_ADDRESS, 0x23, 0x30);
sense = */
	selectsense('A');
}

bool L3GD20::read(float * gyro) {
	char gyr[6];

	//si la lectura se produce de forma correcta
	if (recv(GYR_ADDRESS, L3GD20_OUT_X_L, (char *) gyr, 6)) {

		//obtenemos los datos
		gyro[0] = float(short(gyr[1] << 8 | gyr[0])) * sense;
		gyro[1] = float(short(gyr[3] << 8 | gyr[2])) * sense;
		gyro[2] = float(short(gyr[5] << 8 | gyr[4])) * sense;

		return true;
	}

	return false;
}

bool L3GD20::CalcOffset(float * girooff) {

	float tempoff[3];
	float offset[3];

	for (int i = 0; i < 60; i++) {
		this->read(tempoff);
		offset[0] += tempoff[0];
		offset[1] += tempoff[1];
		offset[2] += tempoff[2];
	}
	girooff[0] = (offset[0]) / 60.0f;
	girooff[1] = (offset[1]) / 60.0f;
	girooff[2] = (offset[2]) / 60.0f;

}
bool L3GD20::calibrate(float * gyro, float * giroff) {

	gyro[0] = gyro[0] - giroff[0];
	gyro[1] = gyro[1] - giroff[1];
	gyro[2] = gyro[2] - giroff[2];
}

bool L3GD20::write_reg(int addr_i2c, int addr_reg, char v) {
	char data[2] = { addr_reg, v };
	return L3GD20::_L3GD20.write(addr_i2c, data, 2) == 0;
}

bool L3GD20::read_reg(int addr_i2c, int addr_reg, char *v) {
	char data = addr_reg;
	bool result = false;

	__disable_irq();
	if ((_L3GD20.write(addr_i2c, &data, 1) == 0)
			&& (_L3GD20.read(addr_i2c, &data, 1) == 0)) {
		*v = data;
		result = true;
	}
	__enable_irq();
	return result;
}

bool L3GD20::recv(char sad, char sub, char *buf, int length) {
	if (length > 1)
		sub |= 0x80;

	return _L3GD20.write(sad, &sub, 1, true) == 0
			&& _L3GD20.read(sad, buf, length) == 0;
}
void L3GD20::selectsense(char select) {
	switch(select){
	case 'A':
		write_reg(GYR_ADDRESS, 0x23, 0x30);
		sense= 0.07f;
		break;
	case 'B':
		write_reg(GYR_ADDRESS, 0x23, 0x10);
				sense= 0.0175f;
	break;

	case 'D':
		write_reg(GYR_ADDRESS, 0x23, 0x00);
		sense= 0.00875f;
		break;
	default:
		write_reg(GYR_ADDRESS, 0x23, 0x30);
				sense= 0.07f;
	}
}
