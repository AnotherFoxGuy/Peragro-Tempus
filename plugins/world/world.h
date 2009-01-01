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

/**
* Manages objects around the camera, by loading and deleting
* them as the camera moves.
*/
class WorldManager : public scfImplementation3<WorldManager,iWorld,iComponent, iEventHandler>
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

  bool HandleEvent(iEvent& ev) { UpdateOptions(); return true; }

  CS_EVENTHANDLER_NAMES ("peragro.world")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

private:
  /// Whether more tiles need to be loaded.
  bool loading;

  /// The proximity range in units.
  size_t radius;

  Common::World::WorldManager* worldManager;

  csRefArray<Instance> instances;
  csRefArray<Instance> cachedInstances;

  void CameraMoved();

private:
  /// The object registry.
  iObjectRegistry* object_reg;

  struct MovableCallBack
    : public scfImplementation1<MovableCallBack, iCameraListener>
  {
    WorldManager* world;
    MovableCallBack (WorldManager* w) : scfImplementationType (this) { world = w;}
    virtual void NewSector (iCamera* camera, iSector* sector) {}
    virtual void CameraMoved (iCamera* camera);
  };
  friend struct MovableCallBack;
  /// Callback to update the camera coordinates.
  csRef<MovableCallBack> cb;
  /// The current coordinates of the camera.
  csVector3 camera;

private:
  // TODO remove.
  void EnterWorld(float x, float z) {}
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

  /**
   * Initialize the world.
   * @param name The world name.
   * @param pl The pointer library.
   * @return true if successful.
   */
  virtual bool Initialize(const std::string& name);

  /// Returns the object registry.
  iObjectRegistry* GetObjectRegistry() { return object_reg; }

  void SetCamera(iCamera* camera);

  void UnSetCamera();

  /// Set the proximity range in units.
  void SetRange(size_t radius) { WorldManager::radius = radius; }
  /// Get the proximity range in units.
  size_t GetRange() const { return radius; }

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);
};


#endif
