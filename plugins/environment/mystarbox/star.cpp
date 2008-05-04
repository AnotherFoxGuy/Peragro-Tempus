/*
    Copyright (C) 2008 by Mogue Carpenter

    This file is part of mystarbox plugin for crystalspace.

    mystarbox is free software; you can redistribute it and/or modify
    it under the terms of the Lesser GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    mystarbox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    Lesser GNU General Public License for more details.

    You should have received a copy of the Lesser GNU General Public License
    along with mystarbox; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "star.h"

// --------------------------------------------------------------------------------//
// Star member functions ----------------------------------------------------------//
// --------------------------------------------------------------------------------//
Star::Star (   std::string star_name, 
					std::string new_classification,
					float new_luminosity,
					int color,
					System* parent_system )  {


	name = star_name;
	classification = new_classification;
	luminosity = new_luminosity;

	mass = pow ( cbrt(luminosity) , 2 ) ;
	starcolor = color;
	system = parent_system;



//	printf ( "SUN mass:%2.19f \n" , mass);
}

Star::~Star() {
}

