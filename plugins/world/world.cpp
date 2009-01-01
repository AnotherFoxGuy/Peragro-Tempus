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

#include "world.h"

#include <iengine/rview.h>
#include <iengine/camera.h>
#include <iengine/scenenode.h>
#include <csgeom/transfrm.h>
#include <ivaria/reporter.h>
#include <iutil/vfs.h>
#include <iutil/cfgmgr.h>
#include <iutil/object.h>
#include <iutil/stringarray.h>

#include <iengine/mesh.h>

#include <csutil/scfstringarray.h>
#include <iutil/document.h>
#include <imesh/objmodel.h>
#include <imesh/object.h>
#include <ivaria/collider.h>
#include <cstool/collider.h>

#include "common/world/world.h"

#include "include/cacheentry.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (WorldManager)

//----------------------------------------------------------
WorldManager::Instance::Instance (const Common::World::Object& object, 
                                  iObjectRegistry* obj_reg) 
                                  : Common::World::Object(object), 
                                  object_reg(obj_reg), iCacheUser(obj_reg)
{
  Load(factoryFile);
}

WorldManager::Instance::~Instance()
{
  ///*
  // TODO: fade instead of just removing.
  if (instance) 
  {
    // Effect is only ref holder, don't have to do anything.
    if (instance->GetRefCount() == 1) return; 

    // Remove from any collections.
    if (instance->QueryObject())
      if (instance->QueryObject()->GetObjectParent())
        instance->QueryObject()->GetObjectParent()->ObjRemove(instance->QueryObject());

    // Remove from sectors and any parent meshes.
    if (instance->GetMovable())
    {
      instance->GetMovable()->ClearSectors();
      instance->GetMovable()->GetSceneNode()->SetParent(0);
      instance->GetMovable()->UpdateMove();
    }
  }
  //*/
  printf("REMOVED: %d %s\n", id, name.c_str());
}

void WorldManager::Instance::Loaded(iCacheEntry* cacheEntry)
{
  if (cacheEntry->WasSuccessful())
  {
    instance = cacheEntry->Create(name, factoryName);

    csVector3 curpos = instance->QuerySceneNode()->GetMovable()->GetFullPosition();
    instance->QuerySceneNode()->GetMovable()->SetPosition(curpos + (csVector3)position);

    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iSector* s = engine->FindSector(sector.c_str());
    if (s) instance->QuerySceneNode()->GetMovable()->SetSector(s);
    else csReport(object_reg, CS_REPORTER_SEVERITY_ERROR, "peragro.world", 
      "Failed to get sector '%s' (object: '%s' has errors)!", sector.c_str(), name.c_str());
    instance->QuerySceneNode()->GetMovable()->UpdateMove();

    csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem> (object_reg);
    csColliderHelper::InitializeCollisionWrapper (cdsys, instance);
  }
  else
    csReport(object_reg, CS_REPORTER_SEVERITY_ERROR, "peragro.world", 
      "Failed to create mesh '%s' (factory: '%s' has errors)!", name.c_str(), factoryFile.c_str());
} // end Loaded()

//----------------------------------------------------------

void WorldManager::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.world", msg, arg);
  va_end (arg);
}

WorldManager::WorldManager(iBase* iParent)
  : scfImplementationType(this, iParent), object_reg(0), worldManager(0)
{
  loading = false;
  camera.Set(0.0f);
  radius = 30;

  worldManager = new Common::World::WorldManager();

} // end World() :P

bool WorldManager::Initialize(iObjectRegistry* obj_reg)
{
  object_reg = obj_reg;

  return true;
}

bool EndsWith(const csString& file, const csString& extension)
{
  csString ext;
  file.SubString(ext, file.Length()-extension.Length());
  //printf("- %s\n", ext.GetData());
  return ext.CompareNoCase(extension);
}

