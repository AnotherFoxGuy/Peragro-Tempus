// File: `imybody.h'
 
		
#ifndef _H_IMYBODY___
#define _H_IMYBODY___

#include "csutil/scf.h"

// Interface header
struct iMyBody : public virtual iBase
{
	SCF_INTERFACE(iMyBody,1,0,0);

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
	virtual void Set_a(long double val) = 0;
	virtual void Set_i(long double val) = 0;
	virtual void Set_n(long double val) = 0;
	virtual void Set_w(long double val) = 0;
	virtual void Set_e(long double val) = 0;
	virtual void Set_period (long double val) = 0;
	virtual void Set_scale (long double val) = 0;
	virtual void Set_sector(char const* name) = 0;
	virtual void Set_Materal(char const* mat_name) = 0; 

	virtual void Set_Mesh(char const* mesh_name) = 0;
	virtual void Create_Body_Mesh(float radius, int verts, double day, double i) = 0;

	virtual char const* Get_Name() const = 0;
	virtual csRef<iMeshWrapper> Get_MeshWrapper() = 0;

	virtual bool Add_Child(csRef<iMyBody> child) = 0; 
	virtual bool Set_Parent(csRef<iMyBody> par_body) = 0;

	virtual void Set_Parent_Node(iSceneNode* parent) = 0;
	virtual void Set_Camera_Parent(iSceneNode* parent) = 0;
	virtual iSceneNode* Get_SceneNode() = 0;


//	virtual void Update_Body() = 0;  
	virtual void Update_Body(timespec systime) = 0;  
	virtual void Update_Body(timespec systime, csVector3 origin) = 0;
	virtual void Update_Meshs( const csTransform& trans, const double& body_rot, char const* sel_body) = 0;

	virtual void Add_Light(int range,csColor color) = 0;
	virtual void Update_Lights() = 0;

	virtual void Draw_Orbit(iCamera* c) = 0;
	virtual void Draw_Position(iCamera* c, timespec systime) = 0;
	virtual void Draw_Up (const iCamera* c , iGraphics3D* g3d, csVector3 up) = 0; 

	virtual double GetBodyRotation() = 0;
	virtual csTransform Get_Surface_Pos(float lon , float lat) = 0;
	virtual csVector3 GetSurfaceVector (float lon ,float lat) = 0;
	virtual csVector3 GetMeshUpVector (csTransform trans) = 0;

};


// Factory Header
struct iMyBodyFactory : public virtual iBase
{
	SCF_INTERFACE(iMyBodyFactory,1,0,0);
 
	virtual csPtr<iMyBody> CreateObject () = 0;


};

#endif 
// _H_IMYBODY___
