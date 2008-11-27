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
//File: `mybody.h'

#ifndef _H_MYBODY
#define _H_MYBODY

#define report_lvl false

#include <cssysdef.h>
#include <crystalspace.h>
#include <csutil/scf_implementation.h>

/*#include "crystalspace.h"*/
#include "imybody.h"
#include "body.h"

// Implementation header
class MyBody : public scfImplementation1<MyBody, iMyBody>
{
private:
  iObjectRegistry* object_reg;
  csRef<iEngine> engine;
  csRef<iGraphics3D> g3d;

  bool initialized ;
  //	csStringHash xmltokens;
  //	csRef<iStandardReporterListener> stdrep;

  Body* body;

public:
  MyBody(iObjectRegistry *object_reg);
  virtual ~MyBody();
  bool Initialize (iObjectRegistry*);

  virtual void Set_Body(
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
    );
  virtual void Set_Orbit (
    long double i,
    long double n,
    long double w,
    long double e,
    long double a,
    long double period,
    double scale
    );

  virtual void Set_Name(char const* name)  {body->Set_Name(name);};
  virtual void Set_a(long double val) {body->Set_a(val); };
  virtual void Set_i(long double val) {body->Set_i(val); };
  virtual void Set_n(long double val) {body->Set_n(val); };
  virtual void Set_w(long double val) {body->Set_w(val); };
  virtual void Set_e(long double val) {body->Set_e(val); };
  virtual void Set_period(long double val) {body->Set_period(val); };
  virtual void Set_scale(long double val) {body->Set_scale(val); };
  virtual void Set_sector(iSector* sect) { body->Set_Sector(sect); };

  virtual void Create_Body_Mesh() { body->Create_Body_Mesh(); };
  virtual void Create_Body_Mesh(float radius, int verts, double day, double i);
  virtual void Set_Mesh(char const* mesh_name);
  virtual void Set_Material(csRef<iMaterialWrapper>& mat);
  //	virtual bool Set_mesh (iMeshWrapper* );

  virtual char const* Get_Name() const {return body->Get_Name(); };
  virtual csOrthoTransform GetSurfaceOrthoTransform (const float& lon,const float& lat);
  virtual csVector3 GetSurfacePos(const float& lon, const float& lat);
  virtual csVector3 GetSurfaceVector(const float& lon, const float& lat);
  virtual csRef<iMeshWrapper> Get_MeshWrapper();

  virtual bool Set_Parent(csRef<iMyBody> par_body );
  virtual bool Add_Child(csRef<iMyBody> child );

  virtual void Update_Body(long secondspassed );
  virtual void Update_Body(long secondspassed, csVector3 origin );
  virtual void Update_Mesh_Pos();

  virtual void Add_Light(int range,csColor color);
  virtual void Update_Lights();

  virtual void Draw_Orbit( iCamera* c );
  virtual void Draw_Position( iCamera* c, long secondspassed );
  virtual void Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up);

  // extras!!!
  virtual iSceneNode* Get_SceneNode() { return body->Get_SceneNode(); };

  virtual double GetBodyRotation();
  virtual double GetOrbitRotation();
  virtual csVector3 GetAbsPos ();

  virtual void ListChildren (char const* prefix);
  virtual csRef<iMyBody> Find(const std::string& sname);

};

#endif // _H_MYBODY__

