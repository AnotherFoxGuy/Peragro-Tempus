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
#include "body.h"

// --------------------------------------------------------------------------------//
//  Body member functions     -----------------------------------------------------//
// --------------------------------------------------------------------------------//
Body::Body(iObjectRegistry* reg)
{
  object_reg = reg;
  engine = csQueryRegistry<iEngine> (object_reg );

  if (!engine) printf ("Body::Body() no engine\n");

  last_update_seconds = 1;

  body_verts = 100;
  body_radius = 10;
  ellips.SetRadius(10 );
  ellips.SetCenter (csVector3 (0) );


  name = "defaultbody";

  body_day_length =0.1; // in hours
  body_inclination = 0; // in deg

  Create_Body_Mesh(body_radius, body_verts, body_day_length, body_inclination);

  tmp = 0;

}

Body::~Body ()
{
    for (size_t i = 0; i < child_bodies.GetSize(); i++)
    {
      delete child_bodies[i];
    } // end for iterate for child bodies
    delete this;
}

void Body::Create_Body_Mesh(float radius, int verts, double day, double i)
{
  body_radius = radius;
  body_verts = verts;
  body_day_length = day;
  body_inclination = i ;
  Create_Body_Mesh();
}


void Body::Create_Body_Mesh()
{
  if (!sector)
  {
    if (csbody_report_lvl) printf("Body::Create_Body_Mesh: No sector set\n");
    return ;
  }

  if (mesh)
  {
    //remove old factory and mesh
    engine->RemoveObject(mesh->GetFactory() );
    engine->RemoveObject(mesh);
  }

  ellips.SetRadius(body_radius);
  ellips.SetCenter(csVector3(0));

  // create a default sphere mesh
  CS::Geometry::Sphere* sphere = new CS::Geometry::Sphere(ellips, body_verts);
  // sphere->reversed = true;
  csRef<iMeshFactoryWrapper> fact =
    CS::Geometry::GeneralMeshBuilder::CreateFactory (engine, name.c_str() , sphere );

  mesh = engine->CreateMeshWrapper (fact, name.c_str() , sector, csVector3 (0));
  mesh->SetZBufMode (CS_ZBUF_USE);
  mesh->SetRenderPriority (engine->GetObjectRenderPriority ());

  mesh->SetLightingUpdate( CS_LIGHTINGUPDATE_ALWAYSUPDATE, 8 );

  if (csbody_report_lvl) printf ("Body::Create_Body_Mesh:created body %s\n",name.c_str());
}

void Body::Set_Name (char const* body_name)
{

  if (light)
  {
    light->QueryObject()->SetName(body_name);
    if (csbody_report_lvl) printf("Body::Set_Name:updating light name:%s\n",body_name);
  }
  // update body name
  name = body_name;
}

void Body::Set_Sector ( iSector* sect )
{
  sector=sect;
  // Move the mesh to the sector
  if (mesh) mesh->GetMovable()->SetSector(sector);
}


bool Body::Add_Child (csRef<iMyBody> child )
{
  child_bodies.Insert(0,child);
  return true;
}

bool Body::Set_Parent (csRef<iMyBody> par_body )
{
  // If already has parent need to remove it from that parent's childern array

  parent = par_body;
  return true;
}


bool Body::Draw_FullOrbit (iCamera* c, iGraphics3D* g3d)
{
  if (!mesh || !c || !g3d) return false;

  csVector3 origin(0,0,0);
  if (parent)
  {
    csRef<iMeshWrapper> par_mesh = parent->Get_MeshWrapper();
    if (par_mesh) origin = par_mesh->GetMovable()->GetPosition();

  };

  Draw_Orbit (c, g3d, origin);

  if (mesh) origin = mesh->GetMovable()->GetPosition();

  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)-> Draw_Orbit (c);
  } // end for iterate for child bodies

  return true;
}
bool Body::Draw_FullPosition (iCamera* c, iGraphics3D* g3d,long secondspassed)
{
  csVector3 origin(0,0,0);
  iSceneNode* par_node;
  par_node = mesh->QuerySceneNode ()->GetParent();
  if (parent)
  {
     origin = parent->Get_MeshWrapper()->GetMovable()->GetPosition();
  }

  Draw_Position (c , g3d, origin, secondspassed);

  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)->Draw_Position(c , secondspassed);
  } // end for iterate for child bodies


  return true;
}

