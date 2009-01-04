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
struct iMovableListener;
class csVector3;

class iPointerLibrary;

namespace PT
{
  namespace World
  {
    class MapTile;
    class ModelManager;
    class InteriorManager;
  } // World namespace
} // PT namespace

using namespace PT::World;

/**
* Manages the tile grid array around the camera, by loading and deleting
* tiles as the camera moves.
*/
class WorldManager : public scfImplementation2<WorldManager,iWorld,iComponent>
{
private:
  bool UpdateOptions();

  struct WorldEventHandler : public scfImplementation1<WorldEventHandler, iEventHandler>
  {
    /// The world manager.
    WorldManager* mgr;

    /// Constructor.
    WorldEventHandler(WorldManager* mgr) : scfImplementationType (this) { this->mgr = mgr; }
    /// Destructor.
    ~WorldEventHandler() {}

    bool HandleEvent(iEvent& ev){ return mgr->UpdateOptions(); }

    CS_EVENTHANDLER_NAMES ("peragro.world.listener")
    CS_EVENTHANDLER_NIL_CONSTRAINTS
  };
  friend struct WorldEventHandler;
  csRef<WorldEventHandler> listener;

private:
  /// The event queue.
  csRef<iEventQueue> eventQueue;
  /// The name registry.
  csRef<iEventNameRegistry> nameRegistry;
  /// Event ID for "world loading".
  csEventID loadingId;
  /// Event ID for "world loaded".
  csEventID loadedId;

private:
  /// Whether more tiles need to be loaded.
  bool loading;

  /// Number of tiles in the cache.
  unsigned int maptilecachesize;
  /// Tile cache.
  MapTile** maptilecache;

  /// The current grid size.
  unsigned int current_size;
  /// Loaded subset of the world.
  MapTile*** current;

  /// Current x coordinate in tile space.
  int cx;
  /// Current z coordinate in tile space.
  int cz;

  /**
  * Load a tile with the given coordinates or return
  * it from the cache if it was already loaded.
  * @param x Tile index in the x dimension.
  * @param z Tile index in the z dimension.
  * @return The tile at the coordinates.
  */
  MapTile* LoadTile(int x, int z);

  /**
  * Enter a tile and load all surrounding tiles.
  * @param x Tile index in the x dimension.
  * @param z Tile index in the z dimension.
  */
  void EnterTile(int x, int z);

  /**
  * Check if player position is within tile boundaries.
  * @param dt Time delta. (currently unused)
  */
  void Tick(float dt);

private:
  void SetRange(size_t radius) {}
  size_t GetRange() const { return 0; }

private:
  /// The object registry.
  iObjectRegistry* object_reg;
  /// The model manager.
  ModelManager* modelManager;
  /// The interior manager.
  InteriorManager* interiorManager;

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
  /// The current coordinates of the camera.
  csVector3 camera;

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
  /// Returns the model manager.
  ModelManager* GetModelManager() { return modelManager; }
  /// Returns the interior manager.
  InteriorManager* GetInteriorManager() { return interiorManager; }

  void SetMesh(iMeshWrapper* mesh);

  void UnSetMesh();

  /**
  * Enter the world at a coordinate in world space.
  * @param position coordinate.
  */
  void EnterWorld(Geom::Vector3 position);

  /// Set the loaded tile grid size.
  void SetGridSize(unsigned int size);
  /// Get the loaded tile grid size.
  unsigned int GetGridSize() const;

  /// Set the cached tile grid size.
  void SetCacheSize(unsigned int size);
  /// Get the cached tile grid size.
  unsigned int GetCacheSize() const;

  /// Handles reporting warnings and errors.
  void Report(int severity, const char* msg, ...);

  void CommitChanges(Common::World::Object& object) {}
};


#endif
