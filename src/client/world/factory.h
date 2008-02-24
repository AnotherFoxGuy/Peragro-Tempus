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

#ifndef FACTORY_H
#define FACTORY_H

#include <string>

struct iObjectRegistry;
class FileLoader;

namespace PT
{
  struct iModelFactory : public virtual iBase
  {
    SCF_INTERFACE(iModelFactory, 1,0,0);
  };

  struct Factory : public scfImplementation1<Factory, iModelFactory>
  {
  private:
    csRef<iRegion> region;
    FileLoader* fileLoader;
    iObjectRegistry* object_reg;
    bool isPrecached;
    bool isAdded;

  public:
    Factory(const std::string& fileName, iObjectRegistry* object_reg);
    ~Factory();
    std::string fileName;
    /// Don't call this function yourself!
    void Load();
    /// @return True if finished loading, false otherwise.
    bool IsReady() const;
    /// Return true if all textures have been precached.
    bool IsPrecached() const { return isPrecached; }
    /// Check if everything has been added to the engine.
    bool IsAdded() const { return isAdded; }
    /** Add the contents to the engine.
    *  Careful, this will block when it isn't ready yet!
    */
    void AddToEngine();
    /** Precaches 'one' texture (if any) and returns.
    *  Call this repeatetly over a period of time
    *  to smooth out texture loading.
    *  Call IsPrecached() to see if everything has been precached.
    */
    void Precache();
    /// Get the region that is used internally for holding the data.
    iRegion* GetRegion() { return region; }
  };

} // PT namespace

#endif // FACTORY_H
