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

#ifndef INTERIORMANAGER_H
#define INTERIORMANAGER_H

#include <cssysdef.h>
#include <csutil/weakrefarr.h>
#include <iengine/engine.h>

#include <string>

struct iObjectRegistry;
struct iDocumentNode;

namespace PT
{
  struct Interior;
  struct InteriorFactory;
  class World;

  class InteriorManager 
  {
  private:
    bool finished;
    csWeakRefArray<Interior> interiors;
    csWeakRefArray<InteriorFactory> interiorFactories;
    csArray<std::string> toLoad;

  private:
    World* world;
    iObjectRegistry* object_reg;

  private:
    bool CheckResources();

    // Check the status and process the resources each frame.
    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      InteriorManager* intmgr;
      FrameCallBack (InteriorManager* interiorManager) : scfImplementationType (this) { intmgr = interiorManager; }
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;

  public:
    InteriorManager(World* world);
    ~InteriorManager();

    /// Get or create a new Factory. 
    csRef<Interior> Get(iDocumentNode* node);

    /// Registers a interior to be loaded the moment it is created. 
    void RegisterLoad(const std::string& interiorName);

    /// @return True if finished loading, false otherwise.
    bool IsReady() const { return finished; }

  };

} // PT namespace

#endif // INTERIORMANAGER_H