void Body::Set_Orbit (
                      long double i,
                      long double n,
                      long double w,
                      long double e,
                      long double a,
                      long double period,
                      double scale )
{

  this->Set_i(i);
  this->Set_n(n);
  this->Set_w(w);
  this->Set_e(e);
  this->Set_a(a);
  Set_period(period);
  Set_scale(scale);

}

void Body::Set_Material(csRef<iMaterialWrapper>& mat)
{
  if (csbody_report_lvl)
    printf("Body::Set_Material: '%s' body material to %s!\n", name.c_str(), mat->QueryObject ()->GetName ()  );

  if (!Apply_Material(mat))
  {
    printf ("Body::Set_Material:Failed to apply material to '%s' \n", name.c_str() );
  }
  mat_name =  mat->QueryObject ()->GetName () ;
}

void Body::Set_Mesh(char const* mesh_name)
{
// TBD

}

void Body::ListChildren (char const* prefix)
{
  std::string out;
  out = prefix;
  out += name;

  printf ("%s\n", out.c_str());

  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)->ListChildren (out.c_str());
  } // end for iterate for child bodies
}

bool Body::Update_Body (long secondspassed )
{
  long  seconds = secondspassed;

  if (seconds > last_update_seconds )
  {
    last_update_seconds = seconds;

    // Position body
    Rotate_Body(Get_Body_Rotation(secondspassed));
    Position_Body(Orbit_Angle(secondspassed), csVector3(0,0,0));

    for (size_t i = 0; i < child_bodies.GetSize(); i++)
    {
      child_bodies.Get(i)->Update_Body (secondspassed , abs_pos.GetOrigin() );
    } // end for iterate for child bodies
    Update_Mesh_Pos ();  // move the meshes to new position
  } // end if check that body has actualy moved

  return true;
}

// used to position child bodies in relation to their parents position.
// origin is the parents pos in world space
bool Body::Update_Body(long secondspassed, csVector3 orbit_origin)
{
  Rotate_Body (Get_Body_Rotation(secondspassed));
  Position_Body (Orbit_Angle(secondspassed), orbit_origin);

  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)->Update_Body ( secondspassed, abs_pos.GetOrigin());
  } // end for iterate for child bodies

  return true;
}

bool Body::Position_Body (float angle, csVector3 orbit_origin)
{
  // Position this body to the passed angle on it's orbit
  csVector3 v3pos;
  float orb_rad;

  orb_rad = angle * (PI / 180.0);
  v3pos = OrbitPointDeg(angle) + orbit_origin;
  abs_pos.SetO2TTranslation(v3pos);

  return true;
}

bool Body::Rotate_Body (float angle)
{
  // Rotate this body to the passed angle
  float rot_rad , axis_rad;
  csMatrix3 rot_mat;

  rot_rad = angle * (PI / 180.0);
  axis_rad = body_inclination * (PI / 180.0);

  rot_mat = csYRotMatrix3(axis_rad) * csZRotMatrix3(rot_rad);

  abs_pos.SetO2T(rot_mat);
  return true;
}

void Body::Update_Mesh_Pos ()
{
  if (!mesh)
  {
    if (csbody_report_lvl) printf ("Body::Update_Mesh_Pos:body '%s' has no mesh to update\n", name.c_str() );
  } else
  {
    iMovable* movable;
    movable = mesh->GetMovable();
    movable->SetTransform (abs_pos);
    csVector3  body_pos = abs_pos.GetOrigin();
    movable->UpdateMove();

  }

  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)->Update_Mesh_Pos ();
  } // end for iterate for child bodies
}


