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
#include "const.h"
// --------------------------------------------------------------------------------//
// System member functions --------------------------------------------------------//
// --------------------------------------------------------------------------------//
System::System(int Id, std::string Name , float Ra, float Dec, float Distance)
{

  tmp_rot_value = 0;

  id = 0;
  id = Id;
  right_ascension = Ra;
  declination = Dec;
  distance = Distance;
  name = Name;
  Calculate_Position();
}

System::~System()
{
/*
  // Delete all stars from this system first
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end(); ++itr) 
  {
    delete(*itr);
  } // end for iterate systems
*/
  delete system_star ;
}

// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
bool System::Init(int Id, std::string Name , float Ra, float Dec, float Distance)
{
  id = Id;
  right_ascension = Ra;
  declination = Dec;
  distance = Distance;
  name = Name;
  // Calculate a coordinate position for this system
  Calculate_Position();
  
  return true;
}

bool System::Add_Nebula(std::string tex_nebula_name, float w, float h, iEngine* eng)
{
  tex_nebula = eng->GetTextureList()->FindByName(tex_nebula_name.c_str());
  if (!tex_nebula) return false;
  nebula_w = w;
  nebula_h = h;
  printf("Added nebula %s.\n" , tex_nebula_name.c_str());

  return true;
}

void System::DrawStar3D(iGraphics3D* g3d, const iCamera* c)
{
  system_star->DrawStar3D(g3d, c);
}

void System::DrawStar2D(iGraphics2D* g2d, const iCamera* c)
{
  system_star->DrawStar2D(g2d, c );
}

void System::Set_Position(csVector3 new_pos)
{
  system_pos = new_pos;
}

float System::Get_Luminosity() 
{
  // return the total luminosity of all stars in system
/*  float tot_lum = 0;
  
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end(); ++itr) 
  {
    tot_lum += (*itr)->Get_Luminosity();
  } // end for iterate stars
  return tot_lum;
*/

  return system_star->Get_Luminosity();
}

float System::Get_Distance(const iCamera* c)
{
  csVector3 cs_unit_pos = system_pos * SB_LY_CSUNIT;   // conver the star pos into cs_unit position to match camera space
  csVector3 cam_pos = c->GetTransform().GetT2OTranslation();    // get the cam pos 
  // get the distance in CS_UNITS and conver to LY
  float dist = (sqrt(csSquaredDist::PointPoint(cam_pos, cs_unit_pos))) / SB_LY_CSUNIT; 
  return dist;
}


// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
void System::Calculate_Position()
{
  // Create 3D cords for each system
  system_pos.z = distance * cos(right_ascension*PI/180) * cos(declination*PI/180);
  system_pos.x = distance * sin(right_ascension*PI/180) * cos(declination*PI/180);
  system_pos.y = distance * sin(declination*PI/180);     // where dis = distance to star from Sun.

/*  printf("ra:(%4.2f) dec:(%4.2f) dist:(%4.2f) :: x:(%4.2f) y:(%4.2f) z::(%4.2f) \n \n",
    right_ascension, declination, distance, system_pos.x,system_pos.y,system_pos.z);
  printf("/n");
*/
  return ;
}

float System::Get_Apr_Magnitude()
{
  float dist = distance; // Light_LY
  float abs_lum = system_star->Get_Luminosity();
  dist = (dist/3.2616);
  // does not take into account the camera pos in world space.
  // m=M+5(log10(d)-1) where m = apr mag M=abs mag d=distance in parces's 
  float apr_lum = abs_lum + 5 * (log10(dist)-1);
  return apr_lum;
}
// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
bool System::Add_Star(std::string name, std::string classification,
       float luminosity, int color, iTextureWrapper* txt)
{
/*
  try {

    Star* star;
    star = new Star( name, classification , luminosity, color, txt , this );
    stars.push_back(star);
    return true;
  }
  catch( char * str ) {
    printf ("Exception raised:System::Add_Star %s \n" , str );
    return false;
  }

*/

  system_star = new Star(name, classification, luminosity, color, txt, this);
  
  return true;
}  

// --------------------------------------------------------------------------------//
// --------------------------------------------------------------------------------//
int System::Get_StarType ()
{
  // return the startype of the last star in system
/*  int startype = 0 ;
  
  for (std::vector<Star*>::iterator itr = stars.begin();
    itr != stars.end();
      ++itr) {
      startype += (*itr)->Get_Type();
  } // end for iterate stars
  return startype;  
*/
  return system_star->Get_Type();
};


