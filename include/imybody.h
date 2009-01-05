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
// File: `imybody.h'


#ifndef _H_IMYBODY
#define _H_IMYBODY

#include <string>

#include <cssysdef.h>
#include <csutil/scf.h>
#include <iengine/sector.h>

class csVector3;
struct iMeshWrapper;
struct iSceneNode;
struct iMaterialWrapper;
class csTransform;
class csColor;
struct iGraphics3D;
struct iCamera;

// Interface header
struct iMyBody : public virtual iBase
{
  SCF_INTERFACE(iMyBody,1,0,1);

  virtual void Set_Body (
    csVector3 r,
    long double dl,
    long double inc,
    long double i,
    long double n,
    long double w,
    long double e,
    long double a,
    long double period,
    double scale
    ) = 0 ;

  virtual void Set_Orbit (
    long double i,
    long double n,
    long double w,
    long double e,
    long double a,
    long double period,
    double scale                  // need to define this , scale of AU to csUnits?
    ) = 0 ;

  virtual void Set_Name(char const* name) = 0;
/*
  virtual void Set_a(long double val) = 0;
  virtual void Set_i(long double val) = 0;
  virtual void Set_n(long double val) = 0;
  virtual void Set_w(long double val) = 0;
  virtual void Set_e(long double val) = 0;
  virtual void Set_period (long double val) = 0;
*/
  virtual void Set_scale (long double val) = 0;
  virtual void Set_sector(iSector* sect) = 0;
  virtual void Set_Material(csRef<iMaterialWrapper>& mat) = 0;

  virtual void Set_Mesh(char const* mesh_name) = 0;

  virtual void Create_Body_Mesh() = 0;
  virtual void Create_Body_Mesh(float radius, int verts, double day, double i) = 0;

  virtual char const* Get_Name() const = 0;
  virtual long double const GetBodyInc() = 0;

  virtual csRef<iMeshWrapper> Get_MeshWrapper() = 0;
  virtual csVector3 GetPos() = 0;


  virtual bool Add_Child(csRef<iMyBody> child) = 0;
  virtual bool Set_Parent(csRef<iMyBody> par_body) = 0;

  virtual void Update_Body(long secondspassed ) = 0;
  virtual void Update_Body(long secondspassed, csVector3 origin) = 0;
  virtual void Update_Mesh_Pos() = 0;

  virtual void Add_Light(int range, csColor color) = 0;
  virtual void Update_Lights() = 0;

  virtual void Draw_Orbit(iCamera* c) = 0;
  virtual void Draw_Position(iCamera* c, long secondspassed ) = 0;
  virtual void Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up) = 0;

  virtual csOrthoTransform GetSurfaceOrthoTransform (const float& lon,const float& lat) = 0;
  virtual csVector3  GetSurfacePos(const float& lon, const float& lat) = 0;
  virtual csVector3 GetSurfaceVector(const float& lon, const float& lat) = 0;
  virtual double GetBodyRotation() = 0;
  virtual double GetOrbitRotation() = 0;

  virtual void ListChildren (char const* prefix) = 0;
  virtual csRef<iMyBody> Find(const std::string& sname) = 0;

};


// Factory Header
struct iMyBodyFactory : public virtual iBase
{
  SCF_INTERFACE(iMyBodyFactory,1,0,0);

  virtual csPtr<iMyBody> CreateObject () = 0;

};

#endif
// _H_IMYBODY___
