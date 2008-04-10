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

#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <cssysdef.h>
#include <csutil/weakrefarr.h>
#include <csutil/refarr.h>

#include <string>

struct iObjectRegistry;

namespace PT
{
  namespace Effect
  {
  struct Factory;

  class FactoryManager
  {
  private:
    csWeakRefArray<Factory> weakFactories;
    csRefArray<Factory> normalfactories;

  private:
    iObjectRegistry* object_reg;
    size_t cachesize;

    /// If there are more effects then our cachesize,
    /// make them overflow in the csWeakRefArray.
    void Overflow();

  public:
    FactoryManager(iObjectRegistry* object_reg);
    ~FactoryManager();

    /// Get or create a new Factory.
    csRef<Factory> Get(const std::string& fileName);

  };
} // Effect namespace
} // PT namespace

#endif // MODELMANAGER_H
