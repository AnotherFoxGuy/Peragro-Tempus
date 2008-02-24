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

#ifndef WORLD_H
#define WORLD_H

#include <cssysdef.h>
#include <csutil/scf_implementation.h>
#include <iengine/engine.h>
#include <imap/loader.h>

#include <string>

struct iObjectRegistry;
struct iEngineFrameCallback;
class csVector3;

namespace PT
{
  class MapTile;
  class ModelManager;
  class InteriorManager;

  class World 
  {
  private:
    bool loading;

    /// Tile cache.
    int maptilecachesize;
    MapTile** maptilecache;

    /// Loaded subset of the world
    int current_size;
    MapTile*** current;

    /// Coordinates in tile space.
    int cx,cz;

    /// Load a tile with the given coordinates or return
    /// it from the cache if it was already loaded.
    MapTile* LoadTile(int x, int z);

    /// Enter a tile and load all surrounding tiles.
    void EnterTile(int x, int z);

    /// Check if player position is within tile boundaries.
    void Tick(float dt);

  private:
    iObjectRegistry* object_reg;
    ModelManager* modelManager;
    InteriorManager* interiorManager;

    // Updates the camera.
    struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
    {
      World* world;
      FrameCallBack (World* w) : scfImplementationType (this) { world = w;}
      virtual void StartFrame (iEngine* engine, iRenderView* rview);
    };
    friend struct FrameCallBack;
    csRef<FrameCallBack> cb;
    csVector3 camera;

  public:
    std::string basename;

    World(const char* name, iObjectRegistry* object_reg);
    ~World();

    iObjectRegistry* GetObjectRegistry() { return object_reg; }
    ModelManager* GetModelManager() { return modelManager; }
    InteriorManager* GetInteriorManager() { return interiorManager; }

    void EnterWorld(float x, float z);

    void SetGridSize(int size);
    int GetGridSize() const;

    void SetCacheSize(int size);
    int GetCacheSize() const;
  };

} // PT namespace

#endif
