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
//  Body member functions 	-----------------------------------------------------//
// --------------------------------------------------------------------------------//
Body::Body(iObjectRegistry* reg) 
{
  object_reg = reg;
  engine = csQueryRegistry<iEngine> (object_reg );

  if (!engine) printf ("Body::Body(iObjectRegistry* reg) no engine\n");
 
  last_update_seconds = -1;

  body_verts = 100;
  body_radius = 10;
  ellips.SetRadius(10 );
  ellips.SetCenter (csVector3 (0) );


  name = "defaultbody";
 
  body_day_lenght =.1; // in hours
  body_inclination = 0; // in deg

  Create_Body_Mesh(body_radius, body_verts, body_day_lenght, body_inclination);

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
  body_day_lenght = day;
  body_inclination = i ;
  Create_Body_Mesh();
}


void Body::Create_Body_Mesh()
{
  if (!sector) 
  {
    printf("Body::Create_Body_Mesh: No sector set\n");
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
  //	sphere->reversed = true;
  csRef<iMeshFactoryWrapper> fact = 
    CS::Geometry::GeneralMeshBuilder::CreateFactory (engine, name.c_str() , sphere );

  mesh = engine->CreateMeshWrapper (fact, name.c_str() , sector, csVector3 (0));
  mesh->SetZBufMode (CS_ZBUF_USE);
  mesh->SetRenderPriority (engine->GetObjectRenderPriority ()); 

  mesh->SetLightingUpdate( CS_LIGHTINGUPDATE_ALWAYSUPDATE, 8 );

  printf ("created body %s\n",name.c_str());
}

void Body::Set_Name (char const* body_name)
{

  if (light)
  {
    light->QueryObject()->SetName(body_name);
    printf("updating light name:%s\n",body_name);
  }
  // update body name
  name = body_name;

  printf("updating body name:%s\n",body_name);
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

  csVector3 origin(0,0,0);
  if (parent)
  {
    //printf("Body::Draw_FullOrbit:Has Parent.\n");
    csRef<iMeshWrapper> par_mesh = parent->Get_MeshWrapper();
    if (par_mesh) origin = par_mesh->GetMovable()->GetPosition();

  } else 
  {
    //printf ("Body::Draw_FullOrbit:No Parent.\n");
  };
  // printf("%s origin (%4.2f:%4.2f:%4.2f) \n", name.c_str() ,origin.x ,origin.y,origin.z);

  //	Draw_Orbit (planetview->GetCamera (), g3d, origin );
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
     //printf("origin (%4.2f:%4.2f:%4.2f) \n",origin.x ,origin.y,origin.z);
     //Draw_Position (planetview->GetCamera () , g3d, origin , secondspassed );
     //printf("Name: %s :" , name.c_str() );
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
  printf("Body::Set_Material: '%s' body material to %s!\n", name.c_str(), mat->QueryObject ()->GetName ()  );

  if (!Apply_Material(mat)) 
  {
    printf ("Body::Set_Material:Failed to apply material to '%s' \n", name.c_str() );
  }
  mat_name =  mat->QueryObject ()->GetName () ;
}

void Body::Set_Mesh(char const* mesh_name)
{


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
    //	printf ( "body %s rot: %4.2f \n" , name.c_str(), Get_Body_Rotation(secondspassed) );
    Rotate_Body(Get_Body_Rotation(secondspassed));
    Position_Body(Orbit_Angle(secondspassed), csVector3(0,0,0));  

    for (size_t i = 0; i < child_bodies.GetSize(); i++) 
    {
      child_bodies.Get(i)->Update_Body (secondspassed , abs_pos.GetOrigin() ); 
    } // end for iterate for child bodies 
    Update_Mesh_Pos ();  // move the meshes to new position
    List_Light();
  } // end if check that body has actualy moved 

  return true;
}

// used to position child bodies in relation to their parents position.
// origin is the parents pos in world space
bool Body::Update_Body(long secondspassed, csVector3 orbit_origin) 
{

  //	printf ("rotation %4.6f\n", rot_angle);
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
  v3pos = OrbitPointDeg(angle);
  abs_pos.SetO2TTranslation(v3pos + orbit_origin);

  return true;
}

