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
 * @file interiormanager.h
 *
 * @basic Manages interior spaces.
 */

#ifndef INTERIORMANAGER_H
#define INTERIORMANAGER_H

#include <cssysdef.h>
#include <csutil/weakrefarr.h>
#include <iengine/engine.h>

#include <string>

struct iObjectRegistry;
struct iDocumentNode;

class WorldManager;

namespace PT
{
  namespace World
  {
    struct Interior;
    struct InteriorFactory;

    /**
     * Manages interior spaces.
     */
    class InteriorManager
    {
    private:
      /// Whether this has finished loading all interiors.
      bool finished;
      /// The interiors.
      csWeakRefArray<Interior> interiors;
      /// The interior factories.
      csWeakRefArray<InteriorFactory> interiorFactories;
      /// The names of interiors waiting to be loaded.
      csArray<std::string> toLoad;

    private:
      /// The game world.
      WorldManager* world;
      /// The object registry.
      iObjectRegistry* object_reg;

    private:
      /**
       * Check the status and process the resources each frame.
       */
      bool CheckResources();

      struct FrameCallBack
        : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
      {
        InteriorManager* intmgr;
        FrameCallBack (InteriorManager* interiorManager)
          : scfImplementationType (this) { intmgr = interiorManager; }
        virtual void StartFrame (iEngine* engine, iRenderView* rview);
      };
      friend struct FrameCallBack;
      /// Callback from the engine to check progress.
      csRef<FrameCallBack> cb;

    public:
      /**
       * Constructor.
       * @param world The game world.
       */
      InteriorManager(WorldManager* world);

      /// Destructor.
      ~InteriorManager();

      /**
       * Get an instance of an interior, create a new factory if needed.
       * @param node The document node of the interior.
       * @return The interior.
       */
      csRef<Interior> Get(iDocumentNode* node);

      /**
       * Registers a interior to be loaded the moment it is created.
       * @param interiorName The name of the interior.
       */
      void RegisterLoad(const std::string& interiorName);

      /**
       * Check if the interior manager has finished loading all interiors
       * requested.
       * @return True if finished, false otherwise.
       */
      bool IsReady() const { return finished; }

    };

  } // World namespace
} // PT namespace

#endif // INTERIORMANAGER_H
