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
  printf("Mybody::Initialize\n");
  //body = new Body(object_reg);
  //if (!body) body = new Body(object_reg); 
  if (!engine) engine = csQueryRegistry<iEngine> (object_reg);
  if (!g3d) g3d = csQueryRegistry<iGraphics3D> (object_reg);
  //if (!g3d || !engine || !body ) initialized = false; 

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

void MyBody::Set_Parent_Node(iSceneNode* parent)
{
  body->Set_Parent_Node(parent);
}	 

void MyBody::Set_Camera_Parent(iSceneNode* parent )
{
  body->Set_Camera_Parent(parent);
}

csRef<iMeshWrapper> MyBody::Get_MeshWrapper()
{
  return 	body->Get_MeshWrapper();
}

void MyBody::Draw_Orbit(iCamera* c) 
{
  body->Draw_FullOrbit(c, g3d );
}

void MyBody::Draw_Position(iCamera* c ,timespec systime ) 
{
  body->Draw_FullPosition(c, g3d, systime);
}

void MyBody::Update_Body(timespec systime)
{
  body->Update_Body(systime);
}

void MyBody::Update_Body(timespec systime, csVector3 origin )
{
  body->Update_Body(systime, origin);
}

void MyBody::Update_Meshs( const csTransform& trans, const double& body_rot, char const* sel_body)
{
  body->Update_Meshs(trans, body_rot, sel_body);
}

double MyBody::GetBodyRotation() 
{
  return body->GetBodyRotation();
}

csTransform MyBody::Get_Surface_Pos(float lon , float lat)
{
  return body->Get_Surface_Pos(lon, lat);
}

csVector3 MyBody::GetSurfaceVector (float lon ,float lat)
{
  return body->GetSurfaceVector(lon, lat);
}

csVector3 MyBody::GetMeshUpVector(csTransform trans)
{
  return body->GetMeshUpVector(trans);
}

void MyBody::Set_Materal(char const* mat_name)
{
  body->Set_Materal(mat_name);
}

void MyBody::Update_Lights()
{
  body->Update_Lights();
}
void MyBody::Set_Mesh(char const* mesh_name)
{
  body->Set_Materal(mesh_name);	
}
void MyBody::Add_Light(int range,csColor color)
{
  body->Add_Light(range, color);
}

void MyBody::Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up)
{
  body->Draw_Up(c, g3d, up);
}