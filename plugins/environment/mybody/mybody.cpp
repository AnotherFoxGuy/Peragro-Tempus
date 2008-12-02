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
//File: `myobject.cpp'

#include "mybody.h"

MyBody::MyBody(iObjectRegistry *object_reg) : scfImplementationType (this)
{
  initialized = false;
  Initialize(object_reg);
}

MyBody::~MyBody()
{
  delete body;
}
bool MyBody::Initialize(iObjectRegistry *object_reg)
{
  initialized = true;

  body = new Body(object_reg);

  if (!engine) engine = csQueryRegistry<iEngine> (object_reg);
  if (!g3d) g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (!g3d || !engine || !body ) initialized = false;

  return initialized;
}

void MyBody::Set_Body(
                      csVector3 r, long double dl, long double inc, long double i, long double n,
                      long double w, long double e, long double a, long double period, double scale
                      )
{
  body->Set_Orbit(i, n, w, e, a, period, scale );
  body->Set_Radius(r);
  body->Set_Day_Lenght(dl);
  body->Set_Inclination(inc);
}

void MyBody::Set_Orbit(
                       long double i, long double n, long double w, long double e,
                       long double a, long double period, double scale
                       )
{
  body->Set_Orbit(i, n, w, e, a, period, scale );
}

void MyBody::Create_Body_Mesh(float radius, int verts, double day, double i)
{
  body->Create_Body_Mesh( radius, verts, day, i);
}

bool MyBody::Add_Child(csRef<iMyBody> child)
{
  body->Add_Child(child);

  return true;
}

bool MyBody::Set_Parent(csRef<iMyBody> par_body)
{
  // TBD if this already has parent, remove this from parents childern
  body->Set_Parent(par_body);
  par_body->Add_Child(this);
  return true;
}

csOrthoTransform MyBody::GetSurfaceOrthoTransform (const float& lon,const float& lat)
{
  return body->GetSurfaceOrthoTransform (lon, lat);
}

csVector3  MyBody::GetSurfacePos(const float& lon, const float& lat)
{
  return body->GetSurfacePos (lon, lat);
}

csVector3 MyBody::GetSurfaceVector (const float& lon, const float& lat)
{
  return body->GetSurfaceVector (lon, lat);
}

csRef<iMeshWrapper> MyBody::Get_MeshWrapper()
{
  return body->Get_MeshWrapper();
}

void MyBody::Draw_Orbit(iCamera* c)
{
  body->Draw_FullOrbit(c, g3d );
}

void MyBody::Draw_Position(iCamera* c ,long secondspassed)
{
  body->Draw_FullPosition(c, g3d, secondspassed);
}

void MyBody::Update_Body(long secondspassed )
{
  body->Update_Body( secondspassed);
}

void MyBody::Update_Body(long secondspassed, csVector3 origin )
{
  body->Update_Body(secondspassed, origin);
}

double MyBody::GetBodyRotation()
{
  return body->GetBodyRotation();
}

double MyBody::GetOrbitRotation()
{
  return body->GetOrbitRotation();
}

csVector3 MyBody::GetAbsPos ()
{
  return body->GetAbsPos();
}

void MyBody::Set_Material(csRef<iMaterialWrapper>& mat)
{
  body->Set_Material(mat);
}

void MyBody::Update_Lights()
{
  body->Update_Lights();
}
void MyBody::Set_Mesh(char const* mesh_name)
{
  //body->Set_Material(mesh_name);
}
void MyBody::Add_Light(int range,csColor color)
{
  body->Add_Light(range, color);
}

void MyBody::Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up)
{
  body->Draw_Up(c, g3d, up);
}

void MyBody::Update_Mesh_Pos ()
{
  body->Update_Mesh_Pos ();
}

void MyBody::ListChildren (char const* prefix)
{
  body->ListChildren (prefix);
}


csRef<iMyBody> MyBody::Find(const std::string& sname)
{
  csRef<iMyBody> result;
/*
  if (sname == name) result = this;

  std::vector<Body*>::iterator itr = child_bodies.begin();
  while (!result && itr != child_bodies.end() )
  {
    result = (*itr)->Find(sname);
    ++itr;
  } // end for iterate for child bodies
*/
  return result;
}


