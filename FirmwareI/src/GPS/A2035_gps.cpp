#include "A2035_gps.h"

A2035_GPS::A2035_GPS(PinName tx, PinName rx, PinName nreset, PinName on_off) 
    : data(tx, rx)//, pc(USBTX, USBRX)
{
    _nreset = nreset;
    _on_off = on_off;
    
    _working_flag = false;
    
    data.baud(4800);
    data.format(8, SerialBase::None, 1);
    data.attach(this, &A2035_GPS::rxCallback, Serial::RxIrq);
    
    gps_data_valid = false;
    nmea_data = "";
}

A2035_GPS::~A2035_GPS()
{}

void A2035_GPS::rxCallback()
{
    char gps_data = data.getc();
    A2035_GPS::create_gps_nmea(gps_data);
}

void A2035_GPS::create_gps_nmea(char gps_data)
{
    if(gps_data != 0x0A) //Line feed char
    {
        nmea_data += gps_data;
    }
    else
    {
        string stream = nmea_data;
        nmea_data = "";
        A2035_GPS::parse_nmea_data(stream);
    }
}

void A2035_GPS::parse_nmea_data(string stream)
{
    int ref_index = 0;
    string values[50];
    int value_index = 0;
    
    for(int i=0; i<stream.size(); i++)
    {
        if(stream[i] == ',')
        {
            values[value_index] = (i-ref_index != 0) ? stream.substr(ref_index, (i-ref_index)) : " ";
            value_index++;
            ref_index = i + 1;
        }
    }
    
    values[value_index] = stream.substr(ref_index);
    
    if(values[0] != "$GPGGA") //Only .$GPGGA streams are processed
        return;
         
    gps_info.time = values[1];
    gps_info.latitude = values[2] + "," + values[3];
    gps_info.longitude = values[4] + "," + values[5];
    gps_info.fix_quality = values[6];
    gps_info.num_satellites = values[7];
    gps_info.hdp = values[8];
    gps_info.altitude = values[9] + "," + values[10];
    gps_info.HoG = values[11] + "," + values[12];
    gps_info.checksum = values[14];
    
    gps_data_valid = true;
}

void A2035_GPS::switch_on_off()
{
    DigitalOut on_off_pin(_on_off);
    
    on_off_pin.write(1);
    wait(0.2); //200 ms
    on_off_pin.write(0);
    
    _working_flag = !_working_flag;
}

bool A2035_GPS::is_working()
{
    return _working_flag;
}

GPS_information A2035_GPS::get_gpsInfo()
{
    return gps_info;
}

bool A2035_GPS::is_gps_data_valid()
{
    return gps_data_valid;
}

void A2035_GPS::clear_gps_data_valid()
{
    gps_data_valid = false;
}
    