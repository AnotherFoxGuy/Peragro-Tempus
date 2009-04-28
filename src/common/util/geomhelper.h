/*
    Copyright (C) 2009 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GEOMHELPER_H
#define GEOMHELPER_H

#include <cssysdef.h>
#include <csgeom/vector3.h>
#include <csgeom/matrix3.h>

#include <wfmath/point.h>
#include <wfmath/rotmatrix.h>

struct VectorHelper
{
    static WFMath::Point<3> Convert(const csVector3& v) { return WFMath::Point<3>(v[0], v[1], v[2]); }
    static csVector3 Convert(const WFMath::Point<3>& v) { return csVector3(v[0], v[1], v[2]); }
};

struct MatrixHelper
{
    static WFMath::RotMatrix<3> Convert(const csMatrix3& m)
    {
      float arr[9];
      arr[0] = m.m11;
      arr[0] = m.m12;
      arr[0] = m.m13;

      arr[0] = m.m21;
      arr[0] = m.m22;
      arr[0] = m.m23;

      arr[0] = m.m31;
      arr[0] = m.m32;
      arr[0] = m.m33;

      WFMath::RotMatrix<3> mat;
      mat.setVals(arr);
      return mat;
    }

    static csMatrix3 Convert(const WFMath::RotMatrix<3>& m)
    {
      csMatrix3 mat;
      mat.Set(m.elem(0,0),
              m.elem(0,1),
              m.elem(0,2),

              m.elem(1,0),
              m.elem(1,1),
              m.elem(1,2),

              m.elem(2,0),
              m.elem(2,1),
              m.elem(2,2));
      return mat;
    }
};

#endif // GEOMHELPER_H
