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
#include "star.h"

// --------------------------------------------------------------------------------//
// Star member functions ----------------------------------------------------------//
// --------------------------------------------------------------------------------//
Star::Star (   std::string star_name, 
    std::string new_classification,
    float new_luminosity,
    int color,
    iTextureWrapper* tex ,
    System* parent_system ) 

{
  name = star_name;
  classification = new_classification;
  luminosity = new_luminosity;

  mass = pow ( pow(luminosity, 1.0f/3) , 2 );
  starcolor = color;
  system = parent_system;
  star_tex = tex ;
//  printf ( "SUN mass:%2.19f \n" , mass);
}

Star::~Star() {
}

int Star::Get_Type()
{

  std::string tmp_str = classification.substr(0,1);

  switch ( tmp_str[0] ) {
    case 'O':
      return 0;
      break; 
    case 'B':
      return 1;
      break; 
    case 'A':
      return 2;
      break; 
    case 'F':
      return 3;
      break; 
    case 'G':
      return 4;
      break; 
    case 'K':
      return 5;
      break; 
    case 'M':
      return 6;      
      break; 
    default: 
      return 7;      
  }
  return 7;
}
