/*
 * orbit.h
 *
 * Base Class for calculating orbit's and positions 
 * 
 */

#ifndef __ORBIT_H
#define __ORBIT_H

// CS Files
#include <cssysdef.h>
#include <crystalspace.h>

#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <vector>

#define SB_AU_KM (149597870.691)  
#define SB_SCALE_LY_UNIT (100000)    // number of CS world units per light year 

#ifndef PI
	#define PI 3.14159265358979323
#endif
#define THRESH 1.e-8
#define CUBE_ROOT( X)  (exp( log( X) / 3.))

// --------------------------------------------------------------------------------//
// 
// --------------------------------------------------------------------------------//
class Orbit
{

	private:

	//	Orbit* parent ;  // Parent body, Null if none

		long double i ; // inclination	
		long double n ; // longatude of ascending node 
		long double w ; // Argument of periapsis
		long double e ; // eccentricity
		long double a ; // semi-magor axis
//		long double M ; // mean anomaly at epoch , used as mean anomaly at systime
		long double orb_period ; 

		double scale ;  
		long double scalefactor ;

	public:
		Orbit() ;
		~Orbit() ;

		void Set_i ( long double val ) { i = val ; } ;
		void Set_n ( long double val ) { n = val ; } ;
		void Set_w ( long double val ) { w = val ; } ;
		void Set_e ( long double val ) { e = val ; } ;
		void Set_a ( long double val ) { a = val ; } ;
		void Set_period ( long double val ) { orb_period = val ; } ;
		void Set_scale ( long double val ) { scale = val ; scalefactor = SB_AU_KM  / scale ; } ;

		long double const Get_i () { return  i ; } ;
		long double const Get_n () { return  n ; } ;
		long double const Get_w () { return  w ; } ;
		long double const Get_e () { return  e ; } ;
		long double const Get_a () { return  a ; } ;
		long double const Get_period () { return  orb_period ; } ;
		long double const Get_scale () { return  scale ; } ;

		void Draw_Orbit ( iCamera* c, iGraphics3D* g3d,  csVector3 origin );
		void Draw_Plane (const iCamera* c, iGraphics3D* g3d ) ;
		void Draw_Position ( iCamera* c, iGraphics3D* g3d, csVector3 origin, timespec systime ) ;
		void Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up);

		csVector3 CirclePoint ( long double angle ) ;  // point on circle at angle 
		csVector3 EllipsePoint ( long double angle ) ; // point on elipse at angle 
		csVector3 OrbitPoint ( long double angle ) ; // point on orbit at angle
		csVector3 OrbitPoint ( timespec systime ) ;// point on orbit at systime


		long double Orbit_Angle( timespec systime ); 

		// from http://www.projectpluto.com/home.htm // GPL 
		double asinh( const double z ) ;
		double kepler( const double ecc, double mean_anom ) ;

		
};

#endif
// __ORBIT_H
