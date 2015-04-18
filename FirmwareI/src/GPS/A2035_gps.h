#include "mbed.h"
#include "string"

struct GPS_information
{
    string time;
    
    string latitude;
    string longitude;
    
    string fix_quality;
    string num_satellites;
    string hdp; // Horizontal dilution of position
    string altitude; //Altitude, Meters, above mean sea level
    string HoG; //Height of geoid (mean sea level) above WGS84 ellipsoid
    
    string checksum;
};
    
class A2035_GPS
{
private:
    PinName _nreset;
    PinName _on_off;
    bool _working_flag;
    
    Serial data;
    void rxCallback(void);
    
    void create_gps_nmea(char);
    string nmea_data;
    void parse_nmea_data(string);    
    
    GPS_information gps_info;
    bool gps_data_valid;
    
    //Serial pc;

public:
    A2035_GPS(PinName tx, PinName rx, PinName nreset, PinName on_off);
    ~A2035_GPS();

    void switch_on_off(void);
    bool is_working(void);
    
    GPS_information get_gpsInfo(void);
    bool is_gps_data_valid(void);
    void clear_gps_data_valid(void);
};