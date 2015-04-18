/* Copyright (c) 2012 Tristan Hughes, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, 
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is 
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or 
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING 
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "xbee.h"
#include <stdio.h>
#include "Util.h"
#include "c++/4.8.4/sstream"
#include "c++/4.8.4/string"
#include "c++/4.8.4/iostream"
#include "c++/4.8.4/fstream"
xbee::xbee(PinName tx, PinName rx, PinName reset) {
	_tx = tx;
	_rx = rx;
	_reset = reset;
}

xbee::~xbee() {
}

int xbee::ConfigMode() {
	int a = 0;
	Serial DATA(_tx, _rx);
	wait(2);
	DATA.printf("+++");
	while (a != 75) {
		if (DATA.readable()) {
a		= DATA.getc();
	}
}
	wait(1);
	printf("config mode\n");
	return 1;
}

int xbee::GetSerial(int *serial_no) {
	int sh1, sh2, sh3, sl1, sl2, sl3, sl4;
	Serial DATA(_tx, _rx);
	wait_ms(50);
	DATA.printf("ATSL \r");
	DATA.scanf("%2x%2x%2x%2x", &sl1, &sl2, &sl3, &sl4);
	wait_ms(500);
	DATA.printf("ATSH \r");
	DATA.scanf("%2x%2x%2x", &sh1, &sh2, &sh3);

	serial_no[0] = sh1;
	serial_no[1] = sh2;
	serial_no[2] = sh3;
	serial_no[3] = sl1;
	serial_no[4] = sl2;
	serial_no[5] = sl3;
	serial_no[6] = sl4;

	return 1;
}

int xbee::SetKey(char* key) {
	Serial DATA(_tx, _rx);
	DATA.printf("ATEE 1 \r");

	DATA.scanf("%*s");
	wait_ms(1);
	DATA.printf("ATKY %s \r", key);
	DATA.scanf("%*s");
	return 1;
}

int xbee::WriteSettings() {
	Serial DATA(_tx, _rx);
	wait_ms(5);
	DATA.printf("ATWR \r");
	DATA.scanf("%*s");
	return 1;
}

int xbee::ExitConfigMode() {
	Serial DATA(_tx, _rx);
	wait_ms(5);
	DATA.printf("ATCN \r");
	DATA.scanf("%*s");
	return 1;
}

int xbee::SendData(char *data_buf) {
	Serial DATA(_tx, _rx);
	DATA.printf("%s", data_buf);
	return 1;
}
int xbee::sendypr(char * pitch, char * roll, char * yaw, char * tim,char * longitud,char * latitud) {
	Serial DATA(_tx, _rx);
	DATA.printf("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c", 0x7E, 20, pitch[0],
			pitch[1], pitch[2], pitch[3], roll[0], roll[1], roll[2], roll[3],
			yaw[0], yaw[1], yaw[2], yaw[3], tim[0], tim[1], tim[2], tim[3],longitud[0],longitud[1],latitud[0],latitud[1],
			0xAD);
	return 1;
}
int xbee::sendRawData(char * ax, char * ay, char * az, char * mx, char * my,
		char * mz, char * gx, char * gy, char * gz, char * tim) {
	Serial DATA(_tx, _rx);
	DATA.printf(
			"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
			0x7E, 40, ax[0], ax[1], ax[2], ax[3], ay[0], ay[1], ay[2], ay[3],
			az[0], az[1], az[2], az[3], mx[0], mx[1], mx[2], mx[3], my[0],
			my[1], my[2], my[3], mz[0], mz[1], mz[2], mz[3], gx[0], gx[1],
			gx[2], gx[3], gy[0], gy[1], gy[2], gy[3], gz[0], gz[1], gz[2],
			gz[3], tim[0], tim[1], tim[2], tim[3], 0x0D);

}

void xbee::RecieveData(char * data_buf, int numchar) {

	Serial DATA(_tx, _rx);
	int c = 0;
	for (int i = 0; i < numchar; i++) {
		if (DATA.readable()) {
			data_buf[i] = DATA.getc();

	}

}
}

int xbee::SetPanId(int pan_id) {
	Serial DATA(_tx, _rx);
	wait_ms(5);
	DATA.printf("ATID %i\r", pan_id);
	DATA.scanf("%*s");
	return 1;
}

void xbee::Reset() {
	DigitalOut rst(_reset);
	rst = 0;
	wait_ms(10);
	rst = 1;
	wait_ms(1);
}

void xbee::RecieveFrame(char *data_buf, int numchar) {
	int count = 0;
	if (numchar == 0) {
		numchar = sizeof(data_buf);
	}
	Serial DATA(_tx, _rx);
	while (numchar != count) {
		if (DATA.readable()) {
			*data_buf = DATA.getc();
			data_buf += 1;
			count++;
		}

	}
}
void xbee::sendlogdata(FILE * fp) {
	Serial DATA(_tx, _rx);
	Util util;

	ifstream infile("/sd/log.csv");
	string line = "";
	int i = 0;
	int data[12];
	while (getline(infile, line)) {

		stringstream strstr(line);
		string word = "";
		char line[1024];
		while (getline(strstr, word, ';')) {
			strcpy(line, word.c_str());
			//DATA.printf("%s\n",line);
			data[i] = atoi(line);
			i++;
		}
		i = 0;
		char * rolldata = (char*) &data[0];
		char * pitchdata = (char*) &data[1];
		char * yawdata = (char*) &data[2];
		char * ax = (char*) &data[3];
		char * ay = (char*) &data[4];
		char * az = (char*) &data[5];
		char * mx = (char*) &data[6];
		char * my = (char*) &data[7];
		char * mz = (char*) &data[8];
		char * gx = (char*) &data[9];
		char * gy = (char*) &data[10];
		char * gz = (char*) &data[11];
		char * tim = (char*) &data[12];
		DATA.printf(
				"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				0x7E, 26, rolldata[0], rolldata[1], pitchdata[0], pitchdata[1],
				yawdata[0], yawdata[1], ax[0], ax[1], ay[0], ay[1], az[0],
				az[1], mx[0], mx[1], my[0], my[1], mz[0], mz[1], gx[0], gx[1],
				gy[0], gy[1], gz[0], gz[1], tim[0], tim[1], 0x0F);

	}
	//Ack
	DATA.printf("%c%c%c%c", 0x7E, 1, 2, 0x0A);

}
