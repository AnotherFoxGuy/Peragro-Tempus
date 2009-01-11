/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include <cstool/enginetools.h>
#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/prop.h>
#include <propclass/defcam.h>

#include "common/world/world.h"

#include "include/cacheentry.h"

#include "resourcemanager.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (WorldManager)

//----------------------------------------------------------
WorldManager::Instance::Instance (const Common::World::Object& object,
                                  iObjectRegistry* obj_reg)
                                  : Common::World::Object(object),
                                  iCacheUser(obj_reg), object_reg(obj_reg)
{
  Load(factoryFile);
}

WorldManager::Instance::~Instance()
{
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
  printf("REMOVED: %zd %s\n", id, name.c_str());
}

void WorldManager::Instance::Loaded(iCacheEntry* cacheEntry)
{
  if (cacheEntry->WasSuccessful())
  {
    instance = cacheEntry->Create(name, factoryName);

    csVector3 curpos = instance->QuerySceneNode()->GetMovable()->GetFullPosition();
    instance->QuerySceneNode()->GetMovable()->SetPosition(curpos + (csVector3&)position);

    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iSector* s = engine->FindSector(sector.c_str());
    if (s) instance->QuerySceneNode()->GetMovable()->SetSector(s);
    else csReport(object_reg, CS_REPORTER_SEVERITY_ERROR, "peragro.world",
      "Failed to get sector '%s' (object: '%s' has errors)!", sector.c_str(), name.c_str());
    instance->QuerySceneNode()->GetMovable()->UpdateMove();

    csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem> (object_reg);
    csColliderHelper::InitializeCollisionWrapper (cdsys, instance);
    csRef<EditorObject> edObj; edObj.AttachNew(new EditorObject(*this, object_reg, instance));
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
  : scfImplementationType(this, iParent), worldManager(0), object_reg(0)
{
  loading = false;
  position.Set(0.0f);
  radius = 30;

  worldManager = new Common::World::WorldManager();

} // end World() :P

bool WorldManager::Initialize(iObjectRegistry* obj_reg)
{
  object_reg = obj_reg;

  return true;
}

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

  ResourceManager resourceManager(object_reg, worldManager);
  resourceManager.Initialize();
  resourceManager.ScanFactories("/peragro/art/3d_art/");
  resourceManager.AddTestObjects();
  resourceManager.ScanObjects("/peragro/art/tiles/");

  // Editor.
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object+X"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object-X"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object+Y"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object-Y"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object+Z"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Object-Z"));
  eventQueue->RegisterListener (this, nameRegistry->GetID("input.Interact"));

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

bool WorldManager::Loading()
{
  if (loading)
  {
    bool allLoaded = true;
    float itemsLoaded = 0.0f;
    for (size_t i = 0; i < instances.GetSize(); i++)
    {
      if ( !instances.Get(i)->IsReady() )
        allLoaded = false;
      else
        itemsLoaded += 1;
    }

    csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadingId);
    worldEvent->Add("progress",
      itemsLoaded/static_cast<float>(instances.GetSize()));
    eventQueue->Post(worldEvent);

    if (allLoaded)
    {
      Report(CS_REPORTER_SEVERITY_NOTIFY, "World loaded!");
      csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadedId);
      eventQueue->Post(worldEvent);
      loading = false;
      eventQueue->Unsubscribe(this, nameRegistry->GetID("crystalspace.frame"));
    }
  }

  return true;
} // end Loading()

iCamera* GetCamera(iObjectRegistry* object_reg, iMeshWrapper* wrap)
{
  csRef<iCelPlLayer> pl = csQueryRegistry<iCelPlLayer> (object_reg);
  if (pl && wrap)
  {
    iCelEntity* entity = pl->FindAttachedEntity(wrap->QueryObject());
    if (entity)
    {
      csRef<iPcDefaultCamera> camera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
      if (camera) return camera->GetCamera();
    }
  }

  return 0;
}

