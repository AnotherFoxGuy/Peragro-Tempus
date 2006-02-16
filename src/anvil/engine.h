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

#ifndef __ANV_ENGINE_H
#define __ANV_ENGINE_H

#include "ianvil.h"

#include <cstool/csapplicationframework.h>
#include <csutil/csbaseeventh.h>

struct iEngine;
struct iGraphics3D;
struct iKeyboardDriver;
struct iLoader;
struct iRegion;
struct iSector;
struct iView;

/**
 * The main Anvil application class.
 */
class anvEngine :
  public csApplicationFramework, public csBaseEventHandler
{
private:
  static anvEngine* anvil;

  /**
   * A reference to the 3D renderer plugin.
   */
  csRef<iGraphics3D> g3d;
  
  csRef<iGraphics2D> g2d;
  
  /**
   * A reference to the 3D engine plugin.
   */
  csRef<iEngine> engine;
  
  csRef<iLoader> loader;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iView> view;
  
  csRef<iStringSet> strings;
  
  float speedMultiplier;
  bool do_freelook;
  bool fooliage;
  
  /// The sector currently being edited
  iSector* editSector;
  
  csVector3 pos;
  
  /// Current edit mode
  int editmode;
  
  /// Dirty (modified) flag
  bool isDirty;
  
  /// The current selection for each edit mode
  csArray<anvSelection> selections;
  
  /// Current Undo stack
  anvCommandStack undostack;
  
  /// Current Redo stack
  anvCommandStack redostack;
  
  /// Current operation
  int operation;
  
  /// Current constraint
  int constraint;
  
  /// Is an operation being performed?
  bool isDragging;
  
  /// Starting mouse position in world space
  csVector3 worldStart;
  
  /// Mesh representing the selection
  csRef<iMeshWrapper> selectionMesh;
  
  /// Region where Anvil stores visuals (for example arrows) that are not saved
  iRegion* visualsRegion;
  
  /// References to the arrow meshes
  csRef<iMeshWrapper> arrow_x, arrow_y, arrow_z;
  csRef<iMeshWrapper> arrow_x2, arrow_y2, arrow_z2;
  
  /// The axis of the constraint
  csVector3 constraintAxis;
  
  /// The mesh being dragged
  csRef<iMeshWrapper> dragMesh;
  
  /// Offset from the center of the drag mesh
  csVector3 dragOffset;
  
  csRef<iLight> flashlight;
  
  /**
   * Set up everything that needs to be rendered on screen.  This routine is
   * called from the event handler in response to a cscmdProcess broadcast
   * message.
   */
  virtual void ProcessFrame();

  /**
   * Finally, render the screen.  This routine is called from the event handler
   * in response to a cscmdFinalProcess broadcast message.
   */
  virtual void FinishFrame();

  /**
   * Handle keyboard events, such as key presses and releases.  This routine is
   * called from the event handler in response to a csevKeyboard event.
   */
  virtual bool OnKeyboard(iEvent&);
  
  bool HandleMouseEvent(iEvent& ev);
  
  virtual bool OnMouseDown(iEvent&);
  virtual bool OnMouseUp(iEvent&);
  virtual bool OnMouseMove(iEvent& ev);
  
  /// Reverts the last command.
  void Undo();
  
  /// Performs the last undone command
  void Redo();
  
  /// Updates the operation in progress
  void UpdateOperation();
  
  /// Converts mouse coordinates to camera-space coordinates
  csVector3 MouseToCamera(int x, int y, float z = 10000);
  
  /// Returns hit mesh information given mouse coordinates
  iMeshWrapper* HitMesh(int mouse_x, int mouse_y, csVector3& vc, csVector3& vo, csVector3& vw, csVector3& isect);

  /// Updates the selection information
  void UpdateSelectionInfo();
  
  /// Ends a drag
  void EndDrag();
  
  /// Gets the transform from the current operation given world space coords
  csReversibleTransform GetOperationTransform(csVector3 worldDiff);

public:
  /**
   * Constructor.
   */
  anvEngine();

  /**
   * Destructor.
   */
  virtual ~anvEngine();

  static anvEngine* GetAnvil();
  
  enum anvEditMode
  {
    TerrainEditMode, MeshEditMode, LastEditMode
  };
  
  enum anvOperation
  {
    MoveOperation, RotateOperation, ScaleOperation, LastOperation
  };
  
  enum anvConstraint
  {
    NoConstraint, XConstraint, YConstraint, ZConstraint
  };
  
  /**
   * Final cleanup.
   */
  virtual void OnExit();

  /**
   * Main initialization routine.  This routine should set up basic facilities
   * (such as loading startup-time plugins, etc.).  In case of failure this
   * routine will return false.  You can assume that the error message has been
   * reported to the user.
   */
  virtual bool OnInitialize(int argc, char* argv[]);

  /**
   * Run the application.  Performs additional initialization (if needed), and
   * then fires up the main run/event loop.  The loop will fire events which
   * actually causes Crystal Space to "run".  Only when the program exits does
   * this function return.
   */
  virtual bool Application();
  
  int GetEditMode();
  void SetEditMode(anvEditMode mode);
  
  anvSelection& GetSelection();
  void SetSelection(anvSelection selection);
  
  bool LoadWorld(const char* path, const char* world);
  void SaveWorld(const char* world);
  
  /// Puts the command onto the undo stack, but does not perform it
  void PushCommand(iAnvCommand* command);
  
  /// Performs the command and pushes it to the undo stack
  void PerformAndPush(iAnvCommand* command);
  
  /// Updates the visible selection mesh and arrows
  void UpdateSelection(csBox3 bbox);
  
  csRef<iEngine> GetEngine()
  {return engine;}
  
};

#endif // __ANV_ENGINE_H
