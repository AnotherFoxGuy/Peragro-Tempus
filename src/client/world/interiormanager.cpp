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

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/weakrefarr.h>
#include <csutil/scf_implementation.h>
#include <iutil/objreg.h>
#include <iutil/document.h>

#include "interiormanager.h"
#include "interior.h"
#include "world.h"

namespace PT
{
  InteriorManager::InteriorManager(World* world)
  {
    this->world = world;
    this->object_reg = world->GetObjectRegistry();
  }

  InteriorManager::~InteriorManager()
  {
    // Remove callback.
    if (cb.IsValid())
    {
      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
      engine->RemoveEngineFrameCallback(cb);
    }
  }

  csRef<Interior> InteriorManager::Get(iDocumentNode* node)
  {
    csRef<iDocumentNode> intNode = node->GetNode("interior");
    std::string interiorName = intNode->GetAttributeValue("name");
    std::string fileName = intNode->GetAttributeValue("file");

    // Search for our instance.
    interiors.Compact();
    for (size_t i = 0; i < interiors.GetSize(); i++)
    {
      csRef<Interior> current = csRef<Interior>(interiors.Get(i));
      if (current->interiorName == interiorName)
      {
        return current;
      }
    }

    // We haven't found it, search for the interior factory.
    interiorFactories.Compact();
    csRef<InteriorFactory> fact;
    for (size_t i = 0; i < interiorFactories.GetSize(); i++)
    {
      fact = csRef<InteriorFactory>(interiorFactories.Get(i));
      if (fact->fileName == fileName)
        break;
    }

    // Not found: Create a new factory.
    if (!fact.IsValid())
    {
      fact.AttachNew(new InteriorFactory(fileName, world));
    }
    interiorFactories.Push(fact);

    csRef<Interior> interior; interior.AttachNew(new Interior(node, fact));
    interiors.Push(interior);

    return interior;
  } // end Get()

  void InteriorManager::RegisterLoad(const std::string& interiorName)
  {
    finished = false;

    toLoad.Push(interiorName);

    // Wait for the resources to be loaded.
    if (!cb.IsValid())
    {
      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
      cb.AttachNew(new FrameCallBack(this));
      engine->AddEngineFrameCallback(cb);
    }
  }

  bool InteriorManager::CheckResources()
  {
    // Wait till registered interiors have finished loading.
    for (size_t i = 0; i < toLoad.GetSize(); i++)
    {
      std::string interiorName = toLoad.Get(i);

      // Search.
      csRef<Interior> current;
      interiors.Compact();
      for (size_t j = 0; j < interiors.GetSize(); j++)
      {
        current = csRef<Interior>(interiors.Get(j));
        if (current->interiorName == interiorName)
          break;
      }

      if (current.IsValid())
      {
        current->Load();
        toLoad.DeleteIndexFast(i);
      }
    }

    finished = true;

    return true;

  } // end CheckResources()

  void InteriorManager::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
  {
    if (!intmgr || intmgr->CheckResources())
    {
      engine->RemoveEngineFrameCallback(this);
    }
  } // end StartFrame()

} // PT namespace
