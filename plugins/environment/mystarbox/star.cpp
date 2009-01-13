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
    iTextureWrapper* tex ,
    System* parent_system)
{
  name = star_name;
  classification = new_classification;
  luminosity = new_luminosity;

  mass = pow(pow(luminosity, 1.0f/3), 2);
  starcolor = color;
  system = parent_system;
  star_tex = tex ;
  last_tex = rand()% STAR_ANAM_FRAMES;
}

Star::~Star()
{
  star_textures.DeleteAll();
}

void Star::Set_Texture(iTextureWrapper* tex)
{
  star_textures.Push(tex);
  star_tex = tex;
}


void Star::DrawStar2D( iGraphics2D* g2d, const iCamera* c)
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
    screen_spot.y = Adjust_Perspective_Height(static_cast<int>(screen_spot.y), g2d);

    if (
      screen_spot.x < w && screen_spot.y < h &&
      screen_spot.x > 0 && screen_spot.y > 0 &&
      cp1.z > 0)
    {
      // draw point on screen using 2d cords
      g2d->DrawPixel
      (
        static_cast<int>(screen_spot.x),
        static_cast<int>(h - screen_spot.y),
        static_cast<int>(this->Get_Color())
       );

    } // end if star on screen
  }; // end <  SB_MAX_STAR_DIST_LY

}


void Star::DrawStar3D(iGraphics3D* g3d, const iCamera* c,const float& base_star_size,const float& apr_mag_exp )
{

  float dist = system->Get_Distance(c); // Light_LY
  float abs_lum = system->Get_Luminosity();
  iGraphics2D* g2d = g3d->GetDriver2D();
  int w = g2d->GetWidth();
  int h = g2d->GetHeight();

  dist = (dist/3.2616);
  // need to take into account the camera pos
  // m=M+5(log10(d)-1) where m = apr mag M=abs mag d=distance in parces's
  
  // log10(d) approximation
  int p = 0;
  int dist_t = (int)(dist*10);
  while(dist_t>100) {
    dist_t=dist_t/10;
    p++;
  }
  // p+logtable[dist_t]-1 = log10(d)
  float apr_lum = abs_lum + 5 * (p+logtable[dist_t]-2);

  // scale gives the size of star in screen px
  // visable range is less than 6.5
  //if (apr_lum > 6.5 ) return;
  // scale the star debending on aparent magnatude
  // float scale = (6.5 - apr_lum);
  float scale = 0;
/*
      printf("star info : %i %3.10f %3.10f %4.10f %3.10f %s\n",
              system->Get_Id(),
              system->Get_Ra(),
              system->Get_Dec(),
              system->Get_Distance(),
              system->Get_Luminosity(),
              system->Get_Classification().c_str() );
*/


  if (apr_lum <= 6.5)
  {
    scale = powf(apr_mag_exp, (apr_lum>0.0f?apr_lum:-1*apr_lum));
  }

  int offset = int(scale/2);

//  if (Get_Type() > 1)
//  {
    offset = static_cast<int>(offset * base_star_size);
    scale = scale * base_star_size;
//  } else
//  {
//    img_size = SB_STAR_TEX_SIZE * HALO_SCALE_FACTOR ;
//    offset = offset * HALO_SCALE_FACTOR ;
//    scale = scale * HALO_SCALE_FACTOR ;
//  }


  // get system position in world space
  csVector3 starpos = system->Get_Pos() * SB_LY_CSUNIT;
  // convert point to camera space
  csVector3 cp1 = c->GetTransform().Other2This(starpos);
  csVector2 screen_spot = c->Perspective(cp1);
  screen_spot.y = Adjust_Perspective_Height(static_cast<int>(screen_spot.y), g2d);

  if (
    screen_spot.x < w && screen_spot.y < h &&
    screen_spot.x > 0 && screen_spot.y > 0 &&
    cp1.z > 0
  ){

    // virtual void iGraphics3D::DrawPixmap ( iTextureHandle *  hTex,sx,sy,sw,sh,tx,ty,tw,th,Alpha )
    g3d->DrawPixmap
    (
      star_tex->GetTextureHandle(),
      static_cast<int>(screen_spot.x) - offset, h - static_cast<int>(screen_spot.y) - offset,
      static_cast<int>(scale), static_cast<int>(scale),
      0, 0,
      SB_STAR_TEX_SIZE, SB_STAR_TEX_SIZE,
      0
    );

    last_tex += 0.1;
    size_t cur_tex = static_cast<size_t>(last_tex);
    if (cur_tex > star_textures.GetSize()-1ul)
    {
      cur_tex = 0ul;
      last_tex = 0.0f;
    }

    star_tex = star_textures.Get(cur_tex);

  }; // end cliping
}

int Star::Get_Type()
{

  std::string tmp_str = classification.substr(0,1);

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



int Star::Adjust_Perspective_Height(int y, iGraphics2D* g2d)
{
  float w = static_cast<float>(g2d->GetWidth());
  float h = static_cast<float>(g2d->GetHeight());
  int c = static_cast<int>(h/2.0f);
  float adj;
  adj = (w/h);

  int result = static_cast<int>(((y-c) * adj)+c);

  return result;
}
