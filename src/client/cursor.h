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

#ifndef CURSOR_H
#define CURSOR_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/weakref.h>
#include <cstool/collider.h>

#include "CEGUI.h"

namespace PT
{
  class Client;
} // PT namespace 

struct iCelEntity;
struct iCelPlLayer;
struct iCamera;
struct iPcTooltip;
struct iCollideSystem;

/**
 * Class representing the game mouse cursor.
 */
class Cursor
{
private:
  /// Pointer to the client.
  PT::Client* client;
  /// Pointer to the selected entity.
  csWeakRef<iCelEntity> selectedEntity;
  /// The mouse x coordinate.
  int mouseX;
  /// The mouse y coordinate.
  int mouseY;
  /// Pointer to the name tag object.
  CEGUI::Window* nameTag;

public:
  /**
   * Constructor.
   * @param client Pointer to the client.
   */
  Cursor(PT::Client* client);

  /// Destructor.
  ~Cursor();

  /// Draw the cursor.
  void Draw();

  /**
   * Set the mouse coordinates.
   * @param x X coordinate.
   * @param y Y coordinate.
   */
  void MouseMove(int x, int y);

  /**
   * Update the pointer to the selected entity.
   * @param pl Pointer to the CEL physical layer.
   * @param camera Pointer to the camera.
   */
  void UpdateSelected(iCelPlLayer* pl, iCamera* camera);
  
  /**
   * Returns a pointer to the selected entity.
   */
  iCelEntity* GetSelectedEntity();

  /**
   * Get the mesh, intesection point, and sector, in the world at the cursor
   * position in the 2D view.
   * return a meshwrapper created there.
   * @param camera Pointer to the camera.
   * @param worldCoord Assigned the coordinates of the intersection in world space.
   * @param untransfCoord TODO: I don't know.
   * @param sector Assigned the sector of the intersection.
   * @return The intersected mesh.
   */
  iMeshWrapper* Get3DPointFrom2D(iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);

  /**
   * Get the mesh, intesection point, and sector, in the world at a point
   * in the 2D view.
   * @param x X coordinate.
   * @param y Y coordinate.
   * @param camera Pointer to the camera.
   * @param worldCoord Assigned the coordinates of the intersection in world space.
   * @param untransfCoord TODO: I don't know.
   * @param sector Assigned the sector of the intersection.
   * @return The intersected mesh.
   */
  iMeshWrapper* Get3DPointFrom2D(int x, int y, iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);

  /// Returns the mouse cursor x coordinate.
  int GetMouseX(){ return mouseX; }
  /// Returns the mouse cursor y coordinate.
  int GetMouseY(){ return mouseY; }
};

#endif // __Selector_h