void Body::Pos_Light(const csVector3& npos)
{
  if (!sector) return ;
  if (!light)
  {
    return;
  }else  // update position
  {
    light->SetCenter (npos);
 //   if (csbody_report_lvl) printf("pos %s light( %4.2f, %4.2f, %4.2f)\n", name.c_str(), npos.x, npos.y, npos.z);
  }
}

void Body::Update_Lights()
{
  if (!sector) return ;

  csVector3 pos;

  if (mesh)
  {
    pos = mesh->GetMovable()->GetFullPosition();
//    if (csbody_report_lvl) printf("pos %s light( %4.6f, %4.6f, %4.6f)\n", name.c_str(), pos.x, pos.y, pos.z);
    if (light) Pos_Light (pos);
  }
  // update child bodies
  for (size_t i = 0; i < child_bodies.GetSize(); i++)
  {
    child_bodies.Get(i)->Update_Lights();
  } // end for iterate for child bodies

}

bool Body::Load_Factory (std::string factory )
{
  return true;
}

bool Body::SetMeshsSize ()
{
  return true;
}

csOrthoTransform Body::GetSurfaceOrthoTransform ( const float& lon,const float& lat)
{
  return GetSurfaceTrans( lon, lat);
}

csOrthoTransform Body::GetSurfaceTrans ( const float& lon ,const float& lat )
{

  iMovable* movable;
  csOrthoTransform bodytrans;
  csMatrix3 body_matrix;
  csVector3 body_pos;

  // body pos
  movable = mesh->GetMovable ();
  body_matrix = movable->GetFullTransform ().GetT2O ();
  body_pos = movable->GetFullPosition ();

  csVector3 sur_vec = GetSurfaceVector (lon , lat);

  csVector3 off_set = ( body_radius * sur_vec );
  body_pos += off_set;

  bodytrans = csReversibleTransform ( body_matrix, body_pos );
  bodytrans.LookAt (  GetLookAtVector (lon , lat),  sur_vec  );

  return  bodytrans ;
}

csVector3  Body::GetSurfacePos(const float& lon, const float& lat)
{

  // Get surface vector on sphere at lon/lat
  csVector3 sur_vec = body_radius * GetSurfaceVector(lon, lat);

  return  sur_vec;
}


csVector3  Body::GetSurfaceVector(const float& lon, const float& lat)
{
  // Get surface vector on sphere at lon/lat
  csVector3 sur_vec;

  // Since lon and lat are relative to the sphere need to adjust to
  // absoulte position relative to solarplane
  float adjlon = (lon - body_rotation)*(PI / 180.0);
  float adjlat = (lat + body_inclination)*(PI / 180.0);

  // Get bodys rotaton

  sur_vec.x = cos( adjlon ) * cos( adjlat );
  sur_vec.y = sin( adjlon ) * cos( adjlat );
  sur_vec.z = sin( adjlat );

  sur_vec *= 1.02 ;

  return  sur_vec;
}

csVector3  Body::GetLookAtVector(const float& lon, const float& lat)
{
  // Get surface vector on sphere at lon/lat
  csVector3 sur_vec;

  // Since lon and lat are relative to the sphere need to adjust to
  // absoulte position relative to solarplane
  float adjlon = (lon - body_rotation)*(PI / 180.0);
  float adjlat = (90+lat + body_inclination)*(PI / 180.0);

  // Get bodys rotaton

  sur_vec.x = cos( adjlon ) * cos( adjlat );
  sur_vec.y = sin( adjlon ) * cos( adjlat );
  sur_vec.z = sin( adjlat );

  sur_vec *= 1.02 ;

  return  sur_vec;
}


