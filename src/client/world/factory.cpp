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
#include <iengine/texture.h>
#include <csutil/scf_implementation.h>
#include <iengine/collection.h>

#include "factory.h"
#include "factoryloader/fileloader.h"

#include "client/reporter/reporter.h"

namespace PT
{
  Factory::Factory(const std::string& fileName, 
    iObjectRegistry* object_reg) : scfImplementationType (this)
  {
    this->fileName = fileName;
    this->object_reg = object_reg;
    fileLoader = 0;
    isPrecached = false;
    isAdded = false;
  }

  Factory::~Factory()
  {
    if (fileLoader)
    {
      delete fileLoader;
      fileLoader = 0;
    }
  }

  void Factory::Load()
  {
    fileLoader = new FileLoader(object_reg);

    // Seperate the path of the filename.
    size_t p = fileName.find_last_of("/");
    std::string path = fileName.substr(0,p+1);
    std::string file = fileName.substr(p+1,fileName.length());

    // Create our region.
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    collection = engine->CreateCollection(fileName.c_str());

    fileLoader->Load(path, file, collection);
  } // end Load()

  bool Factory::IsReady() const
  {
    if (fileLoader) 
      return fileLoader->IsReady();
    else
      return true;
  } // end IsReady()

  void Factory::AddToEngine()
  {
    if (!IsReady()) return;

    fileLoader->AddToEngine();
    delete fileLoader;
    fileLoader = 0;
    isAdded = true;
  } // end AddToEngine()

  void Factory::Precache()
  {
    if (!IsReady()) return;
    if (!collection.IsValid()) return;
    if (IsPrecached()) return;

    // Precaches "one" texture and then returns, so you have to call 
    // this repeately and check IsPrecached() wether 'everything' is done.
    csRef<iObjectIterator> iter = collection->QueryObject()->GetIterator();
    while (iter->HasNext ())
    {
      csRef<iTextureWrapper> csth (scfQueryInterface<iTextureWrapper> (iter->Next ()));
      if (csth)
      {
        if (csth->GetTextureHandle ()) 
        {
          if (csth->GetTextureHandle ()->IsPrecached())
            continue;
          else
          {
            csth->GetTextureHandle()->Precache();
            Report(PT::Debug, "Precached texture '%s'", csth->QueryObject()->GetName());
            return;
          }
        }
      }
    }

    isPrecached = true;
  } // end Precache()

} // PT namespace
