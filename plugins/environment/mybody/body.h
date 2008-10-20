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

// CS Files
#include <cssysdef.h>
#include <crystalspace.h>

#include <string>
#include <cstdlib>
#include <cmath>
#include <ctime>
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
  csRef<iView> planetview;

  std::string name;
  std::string sector_name;
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
  csTransform abs_pos;  // this bodies transform with sun as origin

  double scale; 

  long  last_update_seconds; //

  csRef<iMyBody> parent;  // Parent body, Null if none
  csRefArray<iMyBody> child_bodies;


public:


  Body(iObjectRegistry* reg);
  ~Body();

  void Set_Name(char const* body_name);
  void Set_Sector(char const* sector_name);  
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

  void Create_Body_Mesh(float radius, int verts, double day, double i);
  void Set_Mesh(char const* mesh_name);
  void Set_Materal(char const* mat_name); 

  bool Set_Parent (csRef<iMyBody> par_body);
  bool Add_Child (csRef<iMyBody> child);

  void Set_Parent_Node (iSceneNode* parent) { mesh->QuerySceneNode()->SetParent(parent); };	
  void Set_Camera_Parent(iSceneNode* parent) { /*@todo implement.*/ }			

  iSceneNode* Get_SceneNode () { return mesh->QuerySceneNode (); };

  char const* Get_Name() const { return name.c_str(); };
  csVector3 const Get_Radius() const { return ellips.GetRadius(); };
  double const Get_Day_Lenght() const { return body_day_lenght; };
  double const Get_Inclination() const { return body_inclination; };


  csRef<iMeshWrapper> Get_MeshWrapper() { return mesh; };
  csRef<iCamera> Get_Surface_Camera(const iCamera* c,float lon , float lat);


  bool Update_Body(long secondspassed);
  bool Update_Body(long secondspassed, csVector3 orbit_origin); // used for child bodies 
  void Update_Meshs( const csTransform& trans, const double& body_rot, char const* sel_body);

  bool Add_Light(int radius, csColor color);
  void Update_Lights();

  bool Draw_FullOrbit (iCamera* c, iGraphics3D* g3d);
  bool Draw_FullPosition (iCamera* c, iGraphics3D* g3d, long secondspassed);

  csOrthoTransform GetSurfaceTrans (csOrthoTransform cameratrans ,float lon ,float lat);

  double GetBodyRotation() {return body_rotation;}
  csTransform Get_Surface_Pos(float lon, float lat);
  csVector3 GetSurfaceVector(float lon, float lat);  // rename to GetSphereUpVector
  csVector3 GetMeshUpVector(csTransform trans);

private:
  bool Rotate_Body(float angle);
  bool Position_Body(float angle, csVector3 orbit_origin);

  // Child body methods 
  // bool Remove_Child (csRef<iMyBody> child);

  // General help methods
  bool Load_Factory (std::string factory);
  bool SetMeshsSize ();
  bool Apply_Material(std::string mat_name);
  bool Load_Texture(std::string filename, std::string mat_name);

  csVector3  Get_Surface_Position(float lon, float lat);
  csVector3 RotateZ (const csVector3 body_pos,const float body_rotation);
  float Get_Body_Rotation (long secondspassed );
  void List_Light();
  void Pos_Light(const csVector3& npos);

};

#endif 
// _H_BODY_
