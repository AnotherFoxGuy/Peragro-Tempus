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

#ifndef MAPTILE_H
#define MAPTILE_H

#include "level.h"

#define TILESIZE (512.0f)

namespace PT
{
  struct Interior;

  class MapTile : public Level
  {
  private:
    bool visible;

    // Resources.
    csRef<iSector> sector;

    void LoadInstance(iDocumentNode* meshNode);

    virtual void SetReady();

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

} // PT namespace

#endif // MAPTILE_H