std::map<std::string, Geom::Box> FindMeshFacts(iObjectRegistry* obj_reg, const csString& file)
{
  std::map<std::string, Geom::Box> meshFacts;

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (obj_reg);
  csRef<iDataBuffer> xmlfile = vfs->ReadFile (file.GetData());
  csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (obj_reg));
  csRef<iDocument> doc (docsys->CreateDocument());
  const char* error = doc->Parse (xmlfile, true);
  if (error) return meshFacts;
  csRef<iDocumentNode> xml = doc->GetRoot ();
  if (!xml) return meshFacts;
  xml = xml->GetNode ("library");
  if (!xml) return meshFacts;
  csRef<iDocumentNodeIterator> nodes = xml->GetNodes("meshfact");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> node = nodes->Next();
    std::string name = node->GetAttributeValue("name");
    meshFacts[name] = Geom::Box();
  }
  return meshFacts;
}

void AddFactories(iObjectRegistry* obj_reg, Common::World::WorldManager* worldManager, const char* path)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (obj_reg);
  csRef<iStringArray> paths = vfs->FindFiles(path);
  for (size_t i = 0; i < paths->GetSize(); i++ )
  {
    csString path = paths->Get(i);   
    if (EndsWith(path, "/")) // It's a directory
      if (EndsWith(path, ".svn/")) // Ignore;
        continue;
      else
        AddFactories(obj_reg, worldManager, path.GetData());
    else
      if (1 /*EndsWith(path, ".xml")*/)
      {
        std::map<std::string, Geom::Box> facts = FindMeshFacts(obj_reg, path);
        //printf("%s (%d)\n", path.GetData(), facts.size());
        std::map<std::string, Geom::Box>::iterator it;
        for (it = facts.begin(); it != facts.end(); it++ )
        {
          using namespace Common::World;
          Factory factory;
          factory.factoryName = (*it).first;
          factory.factoryFile = path.GetData();
          factory.boundingBox = (*it).second;

          //printf(" - %s\n", factory.factoryName.c_str());

          //-------------------------------------------------
          csRef<iEngine> engine = csQueryRegistry<iEngine> (obj_reg);
          csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (obj_reg);
          csRef<iCollection> coll = engine->CreateCollection("worldmanager.BBcalc");
          csRef<iThreadReturn> tr = loader->LoadLibraryFile(factory.factoryFile.c_str(), coll);
          tr->Wait();
          engine->SyncEngineListsNow(loader);
          csRef<iMeshFactoryWrapper> mfw = coll->FindMeshFactory(factory.factoryName.c_str());
          if (mfw)
            if (mfw->GetMeshObjectFactory())
              if (mfw->GetMeshObjectFactory()->GetObjectModel())
              {
                csRef<iObjectModel> obj = mfw->GetMeshObjectFactory()->GetObjectModel();
                csBox3 box = obj->GetObjectBoundingBox();
                factory.boundingBox = Geom::Box(box.Min(), box.Max());
              }
          coll->ReleaseAllObjects();
          //-------------------------------------------------

          if (factory.boundingBox.Empty())
            printf("E: invalid boundingbox for %s (%s)\n", factory.factoryName.c_str(), factory.factoryFile.c_str());
          else
            worldManager->Add(factory);
        }
      }
  }
} // end AddFactories()

