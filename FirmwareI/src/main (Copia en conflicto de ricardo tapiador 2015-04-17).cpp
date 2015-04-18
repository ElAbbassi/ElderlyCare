#include "mbed.h"
#include "IMU.h"
#include"src/FreeImu Algorithm/FreeImu.h"
#include "math.h"
#include "SDFileSystem/SDFileSystem.h"
#include "XBee/xbee.h"
#include "Util.h"
#include "src/GPS/A2035_gps.h"
#include "c++/4.8.4/string"


Serial pc(USBTX, USBRX);
//xbee Xbee(PA_11 /* TX */, PA_12 /* RX */, D5);
xbee Xbee(PA_15 /* TX */, PB_7 /* RX */, PC_14);
IMU imu(I2C_SDA, I2C_SCL);
//IMU imu(PA_8, PB_4);
FreeImu updatesvalues(395.5f, 60.0f);
DigitalOut led(LED1);
DigitalIn button(USER_BUTTON);
Timer timer;
Timer sampletime;
//SDFileSystem sd(D11, D12, D13, D10, "sd"); // MOSI, MISO, SCK, CS
SDFileSystem sd(PB_5, PB_4, PB_3, PB_10, "sd"); // MOSI, MISO, SCK, CS
FILE *fp;
fpos_t pos[1] = { 0 };
Util util;
A2035_GPS gps(PA_11 /*tx*/, PA_12 /*rx*/, PB_2 /*nrst*/, PB_12 /*on_off*/);
GPS_information gps_info;

int longitud = 0;
int latitud = 0;
char dataread[3] = { 0, 0, 0 };
float qt[4] = { 0, 0, 0, 0 };
float accel[3];
float mag[3];
float gyroscope[3];
float timermeassure = 0;
float timer1 = 0;
float timer2 = 0;
float diferencesample;
float ypr[3];
char pr = 0;
int bandera = 0;
int main() {

	wait(2);
	util.WriteLog(fp, "/sd/log.csv", pos);
	imu.CalcOffset();
	while (1) {

		imu.read();
		imu.calibrate();
		imu.getaxis(gyroscope, accel, mag);

		timer.stop();
		timermeassure = timer.read_us();
		timer.reset();

		char * ax = (char*) &accel[0];
		char * ay = (char*) &accel[1];
		char * az = (char*) &accel[2];
		char * mx = (char*) &mag[0];
		char * my = (char*) &mag[1];
		char * mz = (char*) &mag[2];
		char * gx = (char*) &gyroscope[0];
		char * gy = (char*) &gyroscope[1];
		char * gz = (char*) &gyroscope[2];

		updatesvalues.Update(gyroscope[0] * (M_PI / 180.0f),
				gyroscope[1] * (M_PI / 180.0f), gyroscope[2] * (M_PI / 180.0f),
				accel[0], accel[1], accel[2], mag[0], mag[1], mag[2], qt);

		updatesvalues.getPithRollYaw(ypr);

		char * pitch = (char*) &ypr[1];
		char * roll = (char*) &ypr[2];
		char * yaw = (char*) &ypr[0];

		char * tim = (char*) &timermeassure;

		longitud = atoi(gps_info.longitude.c_str())*1000;
		latitud = atoi(gps_info.latitude.c_str())*1000;

		char *lon = (char*) &longitud;
		char *lat = (char*) &latitud;


		Xbee.sendypr(pitch, roll, yaw, tim,lon,lat);
		Xbee.sendRawData(ax, ay, az, mx, my, mz, gx, gy, gz, tim);

		Xbee.RecieveData(dataread, 1);

		if (dataread[0] == 'L') {
			bandera = 1;
			Xbee.sendlogdata(fp);
			sampletime.stop();
			pos[0]=0;
			util.WriteLog(fp, "/sd/log.csv", pos);

		} else if (dataread[0] == 'C') {
			imu.CalcOffset();
			bandera = 0;
		}
		if (bandera == 0) {
//tiempo entre muestras se coge el valor actual y le restamos el anterior
			timer1 = sampletime.read_ms();
			sampletime.stop();
			sampletime.reset();
			sampletime.start();
			diferencesample = timer1 - timer2;
			timer2 = timer1;
			util.WriteAllData(fp, "/sd/log.csv", ypr, accel, gyroscope, mag,
					pos, diferencesample);
			led = !led;
		} else {
			led = 1;
		}
		//Configuracion remota Giroscopo
		if (dataread[0] == 'A' || dataread[0] == 'B' || dataread[0] == 'D') {
			imu.giroscopo.selectsense(dataread[0]);
		}
		//Configuracion remota acelerometro
		if (dataread[0] == 'a' || dataread[0] == 'b' || dataread[0] == 'd') {
			imu.accelerometro.selectsenseaccel(dataread[0]);
		}
		//Configuracion remota magnetometro
		if (dataread[0] == '1' || dataread[0] == '2' || dataread[0] == '3') {
			imu.accelerometro.selectsensemag(dataread[0]);
		}
		dataread[0] = 0;

	/*	if (button == 0) // Button is pressed
		        {

		         gps.switch_on_off();
		        }

		        if(gps.is_gps_data_valid())
		        {
		          gps_info = gps.get_gpsInfo();
		            pc.printf("Latitud: %s Longitud: %s \r",  gps_info.latitude.c_str(), gps_info.longitude.c_str());
		            gps.clear_gps_data_valid();
		        }*/

	}

}

