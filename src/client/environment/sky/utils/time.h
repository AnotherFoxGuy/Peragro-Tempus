

#ifndef _S_TIME_H_
#define _S_TIME_H_

#include <string>
#include <map>
#include <ctime>


// Conversion in standar Julian time format
#define JD_SECOND 0.000011574074074074074074
#define JD_MINUTE 0.00069444444444444444444
#define JD_HOUR   0.041666666666666666666
#define JD_DAY    1.

// astonomical unit (km)
#define AU 149597870.691

// speed of light (km/sec)
#define SPEED_OF_LIGHT 299792.458

#define MY_MAX(a,b) (((a)>(b))?(a):(b))
#define MY_MIN(a,b) (((a)<(b))?(a):(b))

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616


// General Calendar Functions

// Human readable (easy printf) date format
typedef struct
{
	int years; 		/*!< Years. All values are valid */
	int months;		/*!< Months. Valid values : 1 (January) - 12 (December) */
	int days; 		/*!< Days. Valid values 1 - 28,29,30,31 Depends on month.*/
	int hours; 		/*!< Hours. Valid values 0 - 23. */
	int minutes; 	/*!< Minutes. Valid values 0 - 59. */
	double seconds;	/*!< Seconds. Valid values 0 - 59.99999.... */
}ln_date;


/* Calculate the date from the julian day. */
void get_date(double JD, ln_date * date);

/* Calculate julian day from system time. */
double get_julian_from_sys();

// Calculate tm struct from julian day
void get_tm_from_julian(double JD, struct tm * tm_time);

//! Return the number of hours to add to gmt time to get the local time at time JD
//! taking the parameters from system. This takes into account the daylight saving
//! time if there is. (positive for Est of GMT)
float get_GMT_shift_from_system(double JD, bool _local=0);

//! Return the time zone name taken from system locale
std::wstring get_time_zone_name_from_system(double JD);

//! Return the time in ISO 8601 UTC format that is : %Y-%m-%d %H:%M:%S
std::string get_ISO8601_time_UTC(double JD);

// convert string in ISO 8601-like format [+/-]YYYY-MM-DDThh:mm:ss (no timezone offset) to julian day
int string_to_jday(std::string date, double &jd);

#endif
