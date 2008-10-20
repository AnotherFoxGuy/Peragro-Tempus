//File: `mybody.h'

#ifndef _H_MYBODY
#define _H_MYBODY

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
  virtual void Set_sector(char const* name) {body->Set_Sector(name); };

  virtual void Create_Body_Mesh(float radius, int verts, double day, double i);
  virtual void Set_Mesh(char const* mesh_name);
  virtual void Set_Materal(char const* mat_name);
  //	virtual bool Set_mesh (iMeshWrapper* );

  virtual char const* Get_Name() const {return body->Get_Name(); };

  virtual csRef<iMeshWrapper> Get_MeshWrapper();

  virtual bool Set_Parent(csRef<iMyBody> par_body );
  virtual bool Add_Child(csRef<iMyBody> child );

  virtual void Update_Body(timespec systime );
  virtual void Update_Body(timespec systime, csVector3 origin );
  virtual void Update_Meshs( const csTransform& trans, const double& body_rot, char const* sel_body);


  virtual void Add_Light(int range,csColor color);
  virtual void Update_Lights();

  virtual void Draw_Orbit( iCamera* c );
  virtual void Draw_Position( iCamera* c, timespec systime );
  virtual void Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up);

  // extras!!!
  virtual void Set_Parent_Node(iSceneNode* parent );
  virtual void Set_Camera_Parent(iSceneNode* parent );
  virtual iSceneNode* Get_SceneNode() { return body->Get_SceneNode(); };

  virtual double GetBodyRotation();
  virtual csTransform Get_Surface_Pos(float lon , float lat);
  virtual csVector3 GetSurfaceVector (float lon ,float lat);
  virtual csVector3 GetMeshUpVector (csTransform trans);
};

#endif // _H_MYBODY__

