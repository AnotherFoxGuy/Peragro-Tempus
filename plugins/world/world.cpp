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

#include <wfmath/stream.h>

#include <iengine/scenenode.h>
#include <ivaria/reporter.h>
#include <iutil/vfs.h>
#include <iutil/cfgmgr.h>
#include <iutil/object.h>

#include <iengine/mesh.h>

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

#include "common/util/printhelper.h"
#include "common/util/geomhelper.h"

#include "editorobject.h"

CS_IMPLEMENT_PLUGIN

SCF_IMPLEMENT_FACTORY (WorldManager)

//----------------------------------------------------------
WorldManager::Instance::Instance (Common::World::Objectp object,
                                  iObjectRegistry* obj_reg)
                                  : iCacheUser(obj_reg), object_reg(obj_reg),
                                  _object(object), id(object->id)
{
  Load(_object->factoryFile);
} // end Instance()

WorldManager::Instance::~Instance()
{
  // TODO: fade instead of just removing.
  if (instance)
  {
    // Instance is only ref holder, don't have to do anything.
    if (instance->GetRefCount() == 1) return;

    // Remove from any collections.
    if (instance->QueryObject())
      if (instance->QueryObject()->GetObjectParent())
        instance->QueryObject()->GetObjectParent()->ObjRemove(
          instance->QueryObject());

    // Remove from sectors and any parent meshes.
    if (instance->GetMovable())
    {
      instance->GetMovable()->ClearSectors();
      instance->GetMovable()->GetSceneNode()->SetParent(0);
      instance->GetMovable()->UpdateMove();
    }
  }
  printf("REMOVED: %"SIZET" %s\n", _object->id, _object->name.c_str());
} // end ~Instance()

void WorldManager::Instance::Loaded(iCacheEntry* cacheEntry)
{
  if (cacheEntry->WasSuccessful())
  {
    instance = cacheEntry->Create(_object->name, _object->factoryName);

    // Set the position.
    csVector3 curpos =
      instance->QuerySceneNode()->GetMovable()->GetFullPosition();
    instance->QuerySceneNode()->GetMovable()->
      SetPosition(curpos + VectorHelper::Convert(_object->position));

    // Set the rotation.
    instance->QuerySceneNode()->GetMovable()->
      Transform(MatrixHelper::Convert(_object->rotation));

    // Set the sector.
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iSector* s = engine->FindSector(_object->sector.c_str());
    if (s) instance->QuerySceneNode()->GetMovable()->SetSector(s);
    else csReport(object_reg, CS_REPORTER_SEVERITY_ERROR, "peragro.world",
      "Failed to get sector '%s' (object: '%s' has errors)!",
      _object->sector.c_str(), _object->name.c_str());
    instance->QuerySceneNode()->GetMovable()->UpdateMove();

    // Add collission data.
    csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem> (object_reg);
    csColliderHelper::InitializeCollisionWrapper (cdsys, instance);

    // Add the editor object to track changes.
    csRef<EditorObject> edObj;
    edObj.AttachNew(new EditorObject(_object, object_reg, instance));
  }
  else
  {
    csReport(object_reg, CS_REPORTER_SEVERITY_ERROR, "peragro.world",
      "Failed to create mesh '%s' (factory: '%s' has errors)!",
      _object->name.c_str(), _object->factoryFile.c_str());
  }
} // end Loaded()

//----------------------------------------------------------

void WorldManager::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.world", msg, arg);
  va_end (arg);
} // end Report()

WorldManager::WorldManager(iBase* iParent)
  : scfImplementationType(this, iParent), object_reg(0),
  worldManager(new Common::World::WorldManager()),
  position(0.0f), hasEnteredWorld(false), loading(false), loadRadius(1500), editStepSize(0.1f)
{
} // end World() :P

bool WorldManager::Initialize(iObjectRegistry* obj_reg)
{
  object_reg = obj_reg;

  return true;
} // end Initialize()

