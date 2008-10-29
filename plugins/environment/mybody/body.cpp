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

  sector_name = "sector";
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

  if (mesh) 
  {
    //remove old factory and mesh
    engine->RemoveObject(mesh->GetFactory() );
    engine->RemoveObject(mesh);
  }
  body_radius = radius;
  body_verts = verts;
  body_day_lenght = day;
  body_inclination = i ;

  ellips.SetRadius(body_radius);
  ellips.SetCenter(csVector3(0));

  // create a default sphere mesh
  CS::Geometry::Sphere* sphere = new CS::Geometry::Sphere(ellips, body_verts); 
  //	sphere->reversed = true;

  csRef<iMeshFactoryWrapper> fact = 
    CS::Geometry::GeneralMeshBuilder::CreateFactory (engine, name.c_str() , sphere );
  iSector* sector = engine->GetSectors ()->FindByName (sector_name.c_str() );
  if (!sector) sector = engine->CreateSector (sector_name.c_str() );

  mesh = engine->CreateMeshWrapper (fact, name.c_str() , sector, csVector3 (0));
  mesh->SetZBufMode (CS_ZBUF_USE);
  mesh->SetRenderPriority (engine->GetObjectRenderPriority ()); 

  // give the mesh a meteral
  filename = "/appdata/defaultmap/textures/yellow.jpg";
  mat_name = "yellow";
  Load_Texture(filename, mat_name);

  iMaterialList* materiallist;
  csRef<iMaterialWrapper> mat;
  materiallist = engine->GetMaterialList();
  mat = materiallist->FindByName(mat_name.c_str() );
  Apply_Material (mat);

  printf ("created body %s\n",name.c_str());
}

void Body::Set_Name (char const* body_name)
{
  // update body light if one exists
  iSector* sector;
  iLight* light;

  sector = engine->FindSector(sector_name.c_str() , 0 );
  if (!sector) printf("Body::Add_Light: Failed to locate sector !");
  //	printf ("Pos_Ligh\nSector:%s\n" , sector_name.c_str() ); 

  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  light = ll->FindByName(name.c_str());
  if (light)
  {
    light->QueryObject()->SetName(body_name);
    //		printf("updating light name\n");
  }
  // update body name
  name = body_name;
}

void Body::Set_Sector (char const* name)
{
  sector_name = name;
  // Move the mesh to the sector 
  iSector* sector = engine->GetSectors ()->FindByName (sector_name.c_str() );
  if (!sector) sector = engine->CreateSector (sector_name.c_str() );
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



csOrthoTransform Body::GetSurfaceTrans (csOrthoTransform cameratrans ,float lon ,float lat )
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


  bodytrans = csReversibleTransform (body_matrix ,  body_pos );

  return  (cameratrans * bodytrans );
}