// --------------------------------------------------------------------------------//
// load a texture file from disk
// note filename is the VFS File name of texture
// --------------------------------------------------------------------------------//
bool Body::Load_Texture(std::string filename, std::string mat_name )
{
  iMaterialList* materiallist;
  iMaterialWrapper* mat;

  csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (object_reg );
  if (!g3d) printf("Body::Load_Texture: Failed to locate 3D renderer!\n");

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg );
  if (!vfs) printf("Body::Load_Texture: Failed to locate VFS plugin!\n");

  csRef<iLoader> loader = csQueryRegistry<iLoader> (object_reg );
  if (!loader) printf("Body::Load_Texture: Failed to locate 3D renderer!\n");

  csRef<iTextureManager> texture_manager = g3d->GetTextureManager();
  if (!texture_manager) printf("Body::Load_Texture: Failed to load Texture Manager!\n");

  materiallist = engine->GetMaterialList();
  mat = materiallist->FindByName(mat_name.c_str() );

  if (!mat ) {
    csRef<iDataBuffer>  buf = vfs->ReadFile  (filename.c_str() , false );
    csRef<iTextureWrapper> texturewapper = loader->LoadTexture (mat_name.c_str() ,
      buf, CS_TEXTURE_3D, texture_manager , true, true, true );
    mat = engine->GetMaterialList ()->FindByName (mat_name.c_str());
  }

  if (!mat ) {
    if (csbody_report_lvl) printf(" Body::Load_Texture: Material %s not found!\n",mat_name.c_str() );
    return false;
  }
  return true;
}


// Give this body a material
bool Body::Apply_Material(csRef<iMaterialWrapper>& mat )
{
  if (!mesh)
  {
    printf("Body::Apply_Material: No '%s' body to apply material to!\n", name.c_str() );
    return false;
  }

  if (!mat )
  {
    printf("Body::Apply_Material: invalid material.\n");
    return false;
  } else
  {
    // Now add the texture to the mesh
    mesh->GetMeshObject ()->SetMaterialWrapper (mat);
  } // end no material

  return true;
}

bool Body::Add_Light(int radius, csColor color)
{
  if (!sector)
  {
    printf("Body::Add_Light: No sector set\n");
    return false;
  }

  // remove light if it already exists
  if (light)
  {
    delete light;
  }

  // Now we need light to see something.
  iLightList* ll = sector->GetLights ();
  csVector3 pos(0,0,0);

  if (mesh) pos = mesh->GetMovable()->GetFullPosition();

  light = engine->CreateLight(name.c_str(), pos, radius , color,
    CS_LIGHT_DYNAMICTYPE_DYNAMIC);
//  light->SetAttenuationMode ( CS_ATTN_INVERSE  );
  light->SetAttenuationMode ( CS_ATTN_NONE  );
  ll->Add (light);
  if (csbody_report_lvl)
  {
    printf (" add light sector has %d lights \n" ,  ll->GetCount() );
    printf (" new light pos ( %4.2f,%4.2f,%4.2f )\n" , pos.x, pos.y, pos.z );
    printf (" Cut Off Dist (%6.2f)\n" , light->GetCutoffDistance () );
    csVector4 lgt_atten_cst = light->GetAttenuationConstants ();
    printf("GetAttenuationConstants (%2.4f,%2.4f,%2.4f,%2.4f) \n", lgt_atten_cst.w , lgt_atten_cst.x ,
      lgt_atten_cst.y , lgt_atten_cst.z );
  }
  return true;
}

float Body::Get_Body_Rotation (long secondspassed )
{
  float rot_angle=0;
  long  seconds = secondspassed;
  if ( last_update_seconds < seconds )
  {
    last_update_seconds = seconds;
    if (body_day_length!=0)  // avoid devide by zero
    {
      long day_in_seconds = static_cast<long>(body_day_length * 60 * 60); // day units is hours
      long day_remainder =  (seconds % day_in_seconds) + 1;
      rot_angle = (360 * ((float) (day_remainder) / (float)(day_in_seconds)));
      body_rotation = rot_angle;
    }
    else
    {
      rot_angle = 0;
      body_rotation = rot_angle;
    }
  }
  else
  {
    rot_angle = body_rotation;
  }
  return rot_angle;
}

csVector3 Body::GetPos()
{
  return mesh->GetMovable()->GetFullPosition();
}