bool WorldManager::Initialize(const std::string& name)
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "Loading world %s", basename.c_str());

  basename = name;

  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  if (!loader) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate Loader!");
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate VFS!");

  vfs->SetSyncDir("/peragro/art/world/");
  csRef<iThreadReturn> tr = loader->LoadMapFile("world", false);
  tr->Wait();

  UpdateOptions();

  eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;

  nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
  if (!nameRegistry) return false;

  loadingId = nameRegistry->GetID("world.loading");
  loadedId = nameRegistry->GetID("world.loaded");

  // Register an event for UpdateOptions.
  eventQueue->RegisterListener (this, nameRegistry->GetID("interface.options.video"));

  // TODO test data, remove.
  //------------------------------------------------------------
  printf("================================================\n");
  //AddFactories(object_reg, worldManager, "/peragro/art/3d_art/");
  printf("================================================\n");

   {
    Common::World::Object object;
    object.id = 0;
    object.name = "terrain";
    object.factoryFile = "/peragro/art/3d_art/buildings/island/factories/0,0";
    object.factoryName = "0,0";
    object.position = Geom::Vector3(300, 0, 300);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
  {
    Common::World::Object object;
    object.id = 1;
    object.name = "test1";
    object.factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object.factoryName = "genscythe001";
    object.position = Geom::Vector3(642, 0, 371);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
  {
    Common::World::Object object;
    object.id = 2;
    object.name = "test2";
    object.factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object.factoryName = "genscythe001";
    object.position = Geom::Vector3(642, 0, 376);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
 

  //------------------------------------------------------------

  return true;
} // end Initialize

WorldManager::~WorldManager()
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "Unloading world %s", basename.c_str());

  delete worldManager;

} // end ~World()

bool WorldManager::UpdateOptions()
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (object_reg);

  if (app_cfg)
    radius = (size_t)app_cfg->GetInt("Peragro.World.ProximityRange", (int)radius);

  return true;
} // end UpdateOptions()

void WorldManager::SetCamera(iCamera* camera)
{
  UnSetCamera();

  if (!camera) return;

  cb.AttachNew(new MovableCallBack(this));

  camera->AddCameraListener(cb);

  // TODO
  csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadedId);
  eventQueue->Post(worldEvent);

} // end SetCamera()

void WorldManager::UnSetCamera()
{
  if (cb.IsValid())
    cb.Invalidate();
} // end UnSetCamera()

static int Compare(const Common::World::Object* r1, const Common::World::Object* r2)
{
  if (r1->id < r2->id) return -1;
  else if (r2->id < r1->id) return 1;
  else return 0;
}

template <typename T, typename K>
int ptCompare(T const& r1, K const& r2)
{
  return Compare(r1, r2);
}

void WorldManager::CameraMoved()
{
  using namespace Common::World;
  Octree::QueryResult objects = worldManager->Query(Geom::Sphere(camera, radius));

  csRefArray<Instance> newInstances;
  Octree::QueryResult::iterator it;
  if (objects.size()) printf("QUERY: %d (rad: %f)\n", objects.size(), (float)radius);
  for (it = objects.begin() ; it != objects.end(); it++ )
  {
    //printf("================================================\n");
    //printf("OBJECT: %d %s %s\n", (*it).id, (*it).name.c_str(), (*it).factoryFile.c_str());
    //printf("================================================\n");

    size_t index = instances.FindSortedKey(csArrayCmp<Instance*, const Object*>(&(*it), ptCompare));
    if (index == csArrayItemNotFound)
      index = cachedInstances.FindSortedKey(csArrayCmp<Instance*, const Object*>(&(*it), ptCompare));
    
    if (index != csArrayItemNotFound)
    {
      //printf("FOUND: %d %s\n", (*it).id, (*it).name.c_str());
      // Instance was found, copy it to the new array.
      newInstances.InsertSorted(instances.Get(index), ptCompare);
      instances.DeleteIndex(index);
    }
    else
    {
      printf("OBJECT: %d %s\n", (*it).id, (*it).name.c_str());
      // Instance was not found, make a new instance.
      csRef<Instance> in; in.AttachNew(new Instance(*it, object_reg));
      newInstances.InsertSorted(in, ptCompare);
    }
  }

  cachedInstances = instances;

  // Replace array with the new array.
  instances = newInstances;

} // end CameraMoved()

void WorldManager::MovableCallBack::CameraMoved(iCamera* camera)
{
  if (!world)
  {
    camera->RemoveCameraListener(this);
    return;
  }

  world->camera = camera->GetTransform().GetOrigin();

  world->CameraMoved();

} // end CameraMoved()
