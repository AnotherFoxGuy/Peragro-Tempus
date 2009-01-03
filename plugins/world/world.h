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
/**
 * @file world.h
 *
 * @basic Manages the game world.
 */

#ifndef WORLD_H
#define WORLD_H

#include "include/world.h"

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <iengine/engine.h>
#include <iengine/movable.h>
#include <iengine/camera.h>
#include <imap/loader.h>

#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>
#include <csutil/eventnames.h>

#include <string>

struct iObjectRegistry;
class csVector3;

class iPointerLibrary;

#include "include/cacheuser.h"
struct iCacheEntry;

#include "common/world/world.h"

#include <iengine/mesh.h>
#include <iutil/object.h>
struct EditorObject : public scfImplementation2<EditorObject, iObject, iMovableListener>, 
  public Common::World::Object
{
  iObjectRegistry* object_reg;

  EditorObject (const Common::World::Object& object, iObjectRegistry* obj_reg, iMeshWrapper* wrap) 
    : scfImplementationType (this), Common::World::Object(object), object_reg(obj_reg)
  {
    wrap->GetMovable()->AddListener(this);
    wrap->QueryObject()->ObjAdd(this);
  }

  void CommitChanges() 
  {
    csRef<iWorld> world = csQueryRegistry<iWorld> (object_reg);
    //world->CommitChanges(&this);
  }

  // iMovableListener
  void MovableChanged (iMovable* movable) 
  {
    position = movable->GetTransform().GetOrigin();
    CommitChanges();
  }

  void MovableDestroyed (iMovable* movable) {}
  // iObject
  void SetName (const char *iName) 
  { name = iName; CommitChanges(); }
  const char* GetName () const { return name.c_str(); }
  uint GetID () const { return (uint)id; }
  void SetObjectParent (iObject *obj) {}
  iObject* GetObjectParent () const { return 0; }
  void ObjAdd (iObject *obj) {}
  void ObjRemove (iObject *obj) {}
  void ObjRemoveAll () {}
  void ObjAddChildren (iObject *Parent) {}
  iObject* GetChild (int iInterfaceID, int iVersion, const char *Name, bool FirstName) const { return 0; }
  iObject* GetChild (const char *Name) const { return 0; }
  csPtr<iObjectIterator> GetIterator () { return 0; }
  void ObjReleaseOld (iObject *obj) {}
  void AddNameChangeListener (iObjectNameChangeListener* listener) {}
  void RemoveNameChangeListener (iObjectNameChangeListener* listener) {}
  iObject* GetChild (int iInterfaceID, int iVersion, const char *Name = 0) const { return 0; }
};

/**
* Manages objects around the camera, by loading and deleting
* them as the camera moves.
*/
class WorldManager : public scfImplementation3<WorldManager, iWorld, iComponent, iEventHandler>
{
private:
  bool UpdateOptions();

private:
  struct Instance : public Common::World::Object, public csRefCount, public iCacheUser
  {
  private:
    iObjectRegistry* object_reg;
    csRef<iMeshWrapper> instance;
    void Loaded(iCacheEntry* cacheEntry);
    void DoneLoading(bool success) {}

  public:
    Instance (const Common::World::Object& object, iObjectRegistry* obj_reg);
    ~Instance ();
  };

private:
  /// The event queue.
  csRef<iEventQueue> eventQueue;
  /// The name registry.
  csRef<iEventNameRegistry> nameRegistry;
  /// Event ID for "world loading".
  csEventID loadingId;
  /// Event ID for "world loaded".
  csEventID loadedId;

  bool HandleEvent(iEvent& ev);

  CS_EVENTHANDLER_NAMES ("peragro.world")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

private:
  /// Whether more meshes need to be loaded.
  bool loading;
  bool Loading();

  /// The proximity range in units.
  size_t radius;

  Common::World::WorldManager* worldManager;

  csRefArray<Instance> instances;

  void CameraMoved();

private:
  /// The object registry.
  iObjectRegistry* object_reg;

  struct MovableCallBack
    : public scfImplementation1<MovableCallBack, iMovableListener>
  {
    WorldManager* world;
    MovableCallBack (WorldManager* w) : scfImplementationType (this) { world = w;}
    void MovableChanged (iMovable* movable);
    void MovableDestroyed (iMovable* movable) {}
  };
  friend struct MovableCallBack;
  /// Callback to update the mesh coordinates.
  csRef<MovableCallBack> cb;
  csVector3 position;

private:
  // TODO remove.
  void SetGridSize(unsigned int size) {}
  unsigned int GetGridSize() const { return 0; }
  void SetCacheSize(unsigned int size) {}
  unsigned int GetCacheSize() const { return 0; }

public:
  /// The name of the world.
  std::string basename;

  /// Constructor.
  WorldManager(iBase* iParent);
  /// Destructor.
  ~WorldManager();

  /// From iComponent.
  virtual bool Initialize(iObjectRegistry*);

  /// Returns the object registry.
  iObjectRegistry* GetObjectRegistry() { return object_reg; }

  /**
   * Initialize the world.
   * @param name The world name.
   * @param pl The pointer library.
   * @return true if successful.
   */
  virtual bool Initialize(const std::string& name);

  /**
  * Enter the world at a coordinate in world space.
  * @param position coordinate.
  */
  void EnterWorld(Geom::Vector3 position);

  void SetMesh(iMeshWrapper* wrapper);

  void UnSetMesh();

  /// Set the proximity range in units.
  void SetRange(size_t radius) { WorldManager::radius = radius; }
  /// Get the proximity range in units.
  size_t GetRange() const { return radius; }

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);
};


#endif
