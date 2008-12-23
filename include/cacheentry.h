/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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
 * @file cacheentry.h
 *
 * @basic Used to create instances of a model.
 */

#ifndef ICACHEENTRY_H
#define ICACHEENTRY_H

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <string>

struct iMeshWrapper;

/**
 * The CacheEntry interface.
 */
struct iCacheEntry : public virtual iBase
{
  SCF_INTERFACE(iCacheEntry, 1,0,0);

  /// Returns true if finished loading, false otherwise.
  virtual bool IsFinished() const = 0;

  /// Returns true if successfully loaded, false otherwise.
  virtual bool WasSuccessful() const = 0;

  /// Get size of this entry in bytes.
  virtual size_t GetSize() const = 0;

  // Convience function.
  virtual iMeshWrapper* Create(const std::string& meshName,
                               const std::string& factoryName) = 0;
};

#endif // ICACHEENTRY_H
