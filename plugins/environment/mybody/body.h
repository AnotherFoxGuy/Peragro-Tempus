/*
    Copyright (C) 2008 by Mogue Carpenter

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
/*
* body.h
*
* Base Class for managing bodies    
* 
*/

#ifndef _H_BODY_
#define _H_BODY_

#define report_lvl false

// CS Files
#include <cssysdef.h>
#include <crystalspace.h>

#include <string>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <cstool/primitives.h>

#include "imybody.h"
#include "orbit.h" 

// --------------------------------------------------------------------------------//
// 
// --------------------------------------------------------------------------------//
class Body: public Orbit
{

private:
  iObjectRegistry* object_reg; 
  csRef<iEngine> engine;
  csRef<iSector> sector;
  csRef<iLight> light;

  std::string name;
//  std::string sector_name;


  std::string filename;
  std::string mat_name;

  csEllipsoid ellips;
  float body_radius;
  int body_verts;
  double body_day_lenght; // in hours
  double body_inclination; // in deg

  double body_rotation ; // in deg at last update

  csRef<iMeshWrapper> mesh;  // A mesh for this body 
  //		csRef<iLight> light;  declaring this causes a memory fault for some reason  
  csTransform abs_pos;  // this bodies transform with (0,0,0) as origin

  double scale; 

  long  last_update_seconds; //

  csRef<iMyBody> parent;  // Parent body, Null if none
  csRefArray<iMyBody> child_bodies;


public:


  Body(iObjectRegistry* reg);
  ~Body();

  void Set_Name(char const* body_name);
  void Set_Sector(iSector* sect );  
  void Set_Radius(csVector3 r) { ellips.SetRadius (r); body_radius = r.x; };  
  void Set_Day_Lenght(long double dl) { body_day_lenght = dl; };  
  void Set_Inclination(long double i) { body_inclination = i; };  


  void Set_Orbit (
    long double i,
    long double n,
    long double w,
    long double e,
    long double a,
    long double period,
    double scale
    );

  void Create_Body_Mesh();
  void Create_Body_Mesh(float radius, int verts, double day, double i);
  void Set_Mesh(char const* mesh_name);
  void Set_Material(csRef<iMaterialWrapper>& mat); 

  bool Set_Parent (csRef<iMyBody> par_body);
  bool Add_Child (csRef<iMyBody> child);

  iSceneNode* Get_SceneNode () { return mesh->QuerySceneNode (); };

  char const* Get_Name() const { return name.c_str(); };
  csOrthoTransform GetSurfaceOrthoTransform (const float& lon,const float& lat); 
  csVector3 GetSurfacePos(const float& lon, const float& lat);
  csVector3 GetSurfaceVector (const float& lon, const float& lat);

  csVector3 const Get_Radius() const { return ellips.GetRadius(); };
  double const Get_Day_Lenght() const { return body_day_lenght; };
  double const Get_Inclination() const { return body_inclination; };
  csVector3 GetAbsPos () const { return abs_pos.GetOrigin(); };


  csRef<iMeshWrapper> Get_MeshWrapper() { return mesh; };
 
  bool Update_Body(long secondspassed);
  bool Update_Body(long secondspassed, csVector3 orbit_origin); // used for child bodies 
  void Update_Mesh_Pos(); // positions meshes with (0,0,0) as origin

  bool Add_Light(int radius, csColor color);
  void Update_Lights();

  bool Draw_FullOrbit (iCamera* c, iGraphics3D* g3d);
  bool Draw_FullPosition (iCamera* c, iGraphics3D* g3d, long secondspassed);

  double GetBodyRotation() {return body_rotation;}
  double GetOrbitRotation() {return this->Orbit_Angle(last_update_seconds);}

  void ListChildren (char const* prefix);


private:
  bool Rotate_Body(float angle);
  bool Position_Body(float angle, csVector3 orbit_origin);

  // Surface position methods 
  csOrthoTransform GetSurfaceTrans (const float& lon ,const float& lat );

  // Child body methods 
  // bool Remove_Child (csRef<iMyBody> child);

  // General help methods
  bool Load_Factory (std::string factory);
  bool SetMeshsSize ();
  bool Apply_Material(csRef<iMaterialWrapper>& mat);
  bool Load_Texture(std::string filename, std::string mat_name);

  float Get_Body_Rotation (long secondspassed );
  void Pos_Light(const csVector3& npos);

};

#endif 
// _H_BODY_
