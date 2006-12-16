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

#ifndef SKYMANAGER_H
#define SKYMANAGER_H

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

#include "client/pointer/pointer.h"

#include "client/environment/sky/atmosphere/atmosphere.h"
#include "client/environment/sky/projector/projector.h"
#include "client/environment/sky/navigator/navigator.h"
#include "client/environment/sky/utils/tone_reproductor.h"
#include "client/environment/sky/utils/time.h"

struct iObjectRegistry;
struct iLoader;
struct iDocument;

class SkyMGR
{
private:
  csRef<iEngine> engine;
  csRef<iVFS> vfs;
  csRef<iGraphics3D> g3d;

  Atmosphere* atmosphere;
  ToneReproductor* tone_converter;
  Navigator * navigation;
  Projector* projection;

  int timer;
  float sky_brightness;
  double deltaAlt, deltaAz;	// View movement

public:
  SkyMGR();
  ~SkyMGR();
  bool Initialize(); 
  void Handle();
  void Update(int delta_time);
  void updateMove(int delta_time);
  void turn(int turn, int pitch);
};

#endif // SKYMANAGER_H
