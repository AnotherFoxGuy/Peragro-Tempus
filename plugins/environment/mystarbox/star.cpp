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
#include "const.h"
#include "system.h"
// --------------------------------------------------------------------------------//
// Star member functions ----------------------------------------------------------//
// --------------------------------------------------------------------------------//
Star::Star(std::string star_name,
           std::string new_classification,
           float new_luminosity,
           int color,
           iTextureWrapper* tex,
           System* parent_system)
{
  name = star_name;
  classification = new_classification;
  luminosity = new_luminosity;

  mass = pow(pow(luminosity, 1.0f / 3), 2);
  starcolor = color;
  system = parent_system;
  star_tex = tex ;
  //printf("SUN mass: %2.19f.\n", mass);
}

Star::~Star()
{
}

void Star::DrawStar2D(iGraphics2D* g2d, const iCamera* c)
{
  int w = g2d->GetWidth();
  int h = g2d->GetHeight();

  csVector3 starpos;
  csVector3 cp1;
  float distance = system->Get_Distance(c);

  if (distance < SB_MAX_STAR_DIST_LY)
  {
    // get system position in world space
    starpos = system->Get_Pos() * SB_LY_CSUNIT;
    // convert point to camera space
    csVector3 cp1 = c->GetTransform().Other2This(starpos);
    csVector2 screen_spot = c->Perspective(cp1);

    if (screen_spot.x < w && screen_spot.y < h &&
      screen_spot.x > 0 && screen_spot.y > 0 &&
      cp1.z > 0)
    {
       // draw point on screen using 2d cords
       g2d->DrawPixel(static_cast<int>(screen_spot.x),
         static_cast<int>(h - screen_spot.y),
         static_cast<int>(this->Get_Color()));

      //printf("DrawStar2d x:(%2.4f) y:(%2.4f) z:(%2.4f)\n", cp1.x, cp1.y, cp1.z);
      //printf("DrawStar2d x:(%2.4f) y:(%2.4f) z:(%2.4f)\n", starpos.x, starpos.y, starpos.z);

    } // end if star on screen
  } // end <  SB_MAX_STAR_DIST_LY

}

void Star::DrawStar3D(iGraphics3D* g3d, const iCamera* c)
{
  float dist = system->Get_Distance(c); // Light_LY
  float abs_lum = system->Get_Luminosity();
  iGraphics2D* g2d = g3d->GetDriver2D();
  int w = g2d->GetWidth();
  int h = g2d->GetHeight();

  //printf("distance :%4.16f\n", dist);
  dist = (dist/3.2616);
  // Need to take into account the camera pos.
  // m=M+5(log10(d)-1) where m = apr mag M=abs mag d=distance in parsecs.
  float apr_lum = abs_lum + (5 * (log10(dist) - 1));

  //printf("dist: %4.16f ly apr_lum:%4.2f abs_lum:%4.2f", dist*3.2616, apr_lum, abs_lum);
  //printf("log10(%4.2f): %4.2f", dist, log10(dist));

  // Scale gives the size of star in screen px.
  // Viable range is less than 6.5.
  //if (apr_lum > 6.5) return;
  // Scale the star depending on aparent magnitude.
  //float scale = (6.5 - apr_lum);
  float scale = 0;
  if (apr_lum <= 6.5)
  {
    scale = powf(SB_APR_MAG_EXP, (apr_lum > 0.0f ? apr_lum : -1 * apr_lum));
  }
  int offset = static_cast<int>(scale / 2);
  int img_size;
  if (Get_Type() < 8)
  {
    img_size = SB_STAR_TEX_SIZE * 4;
    offset = offset * 4;
    scale = scale * 4;
  }
  else
  {
    img_size = SB_STAR_TEX_SIZE;
  }

  // Get system position in world space.
  csVector3 starpos = system->Get_Pos() * SB_LY_CSUNIT;
  // Convert point to camera space.
  csVector3 cp1 = c->GetTransform().Other2This(starpos);
  csVector2 screen_spot = c->Perspective(cp1);

  if (screen_spot.x < w && screen_spot.y < h &&
    screen_spot.x > 0 && screen_spot.y > 0 &&
    cp1.z > 0)
  {
    g3d->DrawPixmap(
      star_tex->GetTextureHandle (),
      static_cast<int>(screen_spot.x) - offset,
      h - static_cast<int>(screen_spot.y) - offset,
      static_cast<int>(scale), static_cast<int>(scale),
      0, 0,
      img_size, img_size,
      0
      );
  } // end clipping
}

int Star::Get_Type()
{
  std::string tmp_str = classification.substr(0, 1);

  switch (tmp_str[0])
  {
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
