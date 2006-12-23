

#ifndef _TASS17_H_
#define _TASS17_H_


#define TASS17_MIMAS     0
#define TASS17_ENCELADUS 1
#define TASS17_TETHYS    2
#define TASS17_DIONE     3
#define TASS17_RHEA      4
#define TASS17_TITAN     5
#define TASS17_HYPERION  7
#define TASS17_IAPETUS   6

void GetTass17Coor(double jd,int body,double *xyz);


#endif
