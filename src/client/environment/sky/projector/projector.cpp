

#include <iostream>
#include <cstdio>
#include <cassert>

#include "projector.h"

#include <iutil/objreg.h>


void Projector::init()
{
  set_fov(60.);
}


Projector::Projector()
          :fov(60.0), min_fov(0.0001), max_fov(100),
           zNear(0.1), zFar(10000)
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = csQueryRegistry<iEngine> (obj_reg);
  if(!engine) return;
  g3d = csQueryRegistry<iGraphics3D> (obj_reg);
  if(!g3d) return;

  Vec4i vec_viewport(0, 0, (float)g3d->GetWidth(), (float)g3d->GetHeight());

  flip_horz = 1.0;
  flip_vert = 1.0;
  setViewport(vec_viewport);

}

Projector::~Projector()
{
}

// Init the viewing matrix, setting the field of view, the clipping planes, and screen ratio
// The function is a reimplementation of gluPerspective
void Projector::init_project_matrix(void)
{
	double f = 1./tan(fov*M_PI/360.);
	double ratio = (double)getViewportHeight()/getViewportWidth();
/*
    // Calculate the projection matrix TODO is this correct?
    csReversibleTransform tm = g3d->GetWorldToCamera();
    csMatrix3 m = tm.GetO2T();
    csVector4 r1(m.Row1().x,m.Row1().y,m.Row1().z,0);
    csVector4 r2(m.Row2().x,m.Row2().y,m.Row2().z,0);
    csVector4 r3(m.Row3().x,m.Row3().y,m.Row3().z,0);
    csVector4 r4(0,0,0,1);
    mat_projection = csMatrix4(r1,r2,r3,r4);
*/
    
	mat_projection.Set(	flip_horz*f*ratio, 0., 0., 0.,
						0., flip_vert*f, 0., 0.,
						0., 0., (zFar + zNear)/(zNear - zFar), -1.,
						0., 0., (2.*zFar*zNear)/(zNear - zFar), 0.);

    /*
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(mat_projection);
    glMatrixMode(GL_MODELVIEW);
    */
}

void Projector::setViewport(int x, int y, int w, int h)
{
	vec_viewport[0] = x;
	vec_viewport[1] = y;
	vec_viewport[2] = w;
	vec_viewport[3] = h;
	center.Set(vec_viewport[0]+vec_viewport[2]/2,vec_viewport[1]+vec_viewport[3]/2,0);
	view_scaling_factor = 1.0/fov*180./M_PI*MY_MIN(getViewportWidth(),getViewportHeight());
	//glViewport(x, y, w, h);
	init_project_matrix();
}

void Projector::set_fov(double f)
{
	fov = f;
	if (f>max_fov) fov = max_fov;
	if (f<min_fov) fov = min_fov;
	view_scaling_factor = 1.0/fov*180./M_PI*MY_MIN(getViewportWidth(),getViewportHeight());
	init_project_matrix();
}

void Projector::setMaxFov(double max) {
  if (fov > max) set_fov(max);
  max_fov = max;
}

void Projector::set_clipping_planes(double znear, double zfar)
{
	zNear = znear;
	zFar = zfar;
	init_project_matrix();
}

void Projector::change_fov(double deltaFov)
{
  // if we are zooming in or out
  if (deltaFov) set_fov(fov+deltaFov);
}


// Set the standard modelview matrices used for projection
void Projector::set_modelview_matrices(	const Mat4d& _mat_earth_equ_to_eye,
					                    const Mat4d& _mat_helio_to_eye,
					                    const Mat4d& _mat_local_to_eye,
					                    const Mat4d& _mat_j2000_to_eye)
{
	mat_earth_equ_to_eye = _mat_earth_equ_to_eye;
	mat_j2000_to_eye = _mat_j2000_to_eye;
	mat_helio_to_eye = _mat_helio_to_eye;
	mat_local_to_eye = _mat_local_to_eye;

	inv_mat_earth_equ_to_eye = (mat_projection*mat_earth_equ_to_eye).GetInverse();
	inv_mat_j2000_to_eye = (mat_projection*mat_j2000_to_eye).GetInverse();
	inv_mat_helio_to_eye = (mat_projection*mat_helio_to_eye).GetInverse();
	inv_mat_local_to_eye = (mat_projection*mat_local_to_eye).GetInverse();
}

// Set the drawing mode in 2D. Use reset_perspective_projection() to reset
// previous projection mode
void Projector::set_orthographic_projection(void) const
{
  /*
	glMatrixMode(GL_PROJECTION);		// projection matrix mode
    glPushMatrix();						// store previous matrix
    glLoadIdentity();
    gluOrtho2D(	vec_viewport[0], vec_viewport[0] + vec_viewport[2],
				vec_viewport[1], vec_viewport[1] + vec_viewport[3]);	// set a 2D orthographic projection
	glMatrixMode(GL_MODELVIEW);			// modelview matrix mode
    glPushMatrix();
    glLoadIdentity();
    */
}

// Reset the previous projection mode after a call to set_orthographic_projection()
void Projector::reset_perspective_projection(void) const
{
  /*
    glMatrixMode(GL_PROJECTION);		// Restore previous matrix
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    */
}
