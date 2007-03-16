

#ifndef _STEL_OBJECT_BASE_H_
#define _STEL_OBJECT_BASE_H_

#include <cssysdef.h>
/*
#include <csgeom/math3d.h>
#include <csgeom/matrix3.h>
#include <csgeom/transfrm.h>
*/

#include <csutil/csstring.h>

#include "client/environment/sky/utils/vecmath.h"

#include <iostream>

using namespace std;

class Navigator;
class Projector;

class StelObjectBase {
public:

  enum STEL_OBJECT_TYPE {
    STEL_OBJECT_UNINITIALIZED,
    STEL_OBJECT_STAR,
    STEL_OBJECT_PLANET,
    STEL_OBJECT_NEBULA,
    STEL_OBJECT_CONSTELLATION,
    STEL_OBJECT_TELESCOPE
  };

  virtual ~StelObjectBase(void) {}

  virtual void update(void) {}


  //! Return object's type
  virtual STEL_OBJECT_TYPE get_type(void) const = 0;

  //! Return object's name
  virtual csString getEnglishName(void) const = 0;
  virtual wstring getNameI18n(void) const = 0;

  //! Get position in earth equatorial frame
  virtual Vec3d get_earth_equ_pos(const Navigator *nav) const = 0;

  //! observer centered J2000 coordinates
  virtual Vec3d getObsJ2000Pos(const Navigator *nav) const = 0;

  //! Return object's magnitude
  virtual float get_mag(const Navigator *nav) const = 0;

  //! Get object main color, used to display infos
  virtual Vec3f get_RGB(void) const {return Vec3f(1.,1.,1.);}

  //! Return the best FOV in degree to use for a close view of the object
  virtual double get_close_fov(const Navigator *nav) const {return 10.;}

  //! Return the best FOV in degree to use for a global view of the object satellite system (if there are satellites)
  virtual double get_satellites_fov(const Navigator *nav) const {return -1.;}
  virtual double get_parent_satellites_fov(const Navigator *nav) const
    {return -1.;}

protected:
  virtual float get_on_screen_size(const Projector *prj,
                                   const Navigator *nav = NULL) {return 0;}
private:
  static int local_time;
};

#endif
