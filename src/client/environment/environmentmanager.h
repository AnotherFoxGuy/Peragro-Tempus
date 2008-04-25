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

#ifndef ENVIRONMENTMANAGER_H_
#define ENVIRONMENTMANAGER_H_

#include <cssysdef.h>
#include <csutil/ref.h>
#include <imap/loader.h>
#include <iengine/engine.h>

#include "client/event/event.h"

#include "client/environment/clock.h"

struct iEngine;
struct iLight;
struct iEvent;
struct iCamera;
struct iShaderManager;

namespace PT
{
  class EnvironmentManager
  {
  private:
    csRef<iEngine> engine;
    csRef<iShaderManager> shaderMgr;
    csRef<iStringSet> strings;
    csRef<iLight> sun;
    csRef<iMeshWrapper> sky;

    Clock* clock;

    float sun_alfa;
    float sun_theta;

    csStringID string_sunDirection;

    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      EnvironmentManager* envmgr;
      FrameCallBack (EnvironmentManager* mgr) : scfImplementationType (this) { envmgr = mgr; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

    void Update(iCamera* cam);

  public:
    EnvironmentManager();
    ~EnvironmentManager();

   /**
    *Initialize the environmentmanager
    *@return True, indicating success
    */
    bool Initialize();

  };
}

#endif // ENVIRONMENTMANAGER_H_
