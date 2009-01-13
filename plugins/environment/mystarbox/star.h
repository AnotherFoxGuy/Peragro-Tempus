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

static float logtable[] = {0,0.30103,0.477121,0.60206,0.69897,0.778151,0.845098,0.90309,0.954243,1,1.04139,1.07918,1.11394,1.14613,1.17609,1.20412,1.23045,1.25527,1.27875,1.30103,1.32222,1.34242,1.36173,1.38021,1.39794,1.41497,1.43136,1.44716,1.4624,1.47712,1.49136,1.50515,1.51851,1.53148,1.54407,1.5563,1.5682,1.57978,1.59106,1.60206,1.61278,1.62325,1.63347,1.64345,1.65321,1.66276,1.6721,1.68124,1.6902,1.69897,1.70757,1.716,1.72428,1.73239,1.74036,1.74819,1.75587,1.76343,1.77085,1.77815,1.78533,1.79239,1.79934,1.80618,1.81291,1.81954,1.82607,1.83251,1.83885,1.8451,1.85126,1.85733,1.86332,1.86923,1.87506,1.88081,1.88649,1.89209,1.89763,1.90309,1.90849,1.91381,1.91908,1.92428,1.92942,1.9345,1.93952,1.94448,1.94939,1.95424,1.95904,1.96379,1.96848,1.97313,1.97772,1.98227,1.98677,1.99123,1.99564,2};
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

    int Get_Type();

//    void DrawStar3D(iGraphics3D* g3d, const iCamera* c);
    void DrawStar3D(iGraphics3D* g3d, const iCamera* c,const float& base_star_size, const float& apr_mag_exp);
    void DrawStar2D(iGraphics2D* g2d, const iCamera* c);

  private:
    int Adjust_Perspective_Height(int y,iGraphics2D* g2d);

};

#endif
