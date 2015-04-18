
#ifndef __LSM303DLHC_H
#define __LSM303DLHC_H
#include "mbed.h"



class LSM303DLHC {
    public:
        /** Create a new interface for an LSM303DLHC
         *
         * @param sda is the pin for the I2C SDA line
         * @param scl is the pin for the I2C SCL line
         */
        LSM303DLHC(PinName sda, PinName scl);

  
        /** read the raw accelerometer and compass values
         *
         * @param ax,ay,az is the accelerometer 3d vector, written by the function
         * @param mx,my,mz is the magnetometer 3d vector, written by the function
         */
        bool read(float * accel,float*mag);
        bool CalcOffset(float * acceloff,float *magoff);
        bool calibrate(float * accel,float*magnetometer,float * acceloff,float*magnetoff);
        void selectsenseaccel(char select);
        void selectsensemag(char select);
        float senseaccel;
        float magxysense;
               float magzsense;
    private:
        I2C _LSM303;

         
        float ax, ay, az;
        float mx, my, mz;         
         
        bool write_reg(int addr_i2c,int addr_reg, char v);
        bool read_reg(int addr_i2c,int addr_reg, char *v);
        bool recv(char sad, char sub, char *buf, int length);
};

#endif