void Move(iMeshWrapper* selectedMesh, iCamera* cam, const csVector3& v)
{
  if (selectedMesh && cam)
  {
    csReversibleTransform& tr = selectedMesh->GetMovable()->GetTransform();
    tr.Translate (cam->GetTransform().This2OtherRelative(v));
    selectedMesh->GetMovable()->UpdateMove();
  }
}

bool WorldManager::HandleEvent(iEvent& ev)
{
  float f = 0.1f;
  if (ev.GetName() == nameRegistry->GetID("crystalspace.frame"))
    Loading();
  else if (ev.GetName() == nameRegistry->GetID("interface.options.video"))
    UpdateOptions();
  else if (ev.GetName() == nameRegistry->GetID("input.Object+X"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(f, 0, 0));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Object-X"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(-f, 0, 0));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Object+Y"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(0, f, 0));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Object-Y"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(0, -f, 0));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Object+Z"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(0, 0, f));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Object-Z"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    Move(selectedMesh, cam, csVector3(0, 0, -f));
  }
  else if (ev.GetName() == nameRegistry->GetID("input.Interact"))
  {
    iCamera* cam = GetCamera(object_reg, playerMesh);
    if (cam)
    {
      int x, y; x = y = 0;
      ev.Retrieve("X", x);
      ev.Retrieve("Y", y);
      csVector2 p(x, y);
      csScreenTargetResult st = csEngineTools::FindScreenTarget (p, 100.0f, cam);
      selectedMesh = st.mesh;
      if (selectedMesh)
        Report (CS_REPORTER_SEVERITY_NOTIFY, "Selected mesh %s", selectedMesh->QueryObject()->GetName());
    }
  }

  return true;
} // end HandleEvent()

void WorldManager::EnterWorld(Geom::Vector3 position)
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "World loading...");

  this->position = position;
  CameraMoved();

  csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadingId);
  worldEvent->Add("progress", 0.0f);
  eventQueue->Post(worldEvent);

  // Register an event for Loading().
  eventQueue->RegisterListener (this, nameRegistry->GetID("crystalspace.frame"));

  loading = true;
}

void WorldManager::SetMesh(iMeshWrapper* wrapper)
{
  UnSetMesh();

  if (!wrapper) return;

  playerMesh = wrapper;

  cb.AttachNew(new MovableCallBack(this));

  wrapper->GetMovable()->AddListener(cb);
} // end SetCamera()

void WorldManager::UnSetMesh()
{
  if (cb.IsValid())
    cb.Invalidate();

  playerMesh = 0;
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
  Octree::QueryResult objects = worldManager->Query(Geom::Sphere(position, radius));

  csRefArray<Instance> newInstances;
  Octree::QueryResult::iterator it;
  if (objects.size()) printf("QUERY: %zd (rad: %f at %s)\n", objects.size(), (float)radius, position.Description().GetData());
  for (it = objects.begin(); it != objects.end(); it++ )
  {
    size_t index = instances.FindSortedKey(csArrayCmp<Instance*, const Object*>(&(*it), ptCompare));
    if (index != csArrayItemNotFound)
    {
      //printf("FOUND: %d %s\n", (*it).id, (*it).name.c_str());
      // Instance was found, copy it to the new array.
      newInstances.InsertSorted(instances.Get(index), ptCompare);
    }
    else
    {
      printf("OBJECT: %lu %s\n", (*it).id, (*it).name.c_str());
      // Instance was not found, make a new instance.
      csRef<Instance> in; in.AttachNew(new Instance(*it, object_reg));
      newInstances.InsertSorted(in, ptCompare);
    }
  }

  // Replace array with the new array.
  instances = newInstances;

} // end CameraMoved()

void WorldManager::MovableCallBack::MovableChanged (iMovable* movable)
{
  if (!world)
  {
    movable->RemoveListener(this);
    return;
  }

  world->position = movable->GetTransform().GetOrigin();

  world->CameraMoved();

} // end CameraMoved()

void WorldManager::CommitChanges(Common::World::Object& object)
{
  printf("CommitChanges: %s\n", object.name.c_str());
  worldManager->AddLookUp(object, false);
} // end CommitChanges()
