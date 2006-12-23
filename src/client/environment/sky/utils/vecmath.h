#ifndef PT_VECMATH_H_
#define PT_VECMATH_H_

#include <cssysdef.h>
#include <csgeom/math3d.h>
#include <csgeom/vector4.h>
#include <csgeom/transfrm.h>
#include <csgeom/matrix3.h>


csReversibleTransform xrotation(float angle);
csReversibleTransform yrotation(float angle);
csReversibleTransform zrotation(float angle);
csReversibleTransform translation(csVector3 a);

#endif