void System::DrawNebulaTexture(const iCamera* c, iGraphics3D* g3d)
{

  if (!tex_nebula) { return ; }
//  printf("distance: %4.2f  nebula_w: %2.2f nebula_h:%2.2f\n", distance, nebula_w, nebula_h);

//  if  (distance < 100 ) { return ; }  
  iGraphics2D* g2d;
  g2d = g3d->GetDriver2D();


  // Hard coded values , will eventualy come from star data
  float w = nebula_w * SB_SCALE_LY_UNIT;
  float h = nebula_h * SB_SCALE_LY_UNIT;

  std::string mat_name = "nebula1";

  // just a bit of debuging
  tmp_rot_value = tmp_rot_value + 1;
  if (tmp_rot_value > 360) tmp_rot_value = 0;

  csSimpleRenderMesh mesh;
  csVector3 verts[4];
  csVector2 texels[4];
  csVector4 colors[4];

  // Create verts in 3d sapce , create billboard then rotate by RA and Dec 
  csVector3 p1,p2,p3,p4 ;
  csVector3 billboard_pos( -(w/2) , -(h/2) , 0 );
  csVector3 starpos( system_pos.x * SB_SCALE_LY_UNIT , system_pos.y * SB_SCALE_LY_UNIT , system_pos.z * SB_SCALE_LY_UNIT );

  p1 = billboard_pos + csVector3(0, 0, 0);
  p2 = billboard_pos + csVector3(w, 0, 0);
  p3 = billboard_pos + csVector3(w, h, 0);
  p4 = billboard_pos + csVector3(0, h, 0);

  // rotate points so the perpendicular to the stars vector from the orgin 
  float rot_ra , rot_dec;
  csMatrix3 rot_mat;

  // should really use the camers rotation, this only works right at (0,0,0) 
  rot_ra = 180+(right_ascension * (PI / 180.0));
  rot_dec = declination * (PI / 180.0);
  rot_mat = csXRotMatrix3(rot_ra) * csZRotMatrix3(rot_dec);

  // reposition the bilboard to correct spot in world space
  p1 = (rot_mat * p1) + starpos;
  p2 = (rot_mat * p2) + starpos;
  p3 = (rot_mat * p3) + starpos;
  p4 = (rot_mat * p4) + starpos;
  
  // convert the world space points to screen space 
  csVector3 sp1 = Get_SC_Point(c, g3d, p1);
  csVector3 sp2 = Get_SC_Point(c, g3d, p2);
  csVector3 sp3 = Get_SC_Point(c, g3d, p3);
  csVector3 sp4 = Get_SC_Point(c, g3d, p4);


  // setup the simplemesh and draw it
  verts[0].Set(sp1);
  verts[1].Set(sp2);
  verts[2].Set(sp3);
  verts[3].Set(sp4);
  
//  printf("lefttop_edge ( %4.6f / %i ) = %2.16f\n", sp1.x , sc_w , left_edge);
//  printf("leftbot_edge ( %4.6f / %i ) = %2.16f\n", sp4.x , sc_w , left_edge);
//  printf( "sp1( %4.6f,%4.5f,%4.5f) \n", sp1.x , sp1.y , sp1.z );
//  printf( "sp2( %4.6f,%4.5f,%4.5f) \n", sp2.x , sp2.y , sp2.z );
//  printf( "sp3( %4.6f,%4.5f,%4.5f) \n", sp3.x , sp3.y , sp3.z );
//  printf( "sp4( %4.6f,%4.5f,%4.5f) \n", sp4.x , sp4.y , sp4.z );

  texels[0].Set(0.0f, 1.0f);
  texels[1].Set(0.0f, 0.0f);
  texels[2].Set(1.0f, 0.0f);
  texels[3].Set(1.0f, 1.0f);

  colors[0].Set(1, 1, 1, .7);
  colors[1].Set(1, 1, 1, .7);
  colors[2].Set(1, 1, 1, .7);
  colors[3].Set(1, 1, 1, .7);

  mesh.meshtype = CS_MESHTYPE_QUADS;
  mesh.vertexCount = 4;
  mesh.vertices = verts;
  mesh.texcoords = texels;
  mesh.colors = colors;
  mesh.texture =  tex_nebula->GetTextureHandle();
  mesh.mixmode = CS_FX_ALPHA;// CS_FX_ADD ; //CS_FX_COPY ; //
  g3d->DrawSimpleMesh(mesh, csSimpleMeshScreenspace); 

}; 
// Return the screen location(x,y) of star system in camera space.
 
csVector2 System::Get_Sc_Pt(const iCamera* c)
{
  // convert system position to camera space 
  csVector3 pos = system_pos * SB_LY_CSUNIT;
  csVector3 cp1 = c->GetTransform().Other2This(pos); 
  return c->Perspective(cp1);
};

// Return the hight of the image in screen pixels 
int System::Get_SC_Height()
{
  return 0;
};
// Return the width of the image in screen pixels 
int System::Get_SC_Width() 
{
  return 0;
};


// Return the screen location(x,y) of point in world space. 
csVector3 System::Get_SC_Point(const iCamera* c,iGraphics3D* g3d , csVector3 pt)
{
  csVector2 sc_pt;
  int sh = g3d->GetDriver2D()->GetHeight();

  pt = (c->GetTransform().Other2This(pt));

  sc_pt = c->Perspective(pt);
  pt.x = sc_pt.x;
  pt.y = sh - sc_pt.y;
  pt.z = 0;

  return pt;
};

