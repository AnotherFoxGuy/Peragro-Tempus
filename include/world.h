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
/**
* @file world.h
*
* @basic Manages the game world.
*/

#ifndef IWORLD_H
#define IWORLD_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <string>

#include <boost/shared_ptr.hpp>

class iPointerLibrary;
struct iCamera;
struct iMeshWrapper;

namespace WFMath
{
  template<const int Dim>
  class Point;
}

namespace Common
{
  namespace World
  {
    struct Object;
    typedef boost::shared_ptr<Object> Objectp;
  }
}

struct iWorld : public virtual iBase
{
  SCF_INTERFACE(iWorld, 1,0,0);

  virtual bool Initialize(const std::string& name) = 0;

  /**
  * Enter the world at a coordinate in world space.
  * @param position coordinate.
  */
  virtual void EnterWorld(WFMath::Point<3> position) = 0;

  virtual void SetMesh(iMeshWrapper* wrapper) = 0;

  virtual void UnSetMesh() = 0;

  /// Set the proximity range in units.
  virtual void SetRange(size_t size) = 0;
  /// Get the proximity range in units.
  virtual size_t GetRange() const = 0;

  virtual void CommitChanges(Common::World::Objectp object) = 0;

  /// Set the resource path.
  //virtual void SetResourcePath(const std::string& path) = 0;
};



#endif
