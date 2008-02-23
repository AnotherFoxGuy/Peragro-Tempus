#ifndef MAPTILE_H
#define MAPTILE_H

#include "level.h"

#define TILESIZE (512.0f)

struct Interior;

class MapTile : public Level
{
private:
  bool visible;

  // Resources.
  csRef<iSector> sector;

  void LoadInstance(iDocumentNode* meshNode);

public:
  /// Coordinates in tile space.
  int x, z;
  /// Coordinates in worldspace.
  float xbase, zbase;

  MapTile(int x0, int z0, const std::string& fileName, World* world);
  ~MapTile();

  /**
   * Set the visibility of the tile.
   * @param visible True is visible, false is invisible.
   */
  void SetVisible(bool visible);
};

#endif // MAPTILE_H
