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

#include "effect.h"


#include <iengine/collection.h>
#include <imap/loader.h>
#include <iengine/engine.h>
#include <iengine/texture.h>
#include <iutil/object.h>

#include "client/pointer/pointer.h"
#include "client/reporter/reporter.h"

namespace PT
{
  namespace Effect
  {
    Factory::Factory(const std::string& fileName, 
      iObjectRegistry* object_reg) : scfImplementationType (this)
    {
      this->fileName = fileName;
      this->object_reg = object_reg;
    }

    Factory::~Factory()
    {
    }

    void Factory::Load()
    {
      // Create our collection.
      csRef<iLoader> loader =  csQueryRegistry<iLoader> (object_reg);
      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
      collection = engine->CreateCollection(fileName.c_str());

      // load the factory for the effect from file
      if ( !loader->LoadLibraryFile (fileName.c_str(), collection) )  
      {
        Report(PT::Error, "Effect::Factory: Couldn't load effect file: ' %s ' !", 
          fileName.c_str());
      }

    } // end Load()

    //--------------------------------------------------------------

    Effect::Effect (Factory* factory, Data::Effect* eff)
    {
      Effect::factory = factory;
      Effect::timeleft = eff->GetDuration();
      Effect::meshName = eff->GetMeshName();
      Effect::offset = eff->GetOffset();
    }

    Effect::~Effect()
    {
      if (mesh)
      {
        csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->GetObjectRegistry());
        engine->RemoveObject (mesh);
      }
    }

    void Effect::Load()
    {
      csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->GetObjectRegistry());

      // Find the factory and turn it into a factorywrapper.
      csRef<iMeshFactoryWrapper> effectfmw = engine->FindMeshFactory(meshName.c_str(), factory->GetCollection());
      if (!effectfmw)
      {
        Report(PT::Error, "Effect: Couldn't find effect factory: ' %s ' !", meshName.c_str());
        return;
      }

      // Create the effect object.
      csVector3 offset(offset.x, offset.y, offset.z);
      mesh = engine->CreateMeshWrapper(effectfmw, "effect", 0, offset);
    } // end Load()

    bool Effect::Handle (csTicks elapsed_ticks)
    {
      timeleft -= elapsed_ticks;
      if (timeleft <= 0)
      {
        return false;
      }
      return true;
    }

  } // Data namespace 
} // PT namespace 
