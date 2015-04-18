#include "Util.h"

Util::Util() {

}

void Util::WriteAllData(FILE * fp, char * buf, float * ypr, float * accel,
		float *gyroscope, float * mag, fpos_t * pos, float timersample) {
	fp = fopen("/sd/log.csv", "w");
	fpos_t posicion = pos[0];
	if (fp == NULL) {
	} else {

		fsetpos(fp, &posicion);
		fprintf(fp, "%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i;%i\n",
				(int) (ypr[2] * 1000), (int) (ypr[1] * 1000),
				(int) (ypr[0] * 1000), (int) (accel[0] * 1000),
				(int) (accel[1] * 1000), (int) (accel[2] * 1000),
				(int) (gyroscope[0] * 1000), (int) (gyroscope[1] * 1000),
				(int) (gyroscope[2] * 1000), (int) (mag[0] * 1000),
				(int) (mag[1] * 1000), (int) (mag[2] * 1000),
				(int) (timersample));
		fgetpos(fp, &posicion);
		pos[0] = posicion;

		fclose(fp);

	}

}
const char* Util::getfield(char* line, int num) {
	const char* tok;
	for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n")) {
		if (!--num)
			return tok;
	}
	return NULL;
}

void Util::WriteLog(FILE * fp, char *buf, fpos_t * pos) {
	fp = fopen(buf, "r");
	fpos_t posicion = pos[0];
	if (fp != NULL) {
		fclose(fp);
		remove(buf);

	}

	fp = fopen(buf, "w");
	if (fp == NULL) {

	} else {
		fsetpos(fp, &posicion);
		fprintf(fp,
				"Roll;Pitch;Yaw;AccelX;AccelY;AccelZ;GyroX;GyroY;GyroZ;MagX;MagY;MagZ;Timersample\n");
		fgetpos(fp, &posicion);
		pos[0] = posicion;
		fclose(fp);

	}
}

