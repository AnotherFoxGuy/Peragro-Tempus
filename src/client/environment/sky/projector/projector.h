

#ifndef _PROJECTOR_H_
#define _PROJECTOR_H_

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csgeom/math3d.h>
#include <iutil/object.h>
#include <csutil/cscolor.h>
#include <ivideo/graph3d.h>
#include <csgeom/vector4.h>

#include "client/pointer/pointer.h"

#include "client/environment/sky/utils/time.h"
#include "client/environment/sky/utils/vecmath.h"


struct iObjectRegistry;
struct iPcDefaultCamera;

// Class which handle projection modes and projection matrix
// Overide some function usually handled by glu
class Projector
{
public:
	enum PROJECTOR_TYPE
	{
		PERSPECTIVE_PROJECTOR    = 0,
	};


    // Main factory constructor
    Projector();
	virtual ~Projector();

	void init();

	virtual PROJECTOR_TYPE getType(void) const {return PERSPECTIVE_PROJECTOR;}

	//! Set the horizontal viewport offset in pixels
	int getViewportPosX(void) const {return vec_viewport[0];}

	//! Get the vertical viewport offset in pixels
	int getViewportPosY(void) const {return vec_viewport[1];}

	int getViewportWidth(void) const {return vec_viewport[2];}
	int getViewportHeight(void) const {return vec_viewport[3];}
	const Vec4i& getViewport(void) const {return vec_viewport;}

    //! Get the Field of View in degree
	double get_fov(void) const;
	double getRadPerPixel(void) const {return view_scaling_factor;}

	// Return true if the 2D pos is inside the viewport
	bool check_in_viewport(const Vec3d& pos) const
	{	return 	(pos[1]>vec_viewport[1] && pos[1]<(vec_viewport[1] + vec_viewport[3]) &&
				pos[0]>vec_viewport[0] && pos[0]<(vec_viewport[0] + vec_viewport[2]));}

	// Set the standard modelview matrices used for projection
	void set_modelview_matrices(const Mat4d& _mat_earth_equ_to_eye,
				                const Mat4d& _mat_helio_to_eye,
				                const Mat4d& _mat_local_to_eye,
				                const Mat4d& _mat_j2000_to_eye);

	// Return in vector "win" the projection on the screen of point v in earth equatorial coordinate
	// according to the current modelview and projection matrices (reimplementation of gluProject)
	// Return true if the z screen coordinate is < 1, ie if it isn't behind the observer
	// except for the _check version which return true if the projected point is inside the screen
	inline bool project_earth_equ(const Vec3d& v, Vec3d& win) const
		{return project_custom(v, win, mat_earth_equ_to_eye);}

	inline bool project_earth_equ_check(const Vec3d& v, Vec3d& win) const
		{return project_custom_check(v, win, mat_earth_equ_to_eye);}

	inline bool project_earth_equ_line_check(const Vec3d& v1, Vec3d& win1, const Vec3d& v2, Vec3d& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_earth_equ_to_eye);}

	inline void unproject_earth_equ(double x, double y, Vec3d& v) const
		{unproject(x, y, inv_mat_earth_equ_to_eye, v);}

	inline void unproject_j2000(double x, double y, Vec3d& v) const
		{unproject(x, y, inv_mat_j2000_to_eye, v);}

	// taking account of precession
	inline bool project_j2000(const Vec3d& v, Vec3d& win) const
		{return project_custom(v, win, mat_j2000_to_eye);}

	inline bool project_j2000_check(const Vec3d& v, Vec3d& win) const
		{return project_custom_check(v, win, mat_j2000_to_eye);}

	inline bool project_j2000_line_check(const Vec3d& v1, Vec3d& win1, const Vec3d& v2, Vec3d& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_j2000_to_eye);}

	// Same function with input vector v in heliocentric coordinate
	inline bool project_helio_check(const Vec3d& v, Vec3d& win) const
		{return project_custom_check(v, win, mat_helio_to_eye);}

	inline bool project_helio(const Vec3d& v, Vec3d& win) const
		{return project_custom(v, win, mat_helio_to_eye);}

	inline bool project_helio_line_check(const Vec3d& v1, Vec3d& win1, const Vec3d& v2, Vec3d& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_helio_to_eye);}

	inline void unproject_helio(double x, double y, Vec3d& v) const
		{return unproject(x, y, inv_mat_helio_to_eye, v);}

	// Same function with input vector v in local coordinate
	inline bool project_local(const Vec3d& v, Vec3d& win) const
		{return project_custom(v, win, mat_local_to_eye);}

	inline bool project_local_check(const Vec3d& v, Vec3d& win) const
		{return project_custom_check(v, win, mat_local_to_eye);}

/*
	inline void unproject_local(double x, double y, Vec3d& v) const
		{unproject(x, y, inv_mat_local_to_eye, v);}
*/
    void unproject_local(double x, double y, Vec3d& v) const;

	// Same function but using a custom modelview matrix
	virtual bool project_custom(const Vec3d& v, Vec3d& win, const Mat4d& mat) const
	{
      //TODO
		//gluProject(v[0],v[1],v[2],mat,mat_projection,vec_viewport,&win[0],&win[1],&win[2]);
		return (win[2]<1.);
	}

    bool project_custom_check(const Vec3d& v, Vec3d& win, const Mat4d& mat) const
		{return (project_custom(v, win, mat) && check_in_viewport(win));}
	// project two points and make sure both are in front of viewer and that at least one is on screen

    bool project_custom_line_check(const Vec3f& v1, Vec3d& win1,
					       const Vec3f& v2, Vec3d& win2, const Mat4d& mat) const
		{return project_custom(v1, win1, mat) && project_custom(v2, win2, mat) &&
		   (check_in_viewport(win1) || check_in_viewport(win2));}

protected:
	// Init the viewing matrix from the fov, the clipping planes and screen ratio
	// The function is a reimplementation of gluPerspective
	virtual void init_project_matrix(void);

	double zNear, zFar;			// Near and far clipping planes
	Vec4i vec_viewport;			// Viewport parameters

	Mat4d mat_projection;		// Projection matrix

	Vec3d center;				// Viewport center in screen pixel
	double view_scaling_factor;	// ??
	double flip_horz,flip_vert;

	Mat4d mat_earth_equ_to_eye;		// Modelview Matrix for earth equatorial projection
	Mat4d mat_j2000_to_eye;         // for precessed equ coords
	Mat4d mat_helio_to_eye;			// Modelview Matrix for earth equatorial projection
	Mat4d mat_local_to_eye;			// Modelview Matrix for earth equatorial projection
	Mat4d inv_mat_earth_equ_to_eye;	// Inverse of mat_projection*mat_earth_equ_to_eye
	Mat4d inv_mat_j2000_to_eye;		// Inverse of mat_projection*mat_earth_equ_to_eye
	Mat4d inv_mat_helio_to_eye;		// Inverse of mat_projection*mat_helio_to_eye
	Mat4d inv_mat_local_to_eye;		// Inverse of mat_projection*mat_local_to_eye

	// transformation from screen 2D point x,y to object
	// m is here the already inverted full tranfo matrix
	virtual
    void unproject(double x, double y, const Mat4d& m, Vec3d& v) const
	{
		v.Set(	(x - vec_viewport[0]) * 2. / vec_viewport[2] - 1.0,
				(y - vec_viewport[1]) * 2. / vec_viewport[3] - 1.0,
				1.0);
        v.transfo4d(m);
	}

    private:
      csRef<iEngine> engine;
      csRef<iGraphics3D> g3d;
      ptEntityManager* entmgr;
};

#endif // _PROJECTOR_H_
