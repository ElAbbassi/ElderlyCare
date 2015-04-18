#include "mbed.h"

class Util {
public:

	Util();

	void WriteLog(FILE * fp,char *buf,fpos_t * pos);
	void WriteAllData(FILE * fp,char * buf,float * ypr,float * accel,float *gyroscope,float * mag,fpos_t * pos,float timer);
	const char* getfield(char* line, int num);
};