bool WorldManager::Initialize(const std::string& name)
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "Loading world %s", basename.c_str());

  basename = name;

  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  if (!loader) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate Loader!");

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate VFS!");

  csRef<iThreadReturn> tr = loader->LoadMapFile("/peragro/art/world/", "world", false);
  tr->Wait();

  UpdateOptions();

  eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;

  nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
  if (!nameRegistry) return false;

  loadingId = nameRegistry->GetID("world.loading");
  loadedId = nameRegistry->GetID("world.loaded");
  updateOptionsId = nameRegistry->GetID("interface.options.view");
  objectPXId = nameRegistry->GetID("input.Object+X");
  objectMXId = nameRegistry->GetID("input.Object-X");
  objectPYId = nameRegistry->GetID("input.Object+Y");
  objectMYId = nameRegistry->GetID("input.Object-Y");
  objectPZId = nameRegistry->GetID("input.Object+Z");
  objectMZId = nameRegistry->GetID("input.Object-Z");
  interactId = nameRegistry->GetID("input.Interact");
  objectStepPId = nameRegistry->GetID("input.ObjectStep+");
  objectStepMId = nameRegistry->GetID("input.ObjectStep-");

  // Register an event for UpdateOptions.
  eventQueue->RegisterListener(this, updateOptionsId);

  ResourceManager resourceManager(object_reg, worldManager.get());
  resourceManager.Initialize();
  if (!worldManager->HasData())
  {
    resourceManager.ScanFactories("/peragro/art/3d_art/");
    resourceManager.AddTestObjects();
    resourceManager.ScanObjects("/peragro/art/tiles/");
  }

  // Editor.
  eventQueue->RegisterListener (this, objectPXId);
  eventQueue->RegisterListener (this, objectMXId);
  eventQueue->RegisterListener (this, objectPYId);
  eventQueue->RegisterListener (this, objectMYId);
  eventQueue->RegisterListener (this, objectPZId);
  eventQueue->RegisterListener (this, objectMZId);
  eventQueue->RegisterListener (this, interactId);
  eventQueue->RegisterListener (this, objectStepPId);
  eventQueue->RegisterListener (this, objectStepMId);

  return true;
} // end Initialize()

WorldManager::~WorldManager()
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "Unloading world %s", basename.c_str());
} // end ~World()

bool WorldManager::UpdateOptions()
{
  csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> (object_reg);

  if (app_cfg)
  {
    loadRadius = static_cast<size_t>(app_cfg->
      GetInt("Peragro.World.LoadRadius", static_cast<int>(loadRadius)));
    // Pretend the camera moved to recheck the load radius.
    if (hasEnteredWorld) CameraMoved();

    editStepSize = app_cfg->GetFloat("Peragro.World.EditStepSize", editStepSize);
  }

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
      if (!instances.Get(i)->IsReady())
      {
        allLoaded = false;
      }
      else
      {
        itemsLoaded += 1;
      }
    }

    if (allLoaded)
    {
      Report(CS_REPORTER_SEVERITY_NOTIFY, "World loaded!");
      csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadedId);
      eventQueue->Post(worldEvent);
      loading = false;
      eventQueue->Unsubscribe(this, nameRegistry->GetID("crystalspace.frame"));
    }
    else
    {
      csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadingId);
      worldEvent->Add("progress",
        itemsLoaded/static_cast<float>(instances.GetSize()));
      eventQueue->Post(worldEvent);
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
      csRef<iPcDefaultCamera> camera =
        CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);

      if (camera) return camera->GetCamera();
    }
  }

  return 0;
} // end GetGamera()

void Move(iMeshWrapper* selectedMesh, iCamera* cam, const csVector3& v)
{
  if (selectedMesh && cam)
  {
    csReversibleTransform& tr = selectedMesh->GetMovable()->GetTransform();
    tr.Translate (cam->GetTransform().This2OtherRelative(v));
    selectedMesh->GetMovable()->UpdateMove();
  }
} // end Move()

void HighLightMesh(iObjectRegistry* object_reg, iMeshWrapper* mesh, bool state)
{
  csRef<iShaderVarStringSet> strings =
    csQueryRegistryTagInterface<iShaderVarStringSet>
    (object_reg, "crystalspace.shader.variablenameset");
  CS::ShaderVarStringID ambient = strings->Request ("light ambient");
  csRef<iShaderVariableContext> svc =
    scfQueryInterfaceSafe<iShaderVariableContext> (mesh);
  if (svc)
  {
    if (state)
    {
      csRef<csShaderVariable> var = svc->GetVariableAdd(ambient);
      var->SetValue(csVector3(2,2,2));
    }
    else
    {
      svc->RemoveVariable(ambient);
    }
  }
} // end HighLightMesh()

