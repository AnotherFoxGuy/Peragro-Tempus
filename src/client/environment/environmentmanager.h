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

#ifndef ENVIRONMENTMANAGER_H
#define ENVIRONMENTMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/parray.h>
#include <iutil/vfs.h>
#include <iutil/virtclk.h>
#include <iengine/engine.h>
#include <csgeom/path.h>
#include <csgeom/math3d.h>
#include <iutil/object.h>
#include <iutil/strset.h>
#include <csutil/cscolor.h>
#include <iutil/document.h>

#include <csutil/csstring.h>

#include "common/util/mutex.h"
#include "client/pointer/pointer.h"

struct iObjectRegistry;
struct iLoader;
struct iDocument;

class EnvironmentMGR
{
public:
  struct TimeData
  {
    unsigned int hour;
  };

private:
  csString sunname;
  struct Sun
  {
    unsigned int id;
    csColor color;
    csVector3 position;
  };
  csArray<Sun*> sunarray;

private:
  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iStringSet> stringset;
  Mutex mutex;

  csPDelArray<TimeData> timearray;

private:
  void Time();

public:
  EnvironmentMGR();
  ~EnvironmentMGR();
  bool Initialize(); 
  void Handle();

  void Time(unsigned int hour);
};

#endif // ENVIRONMENTMANAGER_H
