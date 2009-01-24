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

/*
 * star.h
 *
 * A this class holds the stars information
 *
 */

#ifndef _H_STARBOX_SYSTEM_STAR_
#define _H_STARBOX_SYSTEM_STAR_

#include <cssysdef.h>
#include <crystalspace.h>

#include <string>
#include <cstdlib>

class System;

// --------------------------------------------------------------------------------//
// Star, stores all the information about a star
// --------------------------------------------------------------------------------//
class Star{

  private:
    std::string name;
    std::string classification;
    float luminosity;
    System* system;

    float mass;
    int starcolor;

    iTextureWrapper* star_tex;
    csRefArray<iTextureWrapper> star_textures;
    float last_tex;

    int star_tex_size_px;     // ^2 image
    float star_scale;  // calculated from apr_mag

  public:

    Star (
      std::string star_name,
      std::string new_classification,
      float new_luminosity,
      int color,
      iTextureWrapper* tex,
      System* parent_system
    );

    ~Star();

    std::string Get_Name() { return (name); };
    std::string Get_Classification() { return (classification); };

    float Get_Luminosity() { return (luminosity); };
    float Get_Apr_Magnitude();
    float Get_Mass() { return mass; };
    float Get_Color() { return starcolor; };


    iTextureWrapper* Get_Texture() { return star_tex; };
    void Set_Texture(iTextureWrapper* tex);
    float GetStarScale (const float& dist, const float& base_star_size, const float& apr_mag_exp); 
    void SetScale(float scale) {star_scale = scale; };

    int Get_Type();

//    void DrawStar3D(iGraphics3D* g3d, const iCamera* c);
    void DrawStar3D(iGraphics3D* g3d, const iCamera* c,const float& base_star_size, const float& apr_mag_exp);
    void DrawStar2D(iGraphics2D* g2d, const iCamera* c);

  private:
    int Adjust_Perspective_Height(int y,iGraphics2D* g2d);

};

#endif
