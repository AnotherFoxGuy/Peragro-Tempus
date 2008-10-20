/*
    Copyright (C) 2008 by Mogue Carpenter

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "orbit.h"

// --------------------------------------------------------------------------------//
//  Orbit member functions 	-----------------------------------------------------//
// --------------------------------------------------------------------------------//
Orbit::Orbit( ) 
//i,n,w,e,a,per 
{
	// i,n,w orientate the ellipse  
	i = 7.25; // inclination	
	n = 348.73936; // longatude of ascending node 
	w = 10; // Argument of periapsis

   // e and a give the shape of ellipes
	e = 0.0096710219; // eccentricity
	a = 1.10000001124; // semi-major axis AU

	// gives time for one orbit	
	orb_period = 0.52; // orbital period in days 
	// M = 0; // mean anomaly at epoch  ?? 2*PI*(t/T) 
	// perihelion = "4/1/08"; closest approach to sun.

	//scale = 100000;
	scalefactor = ( (SB_AU_KM*40)/100000 ); // turns AU into KM // convert AU to CS-units
}


Orbit::~Orbit() 
{
}

long double Orbit::Orbit_Angle( long secondspassed )
{
	long orbit_in_seconds;
	long orbit_remainder;
	long double orb_angle;
	long double M; // is the mean anomaly, 2πt/T

	long seconds = secondspassed;
	// Get number of seconds for one orbit, work out current angle
	orbit_in_seconds = orb_period * 24 * 60 * 60;
	if (orbit_in_seconds == 0) return 0; 
//	printf ("Orbit in Sec : %d \n",orbit_in_seconds );
	orbit_remainder = seconds %  orbit_in_seconds;
//	printf ("Orbit remainder : %d \n",orbit_remainder );
	M = 2 * PI * (double(orbit_remainder) / double(orbit_in_seconds));
//	printf ("Orbit Mean Anon : %Lf \n", M );
//	orb_angle = (180 / PI ) * ( kepler ( e , M ) );  
	orb_angle = ( 360 * (float(orbit_remainder) / float(orbit_in_seconds)) );
//	printf ("Orbit angle : %Lf \n", orb_angle );
	return orb_angle;  // eccentric anomaly 
}

csVector3 Orbit::OrbitPointSec ( long secondspassed ) 
{
	// Calculate deg into orbit based on time
	return OrbitPointSec (Orbit_Angle( secondspassed ));
}

csVector3 Orbit::OrbitPointDeg ( long double angle ) 
{
	csVector3 pos;
	double b; // minor axis
	double fx; // focus
	csMatrix3 rot;
	csXRotMatrix3  rotx(n * (PI/180.0) );
	csYRotMatrix3  roty(i * (PI/180.0) );
	csZRotMatrix3  rotz(w * (PI/180.0) );
	csTransform trans;

	pos = EllipsePoint( angle );
	// calculate the positive fucus on the major axis
	b = sqrt ( (a*a) - (e*e) * (a*a) );
	fx = scalefactor * sqrt ( (a*a)-(b*b) );
	 
	pos.x -= fx; // move the focus to (0,0,0)

	// Rotate the orbit to right orinitation
	rot = rotx * roty * rotz;  // get full rotation on x and y axis
	pos = rot * pos;
 
	return pos;
}

csVector3 Orbit::CirclePoint ( long double angle ) 
{
	csVector3 pos;
	angle = angle*(PI/180);
	pos.x = scalefactor * a * cos( angle );
	pos.y = scalefactor * a * sin( angle );
	pos.z = 0;

	return pos;
}

/******************************************************************************/
// x1 - x coordinate of the most left point of the ellipse
//   x2 - x of the most right point of the ellipse
//   y1 - y of the lowest point of ellipse
//   y2 - y of the highest point of ellipse
//
/******************************************************************************/
csVector3 Orbit::EllipsePoint ( long double angle )
{
	double t, a1, b1, tinc, centx, centy;

	csVector3 pos, v3Parent_pos;

	double x2 = scalefactor * a;
	double x1 = 0 - x2;
//double y2 =  ( SB_AU_KM / sector_scale ) * bodydata->a; // Make circles not elispse

	// This still not right, think e used wrong ( Deg/rads? ).
	double y2 = scalefactor * ( sqrt ( (a*a) - (e*e) * (a*a) ) );
	double y1 = 0 - y2;

	t = angle * ( PI / 180.0 );

	a1 = fabs(.5 * (double)(x2 - x1));  // ellipse width
	b1 = fabs(.5 * (double)(y2 - y1));  // ellipse height 
	tinc = PI * 2 / (a1 + b1);

	// Getting the foucus? not sure it moves the enter of the elipse to the side?
	// Caused problems with some orbits
	centx = (double)((x1 + x2) + .5) * .5;
	centy = (double)((y1 + y2) + .5) * .5;

	// This moves the center of the elipse to the 0,0
	centx =   centy = 0;

	pos.x = centx + a1 * cos(t);
	pos.y = centy - b1 * sin(t);
	pos.z = 0;

	return pos;
}

