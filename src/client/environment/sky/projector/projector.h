

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


struct iObjectRegistry;

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

	//! Get and set to define and get viewport size
	virtual void setViewport(int x, int y, int w, int h);
	void setViewportSize(int w, int h)
	{
		if (w==getViewportWidth() && h==getViewportHeight())
			return;
		setViewportWidth(w);
		setViewportHeight(h);
	}
	void setViewport(const csVector4& v) {setViewport(v[0], v[1], v[2], v[3]);}

	//! Set the horizontal viewport offset in pixels
	void setViewportPosX(int x) {setViewport(x, vec_viewport[1], vec_viewport[2], vec_viewport[3]);}
	int getViewportPosX(void) const {return vec_viewport[0];}

	//! Get/Set the vertical viewport offset in pixels
	void setViewportPosY(int y) {setViewport(vec_viewport[0], y, vec_viewport[2], vec_viewport[3]);}
	int getViewportPosY(void) const {return vec_viewport[1];}

	void setViewportWidth(int width) {setViewport(vec_viewport[0], vec_viewport[1], width, vec_viewport[3]);}
	void setViewportHeight(int height) {setViewport(vec_viewport[0], vec_viewport[1], vec_viewport[2], height);}


	int getViewportWidth(void) const {return vec_viewport[2];}
	int getViewportHeight(void) const {return vec_viewport[3];}
	const csVector4& getViewport(void) const {return vec_viewport;}

	//! Maximize viewport according to passed screen values
	void setMaximizedViewport(int screenW, int screenH) {setViewport(0, 0, screenW, screenH);}

	//! Set a centered squared viewport with passed vertical and horizontal offset
	void setSquareViewport(int screenW, int screenH, int hoffset, int voffset)
	{
		int m = MY_MIN(screenW, screenH);
		setViewport((screenW-m)/2+hoffset, (screenH-m)/2+voffset, m, m);
	}

	//! Set the current openGL viewport to projector's viewport
	void applyViewport(void) const {}


	bool getFlipHorz(void) const {return (flip_horz < 0.0);}
	bool getFlipVert(void) const {return (flip_vert < 0.0);}
	void setFlipHorz(bool flip) {
		flip_horz = flip ? -1.0 : 1.0;
		init_project_matrix();
		//glFrontFace(needGlFrontFaceCW()?GL_CW:GL_CCW);
	}
	void setFlipVert(bool flip) {
		flip_vert = flip ? -1.0 : 1.0;
		init_project_matrix();
		//glFrontFace(needGlFrontFaceCW()?GL_CW:GL_CCW);
	}
	virtual bool needGlFrontFaceCW(void) const
		{return (flip_horz*flip_vert < 0.0);}

	//! Set the Field of View in degree
	void set_fov(double f);
	//! Get the Field of View in degree
	double get_fov(void) const {return fov;}
	double getRadPerPixel(void) const {return view_scaling_factor;}

	//! Set the maximum Field of View in degree
	void setMaxFov(double max);
	//! Get the maximum Field of View in degree
	double getMaxFov(void) const {return max_fov;}

    //! If is currently zooming, return the target FOV, otherwise return current FOV
    double getAimFov(void) const
	  {return (flag_auto_zoom ? zoom_move.aim : fov);}

    void change_fov(double deltaFov);

	void set_clipping_planes(double znear, double zfar);
	void get_clipping_planes(double* zn, double* zf) const {*zn = zNear; *zf = zFar;}

	// Return true if the 2D pos is inside the viewport
	bool check_in_viewport(const csVector3& pos) const
	{	return 	(pos[1]>vec_viewport[1] && pos[1]<(vec_viewport[1] + vec_viewport[3]) &&
				pos[0]>vec_viewport[0] && pos[0]<(vec_viewport[0] + vec_viewport[2]));}

	// Set the standard modelview matrices used for projection
	void set_modelview_matrices(const csReversibleTransform& _mat_earth_equ_to_eye,
				    const csReversibleTransform& _mat_helio_to_eye,
				    const csReversibleTransform& _mat_local_to_eye,
				    const csReversibleTransform& _mat_j2000_to_eye);

	// Return in vector "win" the projection on the screen of point v in earth equatorial coordinate
	// according to the current modelview and projection matrices (reimplementation of gluProject)
	// Return true if the z screen coordinate is < 1, ie if it isn't behind the observer
	// except for the _check version which return true if the projected point is inside the screen
	inline bool project_earth_equ(const csVector3& v, csVector3& win) const
		{return project_custom(v, win, mat_earth_equ_to_eye);}

	inline bool project_earth_equ_check(const csVector3& v, csVector3& win) const
		{return project_custom_check(v, win, mat_earth_equ_to_eye);}

	inline bool project_earth_equ_line_check(const csVector3& v1, csVector3& win1, const csVector3& v2, csVector3& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_earth_equ_to_eye);}

	inline void unproject_earth_equ(double x, double y, csVector3& v) const
		{unproject(x, y, inv_mat_earth_equ_to_eye, v);}

	inline void unproject_j2000(double x, double y, csVector3& v) const
		{unproject(x, y, inv_mat_j2000_to_eye, v);}

	// taking account of precession
	inline bool project_j2000(const csVector3& v, csVector3& win) const
		{return project_custom(v, win, mat_j2000_to_eye);}

	inline bool project_j2000_check(const csVector3& v, csVector3& win) const
		{return project_custom_check(v, win, mat_j2000_to_eye);}

	inline bool project_j2000_line_check(const csVector3& v1, csVector3& win1, const csVector3& v2, csVector3& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_j2000_to_eye);}

	// Same function with input vector v in heliocentric coordinate
	inline bool project_helio_check(const csVector3& v, csVector3& win) const
		{return project_custom_check(v, win, mat_helio_to_eye);}

	inline bool project_helio(const csVector3& v, csVector3& win) const
		{return project_custom(v, win, mat_helio_to_eye);}

	inline bool project_helio_line_check(const csVector3& v1, csVector3& win1, const csVector3& v2, csVector3& win2) const
		{return project_custom_line_check(v1, win1, v2, win2, mat_helio_to_eye);}

	inline void unproject_helio(double x, double y, csVector3& v) const
		{return unproject(x, y, inv_mat_helio_to_eye, v);}

	// Same function with input vector v in local coordinate
	inline bool project_local(const csVector3& v, csVector3& win) const
		{return project_custom(v, win, mat_local_to_eye);}

	inline bool project_local_check(const csVector3& v, csVector3& win) const
		{return project_custom_check(v, win, mat_local_to_eye);}

	inline void unproject_local(double x, double y, csVector3& v) const
		{unproject(x, y, inv_mat_local_to_eye, v);}

	// Same function but using a custom modelview matrix
	virtual bool project_custom(const csVector3& v, csVector3& win, const csReversibleTransform& mat) const
	{
		//gluProject(v[0],v[1],v[2],mat,mat_projection,vec_viewport,&win[0],&win[1],&win[2]);
		return (win[2]<1.);
	}

    bool project_custom_check(const csVector3& v, csVector3& win, const csReversibleTransform& mat) const
		{return (project_custom(v, win, mat) && check_in_viewport(win));}
	// project two points and make sure both are in front of viewer and that at least one is on screen

    bool project_custom_line_check(const csVector3& v1, csVector3& win1,
					       const csVector3& v2, csVector3& win2, const csReversibleTransform& mat) const
		{return project_custom(v1, win1, mat) && project_custom(v2, win2, mat) &&
		   (check_in_viewport(win1) || check_in_viewport(win2));}


	// Set the drawing mode in 2D for drawing inside the viewport only.
	// Use reset_perspective_projection() to restore previous projection mode
	void set_orthographic_projection(void) const;

	// Restore the previous projection mode after a call to set_orthographic_projection()
	void reset_perspective_projection(void) const;


	//! Return the initial default FOV in degree
	double getInitFov() const {return initFov;}

