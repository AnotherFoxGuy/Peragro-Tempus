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

#ifndef __STARBOX_SYSTEM_H
#define __STARBOX_SYSTEM_H

// CS Files
#include <cssysdef.h>
#include <crystalspace.h>

#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

struct Cords {
  float x ;
  float y ;
  float z;
};

#include "star.h"

// --------------------------------------------------------------------------------//
// System, Class that give the position of a star system from earth
// --------------------------------------------------------------------------------//
class System {
private:
  int id;
  std::string name;
  float right_ascension;
  float declination;
  float distance;

  Cords pos;

  std::vector < Star* > stars ;

public:

  // Member Functions
  System();
  ~System();

  bool Init ( int Id, std::string Name , float Ra, float Dec, float Distance );

  int Get_Id () { return id; }
  std::string Get_Name () { return ( name ); }
  Cords Get_Pos () { return pos; }
  float Get_Ra () { return right_ascension; }
  float Get_Dec () { return declination; }
  float GetDistance(){ return distance; }
  float Get_lum();
  int Get_StarType ();  // only do first star

  Star* Get_Star() { return stars.front(); }



  bool Add_Star 
  ( 
    std::string new_name,
    std::string classification,
    float new_luminosity,
    int color,
    iTextureWrapper* tex
  );

  Cords Calculate_Cords
  ( 
    float Right_Ascension ,
    float Declination,
    float Distance 
  );

  csPixelCoord * Get_Pixel ();

  int GetPixelX(const int size){return int((right_ascension/360)*size); };
  int GetPixelY(const int size){
      return int( ( ((declination/180)*size) + (size/2) ) );
  };

};

#endif // __STARBOX_SYSTEM_H
