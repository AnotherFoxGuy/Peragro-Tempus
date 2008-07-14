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

#include "level.h"
#include "world.h"
#include "modelmanager.h"
#include "factory.h"
#include "interiormanager.h"
#include "interior.h"
#include "levelloader/levelloader.h"

namespace PT
{
  namespace World
  {
    Level::Level(WorldManager* world)
    {
      this->object_reg = world->GetObjectRegistry();
      modelManager =  world->GetModelManager();
      interiorManager = world->GetInteriorManager();

      levelLoader = 0;
      finished = false;

      missingData.AttachNew(new MissingData(this));
    } // end Level()

    Level::~Level()
    {
      //printf("Unloading Level %s\n", collectionName.c_str());

      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

      // Remove callback.
      if (cb.IsValid())
      {
        engine->RemoveEngineFrameCallback(cb);
      }

      // Stab Xordan for this!!
      engine->RemoveCollection(collectionName.c_str());

      // Unload factories.
      // Just empty the array to decrease ref.
      factories.Empty();

      // Delete the tileloader.
      if (levelLoader) delete levelLoader;
      levelLoader = 0;
    } // end ~Level()

    void Level::OpenFile(const std::string& path, const std::string& fileName)
    {
      // Create the threaded fileloader.
      levelLoader = new LevelLoader(object_reg);
      levelLoader->Load(path, fileName);

      csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

      // Create instances' collection.
      instances = engine->CreateCollection(collectionName.c_str());

      // Wait for the resources to be loaded.
      cb.AttachNew(new FrameCallBack(this));
      engine->AddEngineFrameCallback(cb);

    } // end OpenFile()

    void Level::ParseFactories()
    {
      csArray<std::string>* facts = levelLoader->GetFactories();
      if (!facts) return;

      // For each library tag.
      for (size_t i =0; i < facts->GetSize(); i++)
      {
        csRef<Factory> fact = modelManager->Get(facts->Get(i));
        factories.Push(fact);
      }

    } // end ParseFactories()

    void Level::ParseInstances()
    {
      csRefArray<iDocumentNode>* insts = levelLoader->GetInstances();
      if (!insts) return;

      // For each meshobj tag.
      for (size_t i =0; i < insts->GetSize(); i++)
      {
        instanceNodes.Push(insts->Get(i));
      }

    } // end ParseInstances()

    void Level::ParseInteriors()
    {
      csRefArray<iDocumentNode>* ints = levelLoader->GetInteriors();
      if (!ints) return;

      // For each interior tag.
      for (size_t i =0; i < ints->GetSize(); i++)
      {
        csRef<Interior> interior = interiorManager->Get(ints->Get(i));
        interiors.Push(interior);
        // TODO: just load it for now.
        interior->Load();
      }

    } // end ParseInteriors()

    bool Level::CheckResources()
    {
      /*
      static int counter = 0;
      counter++;
      if (counter < 2) return false;
      counter = 0;
      */

      if (levelLoader && !levelLoader->IsReady())
        return false;
      else if (levelLoader)
      {
        ParseFactories();
        ParseInstances();
        ParseInteriors();
        delete levelLoader;
        levelLoader = 0;
      }

      // Wait till all loaders have finished.
      for (size_t i = 0; i < factories.GetSize(); i++)
      {
        Factory* fact = factories.Get(i);
        if (fact->IsReady() && !fact->IsAdded())
        {
          fact->AddToEngine();
          return false;
        }
        else if (!fact->IsReady())
          return false;
      }

      // Wait till everything is precached.
      for (size_t i = 0; i < factories.GetSize(); i++)
      {
        Factory* fact = factories.Get(i);
        if (!fact->IsPrecached())
        {
          fact->Precache();
          return false;
        }
      }

      while (!instanceNodes.IsEmpty())
      {
        csRef<iDocumentNode> node = instanceNodes.Get(0);
        instanceNodes.DeleteIndex(0);
        LoadInstance(node);
        return false;
      }

      SetReady();

      return true;

    } // end CheckResources()

    void Level::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
    {
      if (!level || level->CheckResources())
      {
        engine->RemoveEngineFrameCallback(this);
      }
    } // end StartFrame()

    iMeshFactoryWrapper* Level::MissingData::MissingFactory(const char* name)
    {
      if (!level) return 0;

      for (size_t i = 0; i < level->factories.GetSize(); i++)
      {
        Factory* fact = level->factories.Get(i);
        if (fact->IsReady() && fact->IsAdded())
        {
          iCollection* coll = fact->GetCollection();
          if (coll)
          {
            iMeshFactoryWrapper* mesh = coll->FindMeshFactory(name);
            if (mesh) return mesh;
          }
        }
      }

      return 0;
    } // end MissingFactory()

    iTextureWrapper* Level::MissingData::MissingTexture(const char* name,
                                                        const char* filename)
    {
      if (!level) return 0;

      for (size_t i = 0; i < level->factories.GetSize(); i++)
      {
        Factory* fact = level->factories.Get(i);
        if (fact->IsReady() && fact->IsAdded())
        {
          iCollection* coll = fact->GetCollection();
          if (coll)
          {
            iTextureWrapper* tex = coll->FindTexture(name);
            if (tex) return tex;
          }
        }
      }

      return 0;
    } // end MissingTexture()

  } // World namespace
} // PT namespace

