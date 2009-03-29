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
#include <iengine/mesh.h>
#include <iengine/camera.h>
#include <imap/loader.h>

#include <csutil/weakref.h>

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

#include "common/util/geomhelper.h"



/**
 * Manages objects around the camera, by loading and deleting
 * them as the camera moves.
 */
class WorldManager : public scfImplementation3<WorldManager, iWorld, iComponent, iEventHandler>
{
private:
  /**
   * Represents an object loaded in the world manager.
   */
  struct Instance : public csRefCount, public iCacheUser
  {
  private:
    /// The object registry.
    iObjectRegistry* object_reg;
    /// The mesh wrapper.
    csRef<iMeshWrapper> instance;
    /// The world object.
    Common::World::Objectp _object;

    /// TODO
    void Loaded(iCacheEntry* cacheEntry);
    /// TODO
    void DoneLoading(bool success);

  public:
    /// Object id.
    size_t id;

  public:
    /// Construct from an object.
    Instance(Common::World::Objectp object, iObjectRegistry* obj_reg);
    /// Destructor.
    ~Instance();
  };

  struct MovableCallBack
    : public scfImplementation1<MovableCallBack, iMovableListener>
  {
    WorldManager* world;
    MovableCallBack(WorldManager* w) : scfImplementationType(this), world(w) {}
    void MovableChanged(iMovable* movable);
    void MovableDestroyed(iMovable* movable) {}
  };
  friend struct MovableCallBack;
  /// Callback to update the reference point position.
  csRef<MovableCallBack> cb;

private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The event queue.
  csRef<iEventQueue> eventQueue;
  /// The name registry.
  csRef<iEventNameRegistry> nameRegistry;

  csEventID loadingId;
  csEventID loadedId;
  csEventID updateOptionsId;
  csEventID objectPXId;
  csEventID objectMXId;
  csEventID objectPYId;
  csEventID objectMYId;
  csEventID objectPZId;
  csEventID objectMZId;
  csEventID interactId;
  csEventID objectStepPId;
  csEventID objectStepMId;

  CS_EVENTHANDLER_NAMES ("peragro.world")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

  /// The world manager that takes care of the tree of all objects.
  boost::shared_ptr<Common::World::WorldManager> worldManager;
  /// Array of objects in world currently loaded.
  csRefArray<Instance> instances;
  /// Position of the reference point, the camera.
  WFMath::Point<3> position;
  /// The player's mesh.
  csWeakRef<iMeshWrapper> playerMesh;
  /// The currently selected mesh.
  csWeakRef<iMeshWrapper> selectedMesh;
  /// Wether EnterWorld() has been called.
  bool hasEnteredWorld;
  /// Whether more meshes need to be loaded.
  bool loading;
  /// The proximity range in units.
  size_t loadRadius;
  /// The distance objects are moved while editing.
  float editStepSize;

private:
  /// Handle all incoming events.
  bool HandleEvent(iEvent& ev);
  /// Update relevant options from the config manager.
  bool UpdateOptions();
  /// Check on the world loading progress, send events if loaded.
  bool Loading();
  /// Called when the camera moves to update visible objects.
  void CameraMoved();

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
  iObjectRegistry* GetObjectRegistry();

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
  void EnterWorld(WFMath::Point<3> position);

  /// Set the mesh that the reference point follows.
  void SetMesh(iMeshWrapper* wrapper);

  /// Stop following the current reference point target.
  void UnSetMesh();

  /// Set the proximity range in units.
  void SetRange(size_t radius);
  /// Get the proximity range in units.
  size_t GetRange() const;

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

  /// Commit changes to an object to the Common::World::WorldManager.
  void CommitChanges(Common::World::Objectp object);
  /// Commit a new object to the Common::World::WorldManager.
  void CommitNew(Common::World::Objectp object);
};

inline void WorldManager::Instance::DoneLoading(bool success)
{}

inline iObjectRegistry* WorldManager::GetObjectRegistry()
{ return object_reg; }

inline void WorldManager::SetRange(size_t radius)
{ WorldManager::loadRadius = radius; }

inline size_t WorldManager::GetRange() const
{ return loadRadius; }

#endif
