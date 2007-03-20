
#include "projector.h"

#include <iostream>
#include <cstdio>
#include <cassert>

#include <iutil/objreg.h>

#include "client/entity/ptentitymanager.h"


void Projector::init()
{
}


Projector::Projector()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = csQueryRegistry<iEngine> (obj_reg);
  if(!engine) return;
  g3d = csQueryRegistry<iGraphics3D> (obj_reg);
  if(!g3d) return;

  entmgr = PointerLibrary::getInstance()->getEntityManager();

  vec_viewport = Vec4i(0, 0, (int)g3d->GetWidth(), (int)g3d->GetHeight());

  flip_horz = 0.0;
  flip_vert = 0.0;

  zNear = 0.1;
  zFar = 1000;
  if (entmgr)
  {
    csRef<iPcDefaultCamera> cam = entmgr->getOwnCamera();
    if (cam.IsValid())
    {
      zFar = cam->GetCamera()->GetFarPlane()->D();
    }
  }

  init_project_matrix();
}

Projector::~Projector()
{
}

void Projector::unproject_local(double x, double y, Vec3d& v) const
{
  if (!entmgr) return;
  csRef<iPcDefaultCamera> cam = entmgr->getOwnCamera();
  if (!cam.IsValid()) return;

  csOrthoTransform tm = cam->GetCamera()->GetTransform();
  csMatrix3 m = tm.GetT2O();
  if (x == 0.0 && y == 0.0)
  {
    //printf("\n%f %f %f\n%f %f %f\n%f %f %f\n", m.m11, m.m12, m.m13, m.m21, m.m22, m.m23, m.m31, m.m32, m.m33);
  }

  //printf("p %4.0f %4.0f     ", x, y);
  csVector2 point(x,g3d->GetHeight()-y);   // Y axis is inverted (why?)
  csVector3 vec = cam->GetCamera()->InvPerspective(point, 1000);
  //printf("%9.2f %9.2f %9.2f    ", vec[0], vec[1], vec[2]);
  csVector3 vec2;

  vec2 = tm.This2OtherRelative(vec); // we have local coord system centered 0 at actor that's why "Relative"
  //printf("%9.2f %9.2f %9.2f    ", vec2[0], vec2[1], vec2[2]);

  v[0] = vec2.x;
  v[1] = vec2.y;
  v[2] = vec2.z;
  v.Normalize();
  //printf("%9.6f %9.6f %9.6f    ", v[0], v[1], v[2]);

  // Convert CS to local coordinates
  // In local: Z - up, In CS: Z - forward, Y - up
  // TODO: Check transform
  Mat4d m2(-1, 0, 0, 0,
           0, 0, 1, 0,
           0, 1, 0, 0,
           0, 0, 0, 0);
  v.transfo4d(m2);
  //printf("%9.6f %9.6f %9.6f\n", v[0], v[1], v[2]);

// unproject(x, y, inv_mat_local_to_eye, v);
}

double Projector::get_fov(void) const
{
  double fov = 90;

  if (entmgr)
  {
    csRef<iPcDefaultCamera> cam = entmgr->getOwnCamera();
    if (cam.IsValid())
    {
      fov = cam->GetCamera()->GetFOVAngle();
    }
  }

  return fov;
}

// Init the viewing matrix, setting the field of view, the clipping planes, and screen ratio
// The function is a reimplementation of gluPerspective
void Projector::init_project_matrix(void)
{

  if (!entmgr) return;
  csRef<iPcDefaultCamera> cam = entmgr->getOwnCamera();
  if (!cam.IsValid()) return;

  // Calculate the projection matrix
  csOrthoTransform tm = cam->GetCamera()->GetTransform();
  csMatrix3 m = tm.GetT2O();
  Vec4d r1(m.Row1().x,m.Row1().y,m.Row1().z,0);
  Vec4d r2(m.Row2().x,m.Row2().y,m.Row2().z,0);
  Vec4d r3(m.Row3().x,m.Row3().y,m.Row3().z,-1);
  Vec4d r4(0,0,0,1);
  mat_projection = Mat4d(r1,r2,r3,r4);

/*   
	double f = 1./tan(get_fov()*M_PI/360.);
	double ratio = (double)getViewportHeight()/getViewportWidth();

	mat_projection.Set(	flip_horz*f*ratio, 0., 0., 0.,
						0., flip_vert*f, 0., 0.,
						0., 0., (zFar + zNear)/(zNear - zFar), -1.,
						0., 0., (2.*zFar*zNear)/(zNear - zFar), 0.);
*/
}

// Set the standard modelview matrices used for projection
void Projector::set_modelview_matrices(	const Mat4d& _mat_earth_equ_to_eye,
					                    const Mat4d& _mat_helio_to_eye,
					                    const Mat4d& _mat_local_to_eye,
					                    const Mat4d& _mat_j2000_to_eye)
{
  init_project_matrix();

	mat_earth_equ_to_eye = _mat_earth_equ_to_eye;
	mat_j2000_to_eye = _mat_j2000_to_eye;
	mat_helio_to_eye = _mat_helio_to_eye;
	mat_local_to_eye = _mat_local_to_eye;

	inv_mat_earth_equ_to_eye = (mat_projection*mat_earth_equ_to_eye).GetInverse();
	inv_mat_j2000_to_eye = (mat_projection*mat_j2000_to_eye).GetInverse();
	inv_mat_helio_to_eye = (mat_projection*mat_helio_to_eye).GetInverse();
	inv_mat_local_to_eye = (mat_projection*mat_local_to_eye).GetInverse();
}
