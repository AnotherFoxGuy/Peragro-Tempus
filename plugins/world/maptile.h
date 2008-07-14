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
 * @file maptile.h
 *
 * @basic A map tile of the world.
 */

#ifndef MAPTILE_H
#define MAPTILE_H

#include "level.h"

#define TILESIZE (512.0f)

namespace PT
{
  namespace World
  {
    struct Interior;

    /**
     * A map tile of the world.
     */
    class MapTile : public Level
    {
    private:
      /// Whether this tile is currently visible.
      bool visible;

      /// The sector this tile is in.
      csRef<iSector> sector;

      /**
       * Load an instance into this tile. (meshes/portals/lights/...)
       * @param meshNode The document node of the mesh.
       */
      void LoadInstance(iDocumentNode* meshNode);

      /**
       * Make this tile and it's instances visible.
       */
      virtual void SetReady();

    public:
      /// X coordinate in tile space.
      int x;
      /// Z coordinate in tile space.
      int z;
      /// X coordinate in world space.
      float xbase;
      /// Z coordinate in world space.
      float zbase;

      /**
       * Create a new map tile.
       * @param x0 The x coordinate in tile space.
       * @param z0 The z coordinate in tile space.
       * @param fileName The data file for this tile.
       * @param world The world to add this tile to.
       */
      MapTile(int x0, int z0, const std::string& fileName, World* world);

      /// Destructor.
      ~MapTile();

      /**
       * Set the visibility of the tile.
       * @param visible True is visible, false is invisible.
       * @param force Resets visibility on _all_ meshes, don't use unless you
       * know what you're doing.
       */
      void SetVisible(bool visible, bool force = false);
    };

  } // World namespace
} // PT namespace

#endif // MAPTILE_H
