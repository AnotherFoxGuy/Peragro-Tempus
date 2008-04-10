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
#include <csutil/scf_implementation.h>
#include <iutil/eventh.h>

struct iCelEntity;
struct iCelPlLayer;
struct iCamera;
struct iPcTooltip;
struct iCollideSystem;
struct iMeshWrapper;
class csVector3;
struct iSector;

namespace CEGUI
{
  class Window;
}

/**
 * Class representing the game mouse cursor.
 */
class Cursor : public scfImplementation1<Cursor, iEventHandler>
{
private:
  /// Pointer to the selected entity.
  csWeakRef<iCelEntity> selectedEntity;
  /// The mouse x coordinate.
  int mouseX;
  /// The mouse y coordinate.
  int mouseY;
  /// Pointer to the name tag object.
  CEGUI::Window* nameTag;

  csEventID mouse;
  csEventID frame;

  bool OnMouseMove(iEvent&);
  void Frame(iEvent&);

  virtual bool HandleEvent(iEvent& ev);
  CS_EVENTHANDLER_NAMES ("peragro.cursor")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

  /**
   * Update the pointer to the selected entity.
   * @param pl Pointer to the CEL physical layer.
   * @param camera Pointer to the camera.
   */
  void UpdateSelected(iCelPlLayer* pl, iCamera* camera);

public:
  /**
   * Constructor.
   */
  Cursor();

  /// Destructor.
  ~Cursor();

  /// Draw the cursor.
  void Draw();

  /**
   * Returns a pointer to the selected entity.
   */
  iCelEntity* GetSelectedEntity();

  /**
   * Get the mesh in the world intersected by the cursor.
   * @param camera Pointer to the camera.
   * @param worldCoord Set to resulting position of the hit in world space.
   * @param untransfCoord Set to resulting position of the hit in camera space.
   * @param sector Set to resulting sector in which the point lies.
   * @return The intersected meshwrapper.
   */
  iMeshWrapper* Get3DPointFrom2D(iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);

  /**
   * Get the mesh in the world intersected by a 2D point in the view.
   * @param x X coordinate in screen space.
   * @param y Y coordinate in screen space.
   * @param camera Pointer to the camera.
   * @param worldCoord Set to resulting position of the hit in world space.
   * @param untransfCoord Set to resulting position of the hit in camera space.
   * @param sector Set to resulting sector in which the point lies.
   * @return The intersected meshwrapper.
   */
  iMeshWrapper* Get3DPointFrom2D(int x, int y, iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector = 0);

  /// Returns the mouse cursor x coordinate.
  int GetMouseX(){ return mouseX; }
  /// Returns the mouse cursor y coordinate.
  int GetMouseY(){ return mouseY; }
};

#endif // __Selector_h
