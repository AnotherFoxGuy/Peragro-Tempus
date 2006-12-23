
#include "vecmath.h"

csReversibleTransform xrotation(float angle)
{
  float c = cos(angle);
  float s = sin(angle);

  csMatrix3 mat(1, 0, 0, 
                0, c, s,
                0,-s, c);

  csReversibleTransform transf(mat, csVector3(0,0,0));

  return transf;
}

csReversibleTransform yrotation(float angle)
{
  float c = cos(angle);
  float s = sin(angle);

  csMatrix3 mat(c, 0,-s,
                0, 1, 0,
                s, 0, c);

  csReversibleTransform transf(mat, csVector3(0,0,0));

  return transf;
}

csReversibleTransform zrotation(float angle)
{
  float c = cos(angle);
  float s = sin(angle);

  csMatrix3 mat(c, s, 0,
                -s, c, 0,
                0, 0, 1);

  csReversibleTransform transf(mat, csVector3(0,0,0));

  return transf;
}

csReversibleTransform translation(csVector3 a)
{
  csReversibleTransform transf;
  transf.Identity();
  transf.Translate( csVector3(a[0], a[1], a[2]) );

  return transf;
}

