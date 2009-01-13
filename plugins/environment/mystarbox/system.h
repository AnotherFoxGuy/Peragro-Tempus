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

#ifndef _H_STARBOX_SYSTEM_
#define _H_STARBOX_SYSTEM_

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

#include "star.h"



// --------------------------------------------------------------------------------//
// System, Class that give the position of a star system from earth
// --------------------------------------------------------------------------------//
class System
{
  private:


//    std::vector < Star* > stars;  // declaring this below tex_nebula causes a seg fault in Add_Star at stars.push_back(star);

    int id;
    std::string name;
    float right_ascension;
    float declination;
    float distance;
    csVector3 system_pos;
    Star* system_star;

    bool flg_useTexStars;
    csRef<iTextureWrapper> tex_nebula;
    float nebula_w;
    float nebula_h;


    float tmp_rot_value;  // temp debug var

  public:

    // Member Functions
    System( int Id, std::string Name , float Ra, float Dec, float Distance );
    ~System();

    bool Init (int Id, std::string Name, float Ra, float Dec, float Distance);

    int Get_Id() { return id; }
    std::string Get_Name() { return (name); }

    csVector3 Get_Pos() { return system_pos; }
    csVector2 Get_Sc_Pt(const iCamera* c);
    csVector3 Get_SC_Point(const iCamera* c, iGraphics3D* g3d, csVector3 pt);

    float Get_Ra() { return right_ascension; }
    float Get_Dec() { return declination; }
    float Get_Distance() { return distance; }  // distance from origin in light years
    float Get_Distance(const iCamera* c);  // distance from camera in light years
    float GetDistanceOrigin(); // distance from origin in light years
    float Get_Luminosity();
    std::string Get_Classification() { return system_star->Get_Classification(); };
    float Get_Apr_Magnitude();
    int Get_StarType();  // only do first star
    int Get_SC_Height();
    int Get_SC_Width();
    bool Has_Nebula() { if (tex_nebula) return true; return false; };

    void Set_Position(csVector3 new_pos);
    void SetScale (const float& base_star_size,const float& apr_mag_exp );
    Star* Get_Star() { return system_star; }

    void DrawStar3D(iGraphics3D* g3d, const iCamera* c, const float& base_star_size,const float& apr_mag_exp);
    void DrawStar2D(iGraphics2D* g2d, const iCamera* c);

    bool Add_Star
    (
      std::string new_name,
      std::string classification,
      float new_luminosity,
      int color,
      iTextureWrapper* tex
    );

    bool Add_Nebula
    (
      std::string tex_nebula_name,
      float w,
      float h,
      iEngine* eng
    );



    void Calculate_Position();

    csPixelCoord* Get_Pixel();

    void DrawNebulaTexture(const iCamera* c, iGraphics3D* g3d);


    int GetPixelX(const int size) { return int((right_ascension/360)*size); };
    int GetPixelY(const int size)
    {
        return int((((declination/180)*size) + (size/2)));
    };


};
#endif
