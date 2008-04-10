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

#include <string>

struct iObjectRegistry;

namespace PT
{
  struct Factory;

  class ModelManager
  {
  private:
    csWeakRefArray<Factory> factories;

  private:
    iObjectRegistry* object_reg;

  public:
    ModelManager(iObjectRegistry* object_reg);
    ~ModelManager();

    /// Get or create a new Factory.
    csRef<Factory> Get(const std::string& fileName);

  };

} // PT namespace

#endif // MODELMANAGER_H