protected:
	// Struct used to store data for auto mov
	typedef struct
	{
		double start;
	    double aim;
	    float speed;
	    float coef;
	}auto_zoom;

	// Init the viewing matrix from the fov, the clipping planes and screen ratio
	// The function is a reimplementation of gluPerspective
	virtual void init_project_matrix(void);

	double initFov;				// initial default FOV in degree
	double fov;					// Field of view in degree
	double min_fov;				// Minimum fov in degree
	double max_fov;				// Maximum fov in degree
	double zNear, zFar;			// Near and far clipping planes
	csVector4 vec_viewport;			// Viewport parameters
	csReversibleTransform mat_projection;		// Projection matrix

	csVector3 center;				// Viewport center in screen pixel
	double view_scaling_factor;	// ??
	double flip_horz,flip_vert;

	csReversibleTransform mat_earth_equ_to_eye;		// Modelview Matrix for earth equatorial projection
	csReversibleTransform mat_j2000_to_eye;         // for precessed equ coords
	csReversibleTransform mat_helio_to_eye;			// Modelview Matrix for earth equatorial projection
	csReversibleTransform mat_local_to_eye;			// Modelview Matrix for earth equatorial projection
	csReversibleTransform inv_mat_earth_equ_to_eye;	// Inverse of mat_projection*mat_earth_equ_to_eye
	csReversibleTransform inv_mat_j2000_to_eye;		// Inverse of mat_projection*mat_earth_equ_to_eye
	csReversibleTransform inv_mat_helio_to_eye;		// Inverse of mat_projection*mat_helio_to_eye
	csReversibleTransform inv_mat_local_to_eye;		// Inverse of mat_projection*mat_local_to_eye

	// transformation from screen 2D point x,y to object
	// m is here the already inverted full tranfo matrix
	virtual
    void unproject(double x, double y, const csReversibleTransform& m, csVector3& v) const
	{
		v.Set(	(x - vec_viewport[0]) * 2. / vec_viewport[2] - 1.0,
				(y - vec_viewport[1]) * 2. / vec_viewport[3] - 1.0,
				1.0);
		v = m*v;
	}

	// Automove
	auto_zoom zoom_move;		// Current auto movement
	bool flag_auto_zoom;		// Define if autozoom is on or off

    private:
      csRef<iEngine> engine;
      csRef<iGraphics3D> g3d;
};

#endif // _PROJECTOR_H_
