#ifndef WORLD_H
#define WORLD_H

#include <crystalspace.h>
#include <string>

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

#endif