bool Body::Rotate_Body (float angle)
{
  // Rotate this body to the passed angle 
  //printf (" %s Body Rotation %f  \n" , name.c_str() , angle ); 

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
    printf ("Body::Update_Mesh_Pos:body '%s' has no mesh to update\n", name.c_str() );
  } else
  {
    //printf("Moving Body Mesh %s\n", name.c_str() );
    iMovable* movable;
    movable = mesh->GetMovable();
    //printf("Moving Body Mesh %s\n", name.c_str() );
    movable->SetTransform (abs_pos);
    csVector3  body_pos = abs_pos.GetOrigin(); // position on surface of sphere 
 //   printf("Update_Mesh_pos::%s (%4.2f,%4.2f,%4.2f)\n ",name.c_str(), body_pos.x,body_pos.y,body_pos.z);
    movable->UpdateMove();
  }

  for (size_t i = 0; i < child_bodies.GetSize(); i++) 
  {
    //   printf("Updating child " );
    child_bodies.Get(i)->Update_Mesh_Pos ();
  } // end for iterate for child bodies 
}


void Body::Pos_Light(const csVector3& npos)
{
//  printf("Body::Pos_Light:%s\n",name.c_str());
  if (!sector) 
  {
 //   printf("Body::Pos_Light: No sector set\n");
    return ;
  }
  if (!light) 
  {
  //  printf("Body::Pos_Light: No light set\n");
    return ;
  } else  // update position 
  {
    //printf("Body::pos_light:%s !\n", name.c_str());
    //light->GetMovable()->SetPosition(npos);
    //light->GetMovable()->UpdateMove();
    light->SetCenter (npos);
  }
}

