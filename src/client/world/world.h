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
  namespace World
  {
    class MapTile;
    class ModelManager;
    class InteriorManager;

    /**
     * Manages the tile grid array around the camera, by loading and deleting
     * tiles as the camera moves.
     */
    class World
    {
    private:
      /// Whether more tiles need to be loaded.
      bool loading;

      /// Number of tiles in the cache.
      int maptilecachesize;
      /// Tile cache.
      MapTile** maptilecache;

      /// The current grid size.
      int current_size;
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
      /// The object registry.
      iObjectRegistry* object_reg;
      /// The model manager.
      ModelManager* modelManager;
      /// The interior manager.
      InteriorManager* interiorManager;

      struct FrameCallBack
        : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
      {
        World* world;
        FrameCallBack (World* w) : scfImplementationType (this) { world = w;}
        virtual void StartFrame (iEngine* engine, iRenderView* rview);
      };
      friend struct FrameCallBack;
      /// Callback to update the camera coordinates.
      csRef<FrameCallBack> cb;
      /// The current coordinates of the camera.
      csVector3 camera;

    public:
      /// The name of the world.
      std::string basename;

      /**
       * Constructor.
       * @param name The world name.
       * @param object_reg The object registry.
       */
      World(const char* name, iObjectRegistry* object_reg);
      /// Destructor.
      ~World();

      /// Returns the object registry.
      iObjectRegistry* GetObjectRegistry() { return object_reg; }
      /// Returns the model manager.
      ModelManager* GetModelManager() { return modelManager; }
      /// Returns the interior manager.
      InteriorManager* GetInteriorManager() { return interiorManager; }

      /**
       * Enter the world at a horizontal (x, z) coordinate in world space.
       * @param x X coordinate.
       * @param z Z coordinate.
       */
      void EnterWorld(float x, float z);

      /// Set the loaded tile grid size.
      void SetGridSize(int size);
      /// Get the loaded tile grid size.
      int GetGridSize() const;

      /// Set the cached tile grid size.
      void SetCacheSize(int size);
      /// Get the cached tile grid size.
      int GetCacheSize() const;
    };

  } // World namespace
} // PT namespace

#endif