bool WorldManager::HandleEvent(iEvent& ev)
{
  if (ev.GetName() == nameRegistry->GetID("crystalspace.frame"))
  {
    Loading();
    return true;
  }
  else if (ev.GetName() == updateOptionsId)
  {
    UpdateOptions();
    return true;
  }

  bool down = false;
  if (ev.Retrieve("buttonState", down) != csEventErrNone)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "Failed to get buttonState from event.");
    return true;
  }

  // For these events, only act on button down.
  if (down)
  {
    if (ev.GetName() == objectPXId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(editStepSize, 0, 0));
    }
    else if (ev.GetName() == objectMXId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(-editStepSize, 0, 0));
    }
    else if (ev.GetName() == objectPYId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(0, editStepSize, 0));
    }
    else if (ev.GetName() == objectMYId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(0, -editStepSize, 0));
    }
    else if (ev.GetName() == objectPZId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(0, 0, editStepSize));
    }
    else if (ev.GetName() == objectMZId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      Move(selectedMesh, cam, csVector3(0, 0, -editStepSize));
    }
    else if (ev.GetName() == interactId)
    {
      iCamera* cam = GetCamera(object_reg, playerMesh);
      if (cam)
      {
        int x, y = 0;
        ev.Retrieve("X", x);
        ev.Retrieve("Y", y);
        csVector2 p(x, y);

        csScreenTargetResult st =
          csEngineTools::FindScreenTarget (p, 100.0f, cam);

        // Unhighlight the previous mesh.
        HighLightMesh(object_reg, selectedMesh, false);

        // Highlight the new mesh.
        selectedMesh = st.mesh;
        HighLightMesh(object_reg, selectedMesh, true);

        if (selectedMesh) Report(CS_REPORTER_SEVERITY_NOTIFY,
          "Selected mesh %s", selectedMesh->QueryObject()->GetName());
      }
    }
    else if (ev.GetName() == objectStepPId)
    {
      editStepSize *= 10.0f;
      Report(CS_REPORTER_SEVERITY_NOTIFY, "New edit step size: %f",
        editStepSize);
    }
    else if (ev.GetName() == objectStepMId)
    {
      editStepSize /= 10.0f;
      Report(CS_REPORTER_SEVERITY_NOTIFY, "New edit step size: %f",
        editStepSize);
    }
  }

  return true;
} // end HandleEvent()

void WorldManager::EnterWorld(WFMath::Point<3> position)
{
  Report(CS_REPORTER_SEVERITY_NOTIFY, "World loading...");

  this->position = position;
  CameraMoved();

  csRef<iEvent> worldEvent = eventQueue->CreateBroadcastEvent(loadingId);
  worldEvent->Add("progress", 0.0f);
  eventQueue->Post(worldEvent);

  // Register an event for Loading().
  eventQueue->RegisterListener(this, nameRegistry->GetID("crystalspace.frame"));

  loading = true;
  hasEnteredWorld = true;
} // end EnterWorld()

void WorldManager::SetMesh(iMeshWrapper* wrapper)
{
  UnSetMesh();

  if (!wrapper) return;

  playerMesh = wrapper;

  cb.AttachNew(new MovableCallBack(this));

  wrapper->GetMovable()->AddListener(cb);
} // end SetMesh()

void WorldManager::UnSetMesh()
{
  if (cb.IsValid())
    cb.Invalidate();

  playerMesh = 0;
} // end UnSetMesh()

template <typename T, typename K>
int ptCompare(T const& r1, K const& r2)
{
  return csComparator<size_t, size_t>::Compare(r1->id, r2->id);
} // end ptCompare()

void WorldManager::CameraMoved()
{
  using namespace Common::World;
  std::list<Objectp> objects =
    worldManager->Query(WFMath::Ball<3>(position, loadRadius));
/*
  if (objects.size()) printf("QUERY: %"SIZET" (rad: %f at %s)\n",
    objects.size(), (float)loadRadius, WFMath::ToString(position).c_str());
*/
  csRefArray<Instance> newInstances;
  std::list<Objectp>::iterator it;
  for (it = objects.begin(); it != objects.end(); it++ )
  {
    size_t index = instances.FindSortedKey(
      csArrayCmp<Instance*, Object*>((*it).get(), ptCompare));
    if (index != csArrayItemNotFound)
    {
      //printf("FOUND: %d %s\n", (*it)->id, (*it)->name.c_str());

      // Instance was found, copy it to the new array.
      newInstances.InsertSorted(instances.Get(index), ptCompare);
    }
    else
    {
      //printf("OBJECT: %"SIZET" %s\n", (*it)->id, (*it)->name.c_str());

      // Instance was not found, make a new instance.
      csRef<Instance> in; in.AttachNew(new Instance(*it, object_reg));
      newInstances.InsertSorted(in, ptCompare);
    }
  }

  // Replace array with the new array.
  instances = newInstances;

} // end CameraMoved()

void WorldManager::MovableCallBack::MovableChanged(iMovable* movable)
{
  if (!world)
  {
    movable->RemoveListener(this);
    return;
  }

  world->position = VectorHelper::Convert(movable->GetTransform().GetOrigin());

  world->CameraMoved();

} // end MovableChanged()

void WorldManager::CommitChanges(Common::World::Objectp object)
{
  printf("CommitChanges: %s\n", object->name.c_str());

  worldManager->Update(object);
} // end CommitChanges()

void WorldManager::CommitNew(Common::World::Objectp object)
{
  printf("CommitNew: %s\n", object->name.c_str());

  worldManager->Add(object, false);
} // end CommitNew()

