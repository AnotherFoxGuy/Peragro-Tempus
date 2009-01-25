/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
/**
 * @file ptvector3.h
 * @brief 3d vector class.
 */

#ifndef GEOM_VECTOR3_H
#define GEOM_VECTOR3_H

#include <iostream>
#include <cmath>

#include "common/constants.h"

namespace Geom
{
  class Sphere;

  /**
   * Simple convenience class representing 3D vector. Access the coordinates
   * directly via members called x, y, and z.
   */
  class Vector3
  {
  public:
    /// X dimension.
    float x;
    /// Y dimension.
    float y;
    /// Z dimension.
    float z;

    /// Default constructor, vector is initialized to 0.
    Vector3()
      : x(0.0f), y(0.0f), z(0.0f)
    {}

    /// Vector initialized to the value passed.
    Vector3(float v)
      : x(v), y(v), z(v)
    {}

    /// Vector initialized to the parameters passed.
    Vector3(float x, float y, float z)
      : x(x), y(y), z(z)
    {}

    /// Copy constructor.
    Vector3(const Vector3& v)
      : x(v.x), y(v.y), z(v.z)
    {}

    /// Templated copy constructor.
    template<typename otherVector>
    Vector3(const otherVector& v)
      : x(v.x), y(v.y), z(v.z)
    {}

    /// Templated conversion function.
    template<typename otherVector>
    operator otherVector() const
    { return otherVector(x, y, z); }

    /// Assignment operator.
    inline Vector3& operator=(const Vector3& v)
    { x = v.x; y = v.y; z = v.z; return *this; }

    inline float& operator[] (size_t n) { return !n?x:n&1?y:z; }

    inline float operator[] (size_t n) const { return !n?x:n&1?y:z; }

    /// Unary plus.
    inline Vector3 operator+() const
    { return *this; }

    /// Unary minus.
    inline Vector3 operator-() const
    { return Vector3(-x, -y, -z); }

    /// Add another vector to this.
    inline Vector3& operator+=(const Vector3& v)
    { x += v.x; y += v.y; z += v.z; return *this; }

    /// Subtract another vector from this.
    inline Vector3& operator-=(const Vector3& v)
    { x -= v.x; y -= v.y; z -= v.z; return *this; }

    /// Multiply all dimensions by a number.
    inline Vector3& operator*=(float f)
    { x *= f; y *= f; z *= f; return *this; }

    /// Divide all dimensions by a number.
    inline Vector3& operator/=(float f)
    { f = 1.0f / f; x *= f; y *= f; z *= f; return *this; }

    /// Test equality.
    inline bool operator==(const Vector3& v) const
    {
      return (((x - v.x > -PT_EPSILON) && (x - v.x < PT_EPSILON)) &&
        ((y - v.y > -PT_EPSILON) && (y - v.y < PT_EPSILON)) &&
        ((z - v.z > -PT_EPSILON) && (z - v.z < PT_EPSILON)));
    }

    /// Test inequality.
    inline bool operator!=(const Vector3& v) const
    {
      return (((x - v.x < -PT_EPSILON) || (x - v.x > PT_EPSILON)) &&
        ((y - v.y < -PT_EPSILON) || (y - v.y > PT_EPSILON)) &&
        ((z - v.z < -PT_EPSILON) || (z - v.z > PT_EPSILON)));
    }

    /// Add two vectors.
    inline friend Vector3 operator+(const Vector3& v1, const Vector3& v2)
    { return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z); }

    /// Subtract one vector from another.
    inline friend Vector3 operator-(const Vector3& v1, const Vector3& v2)
    { return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z); }

    /// Take the dot product of two vectors.
    inline friend float operator*(const Vector3& v1, const Vector3& v2)
    { return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z; }

    /// Multiply a vector and a scalar.
    inline friend Vector3 operator* (const Vector3& v, float f)
    { return Vector3 (v.x*f, v.y*f, v.z*f); }

    /// Divide a vector by a scalar.
    inline friend Vector3 operator/ (const Vector3& v, float f)
    { f = 1.0f/f; return Vector3 (v.x*f, v.y*f, v.z*f); }

    /// Get the distance between two vectors.
    inline friend float Distance(const Vector3& v1, const Vector3& v2)
    { Vector3 diff(v1 - v2); return sqrt(diff * diff); }

    /// Get the squared distance between two vectors.
    inline friend float Distance2(const Vector3& v1, const Vector3& v2)
    { Vector3 diff(v1 - v2); return (diff * diff); }

    /// Insertion operator.
    friend std::istream& operator>>(std::istream& is, Vector3& v);

    /// Extraction operator.
    friend std::ostream& operator<<(std::ostream& os, const Vector3& v);

    /// True if all dimensions are less than a defined limit.
    inline bool IsZero() const
    {
      return (((x > -PT_EPSILON) && (x < PT_EPSILON)) &&
        ((y > -PT_EPSILON) && (y < PT_EPSILON)) &&
        ((z > -PT_EPSILON) && (z < PT_EPSILON)));
    }

    /// Get the vector formatted as a string.
    std::string ToString() const;

    bool Intersect (const Vector3& v) const { return false; }
    bool Intersect (const Sphere& sphere) const { return false; }

  };

} // namespace Geom

typedef Geom::Vector3 PtVector3;

#endif // GEOM_VECTOR3_H
