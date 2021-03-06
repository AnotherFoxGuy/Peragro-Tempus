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
 * @file environmentmanager.h
 *
 * @brief Changes the sky graphics.
 */

#ifndef ENVIRONMENTMANAGER_H_
#define ENVIRONMENTMANAGER_H_

#include <boost/scoped_ptr.hpp>

#include <cssysdef.h>
#include <csutil/ref.h>
#include <imap/loader.h>
#include <iengine/engine.h>
#include <csgfx/shadervar.h>

#include "common/eventcs/event.h"
#include "common/eventcs/eventmanager.h"

// Solarsystem plugins.
#include "imystarbox.h"
#include "isolarsys.h"
#include "imybody.h"

struct iEngine;
struct iLight;
struct iEvent;
struct iCamera;
struct iShaderManager;

namespace PT
{
  namespace Environment
  {
    class Clock;

    /**
     * Changes the sky graphics as the time of day changes.
     */
    class EnvironmentManager
    {
    private:
      /// The engine.
      csRef<iEngine> engine;
      /// The shader manager.
      csRef<iShaderManager> shaderMgr;
      /// The string set.
      csRef<iShaderVarStringSet> strings;

      /// The light for the sun.
      csRef<iLight> sun;

      /// TODO: <camera /> is broken in CS atm, work around.
      /// The sky mesh.
      csRef<iMeshWrapper> sky;
      /// The cloud mesh.
      csRef<iMeshWrapper> clouds;
      void UpdateCameraMesh(iMeshWrapper* m, iRenderView* rview);
      // end TODO.

      /// The game clock.
      boost::scoped_ptr<Clock> clock;
      /// The horizontal angle of the sun.
      float sun_alpha;
      /// The vertical angle of the sun.
      float sun_theta;
      /// The sun direction string.
      CS::ShaderVarStringID string_sunDirection;
      /// Elapsed ticks counter to limit the calculations done.
      csTicks elapsedTicks;
      /// Allows the setting of the base ambient lighting in options.
      float min_light;

      bool UpdateOptions(iEvent& ev);
      void UpdateOptions();
      PT_CALLBACK_HANDLER_LISTENERS

      struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
      {
        EnvironmentManager* envmgr;
        FrameCallBack (EnvironmentManager* mgr) : scfImplementationType (this) { envmgr = mgr; }
        virtual void StartFrame (iEngine* engine, iRenderView* rview);
      };
      friend struct FrameCallBack;
      csRef<FrameCallBack> cb;

      /**
       * Update the sun and sky graphics.
       * @return True, indicating that the event was handled.
       */
      void Update(iCamera* cam);

      /**
       * Update the solar system plugin.
       */
      void UpdateSolarsys(iCamera* cam);

      // Pointer to plugin that draws the stars and planets / moons.
      csRef<iSolarsys> solarsys;

    public:
      /// Constructor.
      EnvironmentManager();
      /// Destructor.
      ~EnvironmentManager();

      /**
       * Initialize the environmentmanager.
       * @return True, indicating success.
       */
      bool Initialize();

      /**
       * Get a pointer to the clock object.
       * @return Clock*
       */
      const Clock* GetClock() const { return clock.get(); }

    };
  } // Environment namespace
} // PT namespace

#endif // ENVIRONMENTMANAGER_H_
