/*
    Copyright (C) 2008 Development Team of Peragro Tempus

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef TERRAINED_H_
#define TERRAINED_H_

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csutil/csstring.h>
#include <iutil/eventnames.h>
#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>

#include <csutil/hash.h>

#include <csutil/refcount.h>

#include "include/terrained.h"

struct iObjectRegistry;

class TerrainEd : public scfImplementation2<TerrainEd, iTerrainEd, iComponent>
{
private:
  iObjectRegistry* object_reg;

public:
  TerrainEd (iBase* parent);
  virtual ~TerrainEd();

  virtual bool Initialize (iObjectRegistry*);
};

#endif // TERRAINED_H_