void Orbit::Draw_Position ( iCamera* c , iGraphics3D* g3d , csVector3 origin, long secondspassed ) 
{
// calculate ecc // use Time and Period
// mean anomaly at epoch  ??
	long double angle;
	csVector3 center_pos;
	csVector3 orbit_pos;

	csVector3 v3CS , v3CE ,v3CP;

	csOrthoTransform csCameraOrth;
	int color;
	float fov;

	csRef<iGraphics2D> g2d = g3d->GetDriver2D();
	csCameraOrth = c->GetTransform ();
	angle = 10;
	int win_w = g2d->GetWidth ();
	int win_h = g2d->GetHeight ();

	c->SetPerspectiveCenter ( (win_w / 2)  , (win_h/2) );
	v3CP = c->GetTransform().GetOrigin();  //camera origin
 
	fov = g3d->GetPerspectiveAspect ();
	color = g3d->GetDriver2D ()->FindRGB  (0,255,0);

	center_pos.x = 0;
	center_pos.y = 0;
	center_pos.z = 0;

	orbit_pos = OrbitPointSec( secondspassed );

	center_pos =  csCameraOrth * (center_pos + origin ); // convert point to camera space 
	orbit_pos =  csCameraOrth * (orbit_pos + origin ); // convert point to camera space 
	g3d->DrawLine( center_pos, orbit_pos , fov, color);

}

void Orbit::Draw_Orbit (iCamera* c , iGraphics3D* g3d, csVector3 origin )
{
	csVector3  v3start, v3end;
	csVector3 v3CS , v3CE ,v3CP;
	csOrthoTransform csCameraOrth;

	float orb_rad;
	int color;
	float fov;
	
	if (!g3d) printf("Orbit::draw_orbit: Failed to locate 3D renderer!\n");
	Draw_Plane ( c , g3d);
	csRef<iGraphics2D> g2d = g3d->GetDriver2D();

	int win_w = g2d->GetWidth();
	int win_h = g2d->GetHeight();
	c->SetPerspectiveCenter((win_w / 2), (win_h/2));
	v3CP = c->GetTransform().GetOrigin();
	fov = g3d->GetPerspectiveAspect();
	color = g3d->GetDriver2D()->FindRGB(0,255,0);

	// Calculate Start point 
	orb_rad = 0 * (PI / 180.0);
	v3start = OrbitPointDeg(0);

	csCameraOrth = c->GetTransform ();
	v3CS =  csCameraOrth * (v3start+origin);   // convert point to camera space 
	//origin =  csCameraOrth * (origin);
//	v3CE = c->Perspective(v3start);
	for (int i= 0; i<361; i+=10)
	{
		// Calculate End point
		v3end = OrbitPointDeg(i);
		// Draw part of arc
		v3CE =  csCameraOrth * (v3end + origin); // convert point to camera space 
		g3d->DrawLine(v3CS, v3CE, fov, color);
//		g3d->DrawLine( origin , v3CE , fov, color);
		// Make end point start of next arc
		v3CS = v3CE;
	}
}
	

void Orbit::Draw_Plane (const iCamera* c , iGraphics3D* g3d) 
{
	csVector3  v3start, v3end;
	csVector3 v3s, v3e;
	int green, red, blue;
	int lp;	
	float fov;
	csOrthoTransform csCameraOrth;
	csCameraOrth = c->GetTransform();

	if (!g3d) printf("Orbit::draw_orbit: Failed to locate 3D renderer!\n");
	csRef<iGraphics2D> g2d = g3d->GetDriver2D();
	red = g3d->GetDriver2D()->FindRGB(255, 0, 0);
	green = g3d->GetDriver2D()->FindRGB(0, 255, 0);
	blue = g3d->GetDriver2D()->FindRGB(0, 0, 255);
	fov = g3d->GetPerspectiveAspect();

	v3start.x = 0;
	v3start.y = +10000;
	v3start.z = 0;

	v3end.x = 0;
	v3end.y = -10000;
	v3end.z = 0;

	for (lp= -100; lp<100; lp++)
	{ 
		v3start.x = lp * 100;
		v3end.x = lp * 100;
		// Draw part of arc
		v3s =  csCameraOrth * (v3start ); // convert point to camera space 
		v3e =  csCameraOrth * (v3end ); // convert point to camera space 
		g3d->DrawLine(v3s, v3e, fov, blue);

	}

	v3start.x = +10000;
	v3start.y = 0;
	v3start.z = 0;

	v3end.x = -10000;
	v3end.y = 0;
	v3end.z = 0;

	for ( lp= -100; lp<100; lp++)
	{ 
		v3start.y = lp * 100;
		v3end.y = lp * 100;
		// Draw part of arc
		v3s =  csCameraOrth * (v3start ); // convert point to camera space 
		v3e =  csCameraOrth * (v3end ); // convert point to camera space 
		g3d->DrawLine( v3s , v3e , fov, blue);

	}
}


void Orbit::Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up) 
{
	csVector3  v3start, v3end;
	csVector3 v3s, v3e;
	int green, red, blue;
	int lp;	
	float fov;
	csOrthoTransform csCameraOrth;
	csCameraOrth = c->GetTransform();

	if (!g3d) printf("Orbit::draw_orbit: Failed to locate 3D renderer!\n");
	csRef<iGraphics2D> g2d = g3d->GetDriver2D();
	red = g3d->GetDriver2D()->FindRGB(255, 0, 0);
	green = g3d->GetDriver2D()->FindRGB(0, 255, 0);
	blue = g3d->GetDriver2D()->FindRGB(0, 0, 255);
	fov = g3d->GetPerspectiveAspect();

	csVector3 origin(0,0,0);
// 	csVector3 up = ;
	g3d->DrawLine(csCameraOrth * (origin), (csCameraOrth * (csVector3(0,0,0) - up)), fov, red);

}



double Orbit::asinh( const double z)
{
	return 0;
}

double Orbit::kepler( const double ecc, double mean_anom)
{
	return 0;
}


 
