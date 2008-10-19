/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

class iPointerLibrary;

struct iWorld : public virtual iBase
{
  SCF_INTERFACE(iWorld, 1,0,0);

  virtual bool Initialize(const std::string& name) = 0;

  /**
  * Enter the world at a horizontal (x, z) coordinate in world space.
  * @param x X coordinate.
  * @param z Z coordinate.
  */
  virtual void EnterWorld(float x, float z) = 0;

  /// Set the loaded tile grid size.
  virtual void SetGridSize(unsigned int size) = 0;
  /// Get the loaded tile grid size.
  virtual unsigned int GetGridSize() const = 0;

  /// Set the cached tile grid size.
  virtual void SetCacheSize(unsigned int size) = 0;
  /// Get the cached tile grid size.
  virtual unsigned int GetCacheSize() const = 0;

  /// Set the resource path.
  //virtual void SetResourcePath(const std::string& path) = 0;
};



#endif