bool Body::Draw_FullOrbit (iCamera* c, iGraphics3D* g3d)
{
  csVector3 origin(0,0,0);
  if (parent)
  {
    origin = parent->Get_MeshWrapper()->GetMovable()->GetPosition();
   // printf("Has Parent.");
  } else 
  {
   // printf ("No Parent.");
  };
  // printf("%s origin (%4.2f:%4.2f:%4.2f) \n", name.c_str() ,origin.x ,origin.y,origin.z);

  //	Draw_Orbit (planetview->GetCamera (), g3d, origin );
  Draw_Orbit (c, g3d, origin);

  origin = this->Get_MeshWrapper()->GetMovable()->GetPosition();
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
  if (!Apply_Material(mat)) 
  {
    printf ("Body '%s' unable to apply material \n", name.c_str() );
  }
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
    Rotate_Body(Rotate_Body(Get_Body_Rotation(secondspassed)));
    Position_Body(Orbit_Angle(secondspassed), csVector3(0,0,0));  

    for (size_t i = 0; i < child_bodies.GetSize(); i++) 
    {
      child_bodies.Get(i)->Update_Body (secondspassed , abs_pos.GetOrigin() ); 
    } // end for iterate for child bodies 
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

csVector3 Body::GetMeshUpVector (csTransform trans)
{
  // trans is the transformation to the surface position abs space to world space 
  // Get surface vector on sphere at lon/lat 
  csVector3 sur_vec;

  // Get bodys rotaton 
  sur_vec = (abs_pos.GetOrigin() - trans.GetOrigin());

  return  sur_vec; 
}
csVector3 Body::Get_Surface_Position (float lon, float lat)
{
  // Get point on a sphere's surface with origin at (0,0,0) 
  csVector3 sur_vec =  GetSurfaceVector(lon, lat); // add raduis and surface position
  csVector3 sur_pos = body_radius * sur_vec; 
  return sur_pos;
}

csVector3 Body::GetSurfaceVector (float lon ,float lat)
{
  // Get surface vector on sphere at lon/lat 
  csVector3 sur_vec;

  // Get bodys rotaton 

  sur_vec.x = cos(lon)*cos(lat);
  sur_vec.y = sin(lon)*cos(lat);
  sur_vec.z = sin(lat);

  return  sur_vec; 
}


void Body::Update_Meshs( const csTransform& trans, const double& body_rot, char const* sel_body )
{
  // trans = translation to surface pos from world space
  // new up direction at surface pos 
  csMatrix3 body_matrix;
  csVector3 body_pos;

  iMovable* movable;
  movable = mesh->GetMovable();


  body_pos = (abs_pos.GetOrigin() - trans.GetOrigin()); // position on surface of sphere 
  body_matrix = abs_pos.GetO2T();  
  //	body_matrix = trans.GetO2T();
  csMatrix3 body_rot_none ( 1,0,0, 0,1,0, 0,0,1 ); 

  std::string tmp = sel_body;

  csOrthoTransform bodytrans;
  if ( name == tmp ) {
    bodytrans.SetT2O(body_rot_none);
    //bodytrans.Identity();
    bodytrans.Translate(csVector3(0,-body_radius,0));
    Pos_Light(csVector3(0,-body_radius,0));

  } else {
    bodytrans.SetT2O(body_matrix);
    // rotate the bodies pos by the rotation if the
    csVector3 npos = RotateZ (body_pos, body_rot);
    bodytrans.Translate(npos);
    Pos_Light(npos);
  } 

  movable->SetTransform (bodytrans);

  movable->UpdateMove();

  //	printf ("%s body_pos (x,y,z): (%3.4f,%3.4f,%3.4f)\n", name.c_str(),  body_pos.x,  body_pos.y,  body_pos.z);
  //	printf ( "mat1( %3.4f,%3.4f,%3.4f)\n", bodytrans.GetT2O().m11, bodytrans.GetT2O().m12, bodytrans.GetT2O().m13 );
  //	printf ( "mat2( %3.4f,%3.4f,%3.4f)\n", bodytrans.GetT2O().m21, bodytrans.GetT2O().m22, bodytrans.GetT2O().m23 );
  //	printf ( "mat3( %3.4f,%3.4f,%3.4f)\n", bodytrans.GetT2O().m31, bodytrans.GetT2O().m32, bodytrans.GetT2O().m33 );

  for (size_t i = 0; i < child_bodies.GetSize(); i++) 
  {
    child_bodies.Get(i)->Update_Meshs (trans , body_rot,  sel_body);
  } // end for iterate for child bodies 

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

  rot_mat = csXRotMatrix3(axis_rad) * csZRotMatrix3(rot_rad);

  abs_pos.SetO2T(rot_mat);


  return true;
}

void Body::Update_Mesh_Pos ()
{

  iMovable* movable;
  movable = mesh->GetMovable();

//  printf("Moving Body Mesh %s\n", name.c_str() );
  movable->SetTransform (abs_pos);
  movable->UpdateMove();

  for (size_t i = 0; i < child_bodies.GetSize(); i++) 
  {
 //   printf("Updateing child " );
    child_bodies.Get(i)->Update_Mesh_Pos ();
  } // end for iterate for child bodies 

}



csTransform Body::Get_Surface_Pos (float lon , float lat)
{
  // the W2O transform for a position on this sphere at lon,lat 

  // get lon/lat rotaton matrix
  float rot_rad , axis_rad;
  csMatrix3 rot_mat;

  // Get rotation matrix for lon/lat pos
  rot_rad = lon * (PI / 180.0);
  axis_rad = lat * (PI / 180.0);

  //Get surface vector on sphere of R and lon and lat
  csVector3 sur_pos = Get_Surface_Position(lon, lat);	


  // set rotation
  float rot_angle = body_rotation ;
  rot_mat = csXRotMatrix3(axis_rad ) * csZRotMatrix3(rot_rad + (rot_angle*(PI /180.0)));

  csReversibleTransform sur_rtrans(rot_mat, sur_pos );

  // create the translation with bodys rotation and suface pos // 
  csTransform sur_trans (sur_rtrans.GetT2O() , sur_rtrans.GetT2OTranslation() ); 

  // get bodys pos
  csVector3 mesh_pos = abs_pos.GetOrigin();


  // move origin 
  sur_trans.Translate( mesh_pos ); 
  sur_trans.SetO2T(rot_mat);
  return sur_trans;
}

void Body::Pos_Light(const csVector3& npos)
{
  iSector* sector;
  iLight* light;


  sector = engine->FindSector(sector_name.c_str() , 0 );
  if (!sector) printf("Body::Add_Light: Failed to locate sector !");
  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  light = ll->FindByName(name.c_str());
  if (light)
  {
    //printf("Body::pos_light:%s !\n", name.c_str());
    light->GetMovable()->SetPosition(npos);
    light->GetMovable()->UpdateMove();
    light->SetCenter (npos);
 //   light->Setup();
  }
}

void Body::Update_Lights()
{
  iSector* sector;
  iLight* light;

  sector = engine->FindSector(sector_name.c_str() , 0 );
  if (!sector) printf("Body::Add_Light: Failed to locate sector !");
  //	printf ("Sector:%s\n" , sector_name.c_str() ); 

  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  csVector3 pos =  mesh->GetMovable()->GetFullPosition();

  light = ll->FindByName(name.c_str());
//light->Setup();
  if (light)
  {
    //printf("Body::update_lights:%s !\n", name.c_str());
    light->GetMovable()->SetPosition(pos);
    light->GetMovable()->UpdateMove();
 //   light->Setup();
  }
 
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

  if (!mat ) 
  {
    printf("Body::Apply_Material: invalide material.\n");
  } else {
    // Now add the texture to the mesh
    mesh->GetMeshObject ()->SetMaterialWrapper (mat);
  } // end no material 
  mesh->GetMeshObject ()->InvalidateMaterialHandles ();	

  printf("Body::Apply_Material: material applied to %s body.\n.", name.c_str() );
  return true;
}  

bool Body::Add_Light(int radius, csColor color)
{
  iSector* sector;
  csRef<iLight> light;

  // need to add a check to remove existing light if one exists 

  sector = engine->FindSector(sector_name.c_str() , 0 );
  if (!sector) printf("Body::Add_Light: Failed to locate sector !");
  printf ("Added light Sector:%s\n" , sector_name.c_str() ); 

  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  csVector3 pos(0,0,0);
  if (mesh) mesh->GetMovable()->GetFullPosition();

  light = engine->CreateLight(name.c_str(), pos, radius , color,
    CS_LIGHT_DYNAMICTYPE_DYNAMIC);


  ll->Add (light);

  //Set_Parent_Node (light->GetMovable()->GetSceneNode());
  //mesh->QuerySceneNode()->SetParent(light->GetMovable()->GetSceneNode());

  printf (" sector:%s has %d lights \n" , sector_name.c_str() , ll->GetCount() );
  printf (" pos ( %4.2f,%4.2f,%4.2f )\n" , pos.x, pos.y, pos.z );

  return true;
}

void Body::List_Light() {

  iSector* sector;
  csRef<iLight> light;

  sector = engine->FindSector(sector_name.c_str() , 0 );
  if (!sector) printf("Body::Add_Light: Failed to locate sector !");
  //	printf ("Sector:%s\n" , sector_name.c_str() ); 

  // Now we need light to see something.
  iLightList* ll = sector->GetLights (); 
  //	printf (" sector:%s has %d lights \n" , sector_name.c_str() , ll->GetCount() );
  for (size_t i = 0; i < ll->GetCount(); i++) 
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
  rot.SetAxisAngle(csVector3(0,0,1) , body_rotation*(PI / 180.0) );
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