void Body::Update_Lights()
{
//  printf("\nBody::update_lights:updating body '%s'!\n", name.c_str() );
  if (light)
  {
//    printf("Light Sector: %s \n", light->GetSector ()->QueryObject ()->GetName() );
//    printf("Mesh  Sector: %s \n", mesh->GetMovable () ->QueryObject ()->GetName() );
  }

  if (!sector) 
  {
    printf("Body::Update_Lights: No sector set\n");
    return ;
  }

  csVector3 pos(0,0,0);
 
  if (mesh) pos = mesh->GetMovable()->GetFullPosition();

  if (light)
  {
//    printf("Body::update_lights:updating body '%s' position to (%3.2f , %3.2f , %3.2f) !\n", name.c_str(), pos.x , pos.y, pos.z );
//    light->GetMovable()->SetPosition(pos);
//    light->GetMovable()->UpdateMove();
 //   pos.y = pos.y + (body_radius * 3);
    Pos_Light (pos);
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

csOrthoTransform Body::GetSurfaceOrthoTransform (const float& lon,const float& lat, const csVector3& offset)
{
  csOrthoTransform surfacetrans = GetSurfaceTrans( lon, lat);
  // Tranlate all meshes in this sector by offset 
  iMeshList* ml = sector->GetMeshes ();
  iMeshWrapper* mw;
  iMovable* mov;
  csVector3 body_pos;
  printf("Offset (%4.2f,%4.2f,%4.2f)\n ",offset.x,offset.y,offset.z);

  for (int i = 0; i < ml->GetCount(); i++) 
  {
    mw = ml->Get(i);
    mov = mw->GetMovable();
    body_pos = mov->GetPosition () ;
   // printf("GetSurfaceOrthoTransform::%s (%4.2f,%4.2f,%4.2f)\n ",mw->QueryObject ()->GetName(), body_pos.x,body_pos.y,body_pos.z);
    mov->MovePosition(offset);
    mov->UpdateMove();
    body_pos = mov->GetPosition ();
   // printf("GetSurfaceOrthoTransform::%s (%4.2f,%4.2f,%4.2f)\n\n",mw->QueryObject ()->GetName(), body_pos.x,body_pos.y,body_pos.z);
  }
  
  return surfacetrans;
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

  csVector3 off_set = body_radius * sur_vec;  

  body_pos += off_set;

// Test code to check GetSurfaceTrans position
  csRef<iMeshList> ml = engine->GetMeshes ();
  csRef<iMeshWrapper> cube = ml->FindByName("Moon1");
  if (cube)
  {
//    printf("GetSurfaceTrans: pos (%4.2f,%4.2f,%4.2f)\n", body_pos.x, body_pos.y, body_pos.z);
    cube->GetMovable ()->SetPosition (body_pos );
  }

  bodytrans = csReversibleTransform ( body_matrix, body_pos );
  bodytrans.LookAt ( csVector3(0,0,1) , sur_vec );
//  printf("GetSurfaceVector:(%4.2f,%4.2f,%4.2f)\n", sur_vec.x, sur_vec.y, sur_vec.z);
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
  float radfactor = (PI / 180.0);
  // Get bodys rotaton 

  sur_vec.x = cos( (lon*radfactor)-(body_rotation*radfactor) ) * cos( (lat*radfactor) - (body_inclination*radfactor) );
  sur_vec.y = sin( (lon*radfactor)-(body_rotation*radfactor) ) * cos( (lat*radfactor) - (body_inclination*radfactor) );
  sur_vec.z = sin( (lat*radfactor) - (body_inclination*radfactor) );

 // printf("GetSurfaceVector:(%4.2f,%4.2f,%4.2f)\n", sur_vec.x, sur_vec.y, sur_vec.z);
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

  csRef<iTextureManager> 	texture_manager = g3d->GetTextureManager();
  if (!texture_manager) printf("Body::Load_Texture: Failed to load Texture Manager!\n");

  materiallist	= engine->GetMaterialList();
  mat = materiallist->FindByName(mat_name.c_str() );

  if (!mat ) {	
    csRef<iDataBuffer>  buf = vfs->ReadFile  (filename.c_str() , false ); 
    csRef<iTextureWrapper> texturewapper = loader->LoadTexture (mat_name.c_str() ,
      buf, CS_TEXTURE_3D, texture_manager , true, true, true );
    mat = engine->GetMaterialList ()->FindByName (mat_name.c_str());
  }

  if (!mat ) { 
    printf(" Body::Load_Texture: Material %s not found!\n",mat_name.c_str() );
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
  }

  if (!mat ) 
  {
    printf("Body::Apply_Material: invalid material.\n");
  } else 
  {
    // Now add the texture to the mesh
    mesh->GetMeshObject ()->SetMaterialWrapper (mat);
    mesh->GetMeshObject ()->InvalidateMaterialHandles ();	
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
    printf("Deleting existing light\n");
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

  printf (" add light sector has %d lights \n" ,  ll->GetCount() );
  printf (" new light pos ( %4.2f,%4.2f,%4.2f )\n" , pos.x, pos.y, pos.z );
  printf (" Cut Off Dist (%6.2f)\n" , light->GetCutoffDistance () );
  csVector4 lgt_atten_cst = light->GetAttenuationConstants ();
  printf("GetAttenuationConstants (%2.4f,%2.4f,%2.4f,%2.4f) \n", lgt_atten_cst.w , lgt_atten_cst.x , lgt_atten_cst.y , lgt_atten_cst.z ); 


  return true;
}

void Body::List_Light() 
{
  if (!sector) 
  {
    printf("Body::List_Light: No sector set\n");
    return ;
  }

  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  //	printf (" sector:%s has %d lights \n" , sector_name.c_str() , ll->GetCount() );
  for (int i = 0; i < ll->GetCount(); i++) 
  {
    light = ll->Get(i);
    csVector3 pos = light->GetMovable()->GetFullPosition();
    light->GetMovable()->MovePosition (csVector3(0,0,1));
 //   light->Setup();
    //		printf (" light %s: pos ( %4.2f,%4.2f,%4.2f )\n", light->QueryObject()->GetName() , pos.x, pos.y, pos.z );
  }
//  sector->ShineLights();
}

csVector3 Body::RotateZ (const csVector3 body_pos,const float body_rotation)
{

  csQuaternion rot;
  rot.SetAxisAngle(csVector3(0,1,0) , body_rotation*(PI / 180.0) );
  return rot.Rotate (body_pos);   
}

float Body::Get_Body_Rotation (long secondspassed )
{

  float rot_angle=0;
  long  seconds = secondspassed;	
  //	printf("seconds %lu\n" ,seconds);
  //	printf("last_update_seconds %lu\n", last_update_seconds);
  if ( last_update_seconds < seconds )
  {
    last_update_seconds = seconds;
    //		printf("last_update_seconds %lu\n", last_update_seconds);
    //		printf("body_day_lenght %4.2f\n", body_day_lenght);
    if (body_day_lenght!=0)
    {
      long day_in_seconds = (body_day_lenght * 60 * 60); // day units is hours 
      //			printf (" day_in_seconds:%lu ", day_in_seconds);  
      long day_remainder =  (seconds % day_in_seconds ) + 1;
      //			printf (" day_remainder%lu \n", day_remainder);
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

  //	printf ("body %s rotation %4.2f \n",name.c_str(),rot_angle);
  return rot_angle;
}
