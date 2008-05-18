/*
    Copyright (C) 2008 by Mogue Carpenter

    This file is part of mystarbox plugin for crystalspace.

    mystarbox is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    mystarbox is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with mystarbox; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "system.h"

// --------------------------------------------------------------------------------//
// System member functions --------------------------------------------------------//
// --------------------------------------------------------------------------------//
System::System() {
  id = 0;
}

System::~System() {
  // Delete all stars from this system first
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end();
      ++itr) {
      delete(*itr);
  } // end for iterate systems
}

// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
bool System::Init ( int Id, std::string Name , float Ra, float Dec, float Distance ) {

  id = Id;
  right_ascension = Ra;
  declination = Dec;
  distance = Distance;
  name = Name;
  // Calculate a coordinate position for this system
  pos = Calculate_Cords( Ra , Dec , Distance );
  return true;
}

float System::Get_lum() {
  // return the total luminosity of all stars in system
  float tot_lum = 0;
  
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end();
      ++itr) {
      tot_lum += (*itr)->Get_Luminosity();
  } // end for iterate stars
  return tot_lum;
}
// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
Cords System::Calculate_Cords( float ra , float dec ,float dis ) {
// Create 3D cords for each system
  Cords result;
  
  result.x = sin ( ra * (PI /180) ) * dis;
  result.y = sin ( dec * (PI / 180) ) * dis ;
  result.z = cos ( ra * (PI /180)) * ( cos (dec*(PI /180)) * dis) ;

//  printf ( " dis:%6.2f dec:%6.2f ra:%6.2f \n" , dis , dec , ra  );
//  printf ( " pos: ( %6.2f , %6.2f , %6.2f )\n " , result.x , result.y , result.z );
  
  return result;
}

// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
bool System::Add_Star ( std::string name, std::string classification,
               float luminosity, int color, iTextureWrapper* txt ) {
try {
    Star* star;
    star = new Star( name, classification , luminosity, color, txt , this );
    // printf( "%s\n" ,classification.c_str() );
    stars.push_back(star);
    return true;
  }
catch( char * str ) {
    printf ("Exception raised:System::Add_Star %s \n" , str );
    return false;
   }
    
}  


int System::Get_StarType ()
{
  // return the startype of the last star in system
  int startype = 0 ;
  
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end();
      ++itr) {
      startype += (*itr)->Get_Type();
  } // end for iterate stars
  return startype;  

};




