


#include <math.h> // fmod
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctime>

#include "time.h"

#define M_PI       3.14159265358979323846
#define M_PI_2     1.57079632679489661923
#define M_PI_4     0.785398163397448309616


// convert string int ISO 8601-like format [+/-]YYYY-MM-DDThh:mm:ss (no timzone offset)
// to julian day
int string_to_jday(string date, double &jd)
{
	char tmp;
	int year, month, day, hour, minute, second;
	year = month = day = hour = minute = second = 0;

	std::istringstream dstr( date );

	// TODO better error checking
	dstr >> year >> tmp >> month >> tmp >> day >> tmp >> hour >> tmp >> minute >> tmp >> second;

	// bounds checking (per s_tui time object)
	if( year > 100000 || year < -100000 ||
	        month < 1 || month > 12 ||
	        day < 1 || day > 31 ||
	        hour < 0 || hour > 23 ||
	        minute < 0 || minute > 59 ||
	        second < 0 || second > 59) return 0;


	// code taken from s_tui.cpp
	if (month <= 2)
	{
		year--;
		month += 12;
	}

	// Correct for the lost days in Oct 1582 when the Gregorian calendar
	// replaced the Julian calendar.
	int B = -2;
	if (year > 1582 || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
	{
		B = year / 400 - year / 100;
	}

	jd = ((floor(365.25 * year) +
	       floor(30.6001 * (month + 1)) + B + 1720996.5 +
	       day + hour / 24.0 + minute / 1440.0 + second / 86400.0));

	return 1;
}

/* Calculate the julian day from a calendar day.
 * Valid for positive and negative years but not for negative JD.
 * Formula 7.1 on pg 61 */
 // Code originally from libnova which appeared to be totally wrong... New code from celestia
double get_julian_day (const ln_date * cdate)
{
	ln_date date;
	int y, m, B;

	date = *cdate;

	y = date.years;
	m = date.months;
	if (date.months <= 2)
	{
		y = date.years - 1;
		m = date.months + 12;
	}

    // Correct for the lost days in Oct 1582 when the Gregorian calendar
    // replaced the Julian calendar.
	B = -2;
	if (date.years > 1582 || (date.years == 1582 && (date.months > 10 || (date.months == 10 && date.days >= 15))))
	{
		B = y / 400 - y / 100;
	}

	return (floor(365.25 * y) +
			floor(30.6001 * (m + 1)) + B + 1720996.5 +
			date.days + date.hours / 24.0 + date.minutes / 1440.0 + date.seconds / 86400.0);
}

// Calculate tm struct from julian day
void get_tm_from_julian(double JD, struct tm * tm_time)
{
	ln_date date;
	get_date(JD, &date);
	tm_time->tm_sec = (int)date.seconds;
	tm_time->tm_min = date.minutes;
	tm_time->tm_hour = date.hours;
	tm_time->tm_mday = date.days;
	tm_time->tm_mon = date.months - 1;
	tm_time->tm_year = date.years - 1900;
	tm_time->tm_isdst = -1;
}

/* Calculate the date from the Julian day
 * params : JD Julian day, date Pointer to new calendar date. */
 // Code originally from libnova which appeared to be totally wrong... New code from celestia
void get_date(double jd, ln_date * date)
{
	int a, d, e, f;
	double c, b;
	double dday, dhour, dminute;

	a = (int) (jd + 0.5);

	if (a < 2299161)
	{
		c = a + 1524;
	}
	else
	{
		b = (int) ((a - 1867216.25) / 36524.25);
		c = a + b - (int) (b / 4) + 1525;
	}

	d = (int) ((c - 122.1) / 365.25);
	e = (int) (365.25 * d);
	f = (int) ((c - e) / 30.6001);

	dday = c - e - (int) (30.6001 * f) + ((jd + 0.5) - (int) (jd + 0.5));

    /* This following used to be 14.0, but gcc was computing it incorrectly, so
	it was changed to 14 */
	date->months = f - 1 - 12 * (int) (f / 14);
	date->years = d - 4715 - (int) ((7.0 + date->months) / 10.0);
	date->days = (int) dday;

	dhour = (dday - date->days) * 24;
	date->hours = (int) dhour;

	dminute = (dhour - date->hours) * 60;
	date->minutes = (int) dminute;

	date->seconds = (dminute - date->minutes) * 60;
}


/* Calculate gmt date from system date.
 * param : date Pointer to store date. */
void get_ln_date_from_sys(ln_date * date)
{
	time_t rawtime;
	struct tm * ptm;

	/* get current time */
	time ( &rawtime );

	/* convert to gmt time representation */
	ptm = gmtime ( &rawtime );

	/* fill in date struct */
	date->seconds = ptm->tm_sec;
	date->minutes = ptm->tm_min;
	date->hours = ptm->tm_hour;
	date->days = ptm->tm_mday;
	date->months = ptm->tm_mon + 1;
	date->years = ptm->tm_year + 1900;
}

/* Calculate julian day from system time. */
double get_julian_from_sys(void)
{
	ln_date date;
	/* get sys date */
	get_ln_date_from_sys(&date);
	return get_julian_day(&date);
}

// Calculate time_t from julian day
time_t get_time_t_from_julian(double JD)
{
	struct tm loctime;
	ln_date date;

	get_date(JD, &date);

	loctime.tm_sec = (int)date.seconds;
	loctime.tm_min = date.minutes;
	loctime.tm_hour = date.hours;
	loctime.tm_mday =date.days;
	loctime.tm_mon = date.months -1;
	loctime.tm_year = date.years - 1900;
	loctime.tm_isdst = -1;

	return mktime(&loctime);
}


// Return the number of hours to add to gmt time to get the local time in day JD
// taking the parameters from system. This takes into account the daylight saving
// time if there is. (positive for Est of GMT)
// TODO : %z in strftime only works on GNU compiler
// Fixed 31-05-2004 Now use the extern variables set by tzset()
float get_GMT_shift_from_system(double JD, bool _local)
{
	/* Doesn't seem like MACOSX is a special case... ??? rob
#if defined( MACOSX ) || defined(WIN32)
	struct tm *timeinfo;
	time_t rawtime; time(&rawtime);
	timeinfo = localtime(&rawtime);
	return (float)timeinfo->tm_gmtoff/3600 + (timeinfo->tm_isdst!=0);
#else */

     struct tm *timeinfo;
     time_t rawtime;
	 time(&rawtime);
	 timeinfo = localtime(&rawtime);
	 return -(float)timezone/3600 + (timeinfo->tm_isdst!=0);

}

// Return the time zone name taken from system locale
wstring get_time_zone_name_from_system(double JD)
{
	// Windows will crash if date before 1970
	// And no changes on Linux before that year either
	// TODO: ALSO, on Win XP timezone never changes anyway???
	if(JD < 2440588 ) JD = 2440588;

	// The timezone name depends on the day because of the summer time
	time_t rawtime = get_time_t_from_julian(JD);

	struct tm * timeinfo;
	timeinfo = localtime(&rawtime);
	static char timez[255];
	timez[0] = 0;
	//StelUtils::my_strftime(timez, 254, "%Z", timeinfo);
	//return StelUtils::stringToWstring(timez);
    return 0;
}



// Return the time in ISO 8601 format that is : %Y-%m-%d %H:%M:%S
string get_ISO8601_time_UTC(double JD)
{
	struct tm time_utc;
	get_tm_from_julian(JD, &time_utc);

	static char isotime[255];
	//StelUtils::my_strftime(isotime, 254, "%Y-%m-%d %H:%M:%S", &time_utc);
	return isotime;
}

