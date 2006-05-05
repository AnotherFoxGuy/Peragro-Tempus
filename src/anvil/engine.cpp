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

#include "command.h"
#include "engine.h"

#include <wx/window.h>

#include <cssysdef.h>
#include <csgeom/math3d.h>
#include <cstool/initapp.h>
#include <cstool/csview.h>  
#include <cstool/saverfile.h>
#include <csutil/csstring.h>
#include <csutil/event.h>
#include <csutil/plugmgr.h>
#include <csutil/refarr.h>
#include <csutil/sysfunc.h>
#include <csutil/syspath.h>
#include <csutil/floatrand.h>
#include <iengine/camera.h>
#include <iengine/campos.h>
#include <iengine/engine.h>
#include <iengine/material.h>
#include <iengine/mesh.h>
#include <iengine/movable.h>
#include <iengine/region.h>
#include <iengine/renderloop.h>
#include <iengine/rendersteps/igeneric.h>
#include <iengine/sector.h>
#include <iengine/collectn.h>
#include <imap/loader.h>
#include <imap/saver.h>
#include <imesh/terrain.h>
#include <imesh/object.h>
#include <iutil/csinput.h>
#include <iutil/event.h>
#include <iutil/eventq.h>
#include <iutil/object.h>
#include <iutil/objreg.h>
#include <iutil/vfs.h>
#include <iutil/virtclk.h>
#include <ivaria/view.h>
#include <ivaria/terraform.h>
#include <ivaria/simpleformer.h>
#include <ivideo/graph2d.h>
#include <ivideo/graph3d.h>
#include <ivideo/material.h>
#include <ivideo/shader/shader.h>
#include <ivideo/wxwin.h>

anvEngine* anvEngine::anvil = NULL;

anvEngine::anvEngine()
{
  anvEngine::anvil = this;
  selectionMesh = 0;
  editmode = MeshEditMode;
  selections.SetLength(LastEditMode);
  operation = MoveOperation;
  isDragging = false;
  constraint = NoConstraint;
  speedMultiplier = 20;
  do_freelook = false;
}

anvEngine::~anvEngine()
{
}

anvEngine* anvEngine::GetAnvil()
{
  return anvEngine::anvil;
}

bool anvEngine::ReportError(const char* error)
{
  csReport(object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.anvil", error);
  return false;
}

void anvEngine::ProcessFrame()
{
  csTicks elapsed_time = vc->GetElapsedTicks ();
  
  float rotateSpeed = (elapsed_time / 1000.0);
  
  float speed = rotateSpeed * speedMultiplier * 0.06;

  iCamera* c = view->GetCamera();

  if (panel3d)
  {
    int w, h;
    panel3d->GetClientSize(&w, &h);
    view->SetRectangle(0, 0, w, h);

    c->SetPerspectiveCenter(w/2, h/2);
    c->SetFOVAngle(90, w);
  }

  float rotX = 0, rotY = 0;

  if (kbd->GetKeyState (CSKEY_UP))
    c->Move (CS_VEC_FORWARD * 8 * speed);
  if (kbd->GetKeyState (CSKEY_DOWN))
    c->Move (CS_VEC_BACKWARD * 8 * speed);
  if (kbd->GetKeyState (CSKEY_RIGHT))
    c->Move (CS_VEC_RIGHT * 8 * speed);
  if (kbd->GetKeyState (CSKEY_LEFT))
    c->Move (CS_VEC_LEFT * 8 * speed);
  
  if (kbd->GetKeyState (CSKEY_SHIFT))
    speedMultiplier += .5;
  else
    speedMultiplier = 20; 

  if (kbd->GetKeyState (CSKEY_PGUP))
    rotX = -rotateSpeed;
  if (kbd->GetKeyState (CSKEY_PGDN))
    rotX = rotateSpeed;
  
  csMatrix3 rot = csXRotMatrix3 (rotX) * csYRotMatrix3 (rotY);
  c->GetTransform ().RotateThis (rot);
  
  flashlight->GetMovable()->GetTransform().SetOrigin(c->GetTransform().GetOrigin());
  
  if (!g3d->BeginDraw (engine->GetBeginDrawFlags () | CSDRAW_3DGRAPHICS))
    return;
  
  view->Draw ();
  g3d->FinishDraw();
}

void anvEngine::FinishFrame()
{
  g3d->Print(0);
}

bool anvEngine::OnKeyboard(iEvent& ev)
{
  // We got a keyboard event.
  if (csKeyEventHelper::GetEventType(&ev) == csKeyEventTypeDown)
  {
    // The user pressed a key (as opposed to releasing it).
    utf32_char code = csKeyEventHelper::GetRawCode(&ev);

    csKeyModifiers modifiers;
    csKeyEventHelper::GetModifiers (&ev, modifiers);
    bool ctrl = modifiers.modifiers[csKeyModifierTypeCtrl] != 0;

    if (code == CSKEY_ESC)
    {
      // The user pressed escape, so terminate the application.  The proper way
      // to terminate a Crystal Space application is by broadcasting a
      // csevQuit event.  That will cause the main run loop to stop.  To do
      // so we retrieve the event queue from the object registry and then post
      // the event.
      csRef<iEventQueue> q =
        CS_QUERY_REGISTRY(GetObjectRegistry(), iEventQueue);
      if (q.IsValid())
        q->GetEventOutlet()->Broadcast(csevQuit (GetObjectRegistry()));
    }

    if (code == 'l')
    {
      if (view->GetCamera()->GetSector()->GetLights()->Find(flashlight) == -1)
      {
        view->GetCamera()->GetSector()->GetLights()->Add(flashlight);
        flashlight->Setup();
        flashlight->Setup();
      }
      else
      {
        view->GetCamera()->GetSector()->GetLights()->Remove(flashlight);
      }
    }

    if (ctrl)
    {
      if (code == 'z')
      {
        Undo ();
      }

      if (code == 'y')
      {
        Redo ();
      }

      if (code == 's')
      {
        SaveWorld ("/this/", "world.save");
      }
    }
  }
  return false;
}

bool anvEngine::OnMouseDown(iEvent& ev)
{
  return HandleMouseEvent(ev);
}

bool anvEngine::OnMouseUp(iEvent& ev)
{
  return HandleMouseEvent(ev);
}

bool anvEngine::OnMouseMove(iEvent& ev)
{
  return HandleMouseEvent(ev);
}

void anvEngine::EndDrag()
{
  csVector3 worldEnd = dragMesh->GetMovable()->GetFullPosition() + dragOffset;
  
  // No movement, cycle through operations
  if (csSquaredDist::PointPoint(worldStart, worldEnd) < EPSILON)
  {
    operation = (operation + 1) % ScaleOperation; // Temporarily disable ScaleOperation
  }
  // Complete operation, push command
  else
  {
    csReversibleTransform transform = GetOperationTransform(worldEnd-worldStart, 0); //this should be fixed; shouldn't be 0... looses last frame on rotate.
    PushCommand(new anvTransformCommand(transform, true));
  }
  
  UpdateSelectionInfo();
  
  isDragging = false;
}

void anvEngine::UpdateSelectionInfo()
{
  if (!GetSelection().IsEmpty())
  {
    if (editmode == MeshEditMode)
    {
      csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper> (GetSelection()[0]);
      if (mesh)
      {
        csVector3 pos = mesh->GetMovable()->GetFullPosition();
        csPrintf("<v x=\"%f\" y=\"%f\" z=\"%f\"/>\n", pos.x, pos.y, pos.z);
      }
    }
  }
}

iMeshWrapper* anvEngine::HitMesh(int mouse_x, int mouse_y, csVector3& vc, csVector3& vo, csVector3& vw, csVector3& isect)
{
  iCamera* camera = view->GetCamera();

  // Setup perspective vertex, invert mouse Y axis.
  csVector2 p (mouse_x, camera->GetShiftY() * 2 - mouse_y);

  camera->InvPerspective (p, 1, vc);
  vw = camera->GetTransform ().This2Other (vc);

  iSector* sector = camera->GetSector ();
  if (sector)
  {
    const uint max_distance = 10000000;
    
    vo = camera->GetTransform ().GetO2TTranslation ();
    csVector3 end = vo + (vw - vo) * (int)max_distance;

    return sector->HitBeam (vo, end, isect, 0, true);
  }
  else
    return 0;
}

bool anvEngine::HandleMouseEvent(iEvent& ev)
{
  int mouse_x, mouse_y;
  
  mouse_x = csMouseEventHelper::GetX(&ev);
  mouse_y = csMouseEventHelper::GetY(&ev);
  
  bool mouse_down = ev.Name == csevMouseDown (object_reg, 0);
  bool mouse_up = ev.Name == csevMouseUp (object_reg, 0);

  bool shift_held = kbd->GetKeyState (CSKEY_SHIFT);
  
  // The following vectors are only set if needed.

  // Vector from (0,0,0) to 'vc' in camera space corresponding to 
  // the point we clicked on.
  csVector3 vc;
  // Vector from 'vo' to 'vw' in world space corresponding to
  // same vector.
  csVector3 vo, vw;
  csVector3 isect;
  
  iMeshWrapper* clickedMesh = 0;

  /* Happens at both press and depress of left mouse button, as well as while dragging */
  if ((mouse_down || mouse_up) && csMouseEventHelper::GetButton(&ev) == csmbLeft || isDragging)
  {
    clickedMesh = HitMesh(mouse_x, mouse_y, vc, vo, vw, isect);
  }
  
  if (mouse_up && !isDragging && csMouseEventHelper::GetButton(&ev) == csmbLeft)
  {
    // Handle selections
    if (clickedMesh && !visualsRegion->IsInRegion(clickedMesh->QueryObject()))
    {
      anvSelection newSelection, oldSelection;
      
      oldSelection = GetSelection();

      if (shift_held)
      {
        newSelection = oldSelection;
        // Deselect
        if (oldSelection.Find(clickedMesh) != csArrayItemNotFound)
          newSelection.Delete(clickedMesh);
        else
          newSelection.Push(clickedMesh);
      }
      else
      {
        newSelection.Push(clickedMesh);
      }
      
      PerformAndPush(new anvSelectCommand(newSelection, oldSelection));
      
      UpdateSelectionInfo();
    }
  }
  
  if (mouse_down)
  {
    if (csMouseEventHelper::GetButton(&ev) == csmbLeft)
    { 
      // Arrow Click
      if (clickedMesh && visualsRegion->IsInRegion(clickedMesh->QueryObject())
          && clickedMesh->QueryObject()->GetName()
          != selectionMesh->QueryObject()->GetName() && !isDragging)
      {
        // Start Dragging operation
        isDragging = true;
        
        if (clickedMesh == arrow_x || clickedMesh == arrow_x2)
          constraint = ZConstraint;
        else if (clickedMesh == arrow_y || clickedMesh == arrow_y2)
          constraint = YConstraint;
        else if (clickedMesh == arrow_z || clickedMesh == arrow_z2)
          constraint = XConstraint;
        
        // Sets constraintAxis as a vector along the selected axis
        constraintAxis = csVector3(
         ((constraint == XConstraint)?1:0),
         ((constraint == YConstraint)?1:0),
         ((constraint == ZConstraint)?1:0));
        dragMesh = ((constraintAxis != csVector3(0,0,0)) 
         ? clickedMesh : (iMeshWrapper*)selectionMesh);
        
        // left from before, uncommented by iceeey ;)
        dragOffset = isect-dragMesh->GetMovable()->GetFullPosition();
        worldStart = isect;
	
        // sets up the Drag Plane
        csVector3 dragCenter = dragMesh->GetMovable()->GetFullPosition();
        dragPlaneNormal = dragCenter - vo;
        dragPlane = csPlane3(dragPlaneNormal, 0);
        dragPlane.SetOrigin(dragCenter);

        // sets up a projection of the constraint axis on the drag plane
        dragProjection = ((constraintAxis % dragPlaneNormal) % dragPlaneNormal);
        dragProjection.Normalize();

        // sets drag plane positions.  both start and current begin at the same point
        float dist;
        csIntersect3::SegmentPlane (vo, (vw - vo) * (int) 100000, dragPlane, isect, dist);
        dragPlaneStartPos = isect;
        dragPlaneCurrentPos = isect;
      }
    }
    
    // Start mouselook
    else if (csMouseEventHelper::GetButton(&ev) == csmbRight)
    {
      do_freelook = true;
      int width, height;
      panel3d->GetClientSize(&width, &height);
      panel3d->WarpPointer (width / 2, height / 2);
      panel3d->SetCursor(wxCursor(wxCURSOR_BLANK));
    }
  }
  
  if (mouse_up)
  {
    // Handle operations
    if (isDragging)
    {
      EndDrag();
    }
  }
  
  if (isDragging)
  {
    csVector3 dragCenter = dragMesh->GetMovable()->GetFullPosition() + dragOffset;
        
    csVector3 isect;
    float dist;

    // if arrow is point STRAIGHT at us, or STRAIGHT away, do nothing
    if (csIntersect3::SegmentPlane (vo, (vw-vo)*(int)100000, dragPlane, isect, dist))
    {
      // Change Starting Position to where the mouse was on the drag plane last frame
      dragPlaneStartPos = dragPlaneCurrentPos;

      // Current Position is where mouse intersects the drag plane
      dragPlaneCurrentPos = isect;
      
      // Distance change along projected line (in the proper direction)
      csVector3 dragPlaneDistanceChange = dragProjection * 
       (dragProjection * (dragPlaneCurrentPos - dragPlaneStartPos));
      dragPlaneCurrentPos = dragPlaneStartPos + dragPlaneDistanceChange;

      // + or - change in numbers, used for move and rotate
      float scalarChange = (dragPlaneDistanceChange.Norm() *
      (((dragProjection * (dragPlaneCurrentPos - dragPlaneStartPos)) < 0) ? 1 : -1));

      // Apply to coordinate axis, + or -
      csVector3 worldDiff = constraintAxis * scalarChange;

      // Make the changes
      csReversibleTransform transform = GetOperationTransform(worldDiff, scalarChange);
      anvTransformCommand::Transform(transform, true);
    }
  }

  // Mouse Look

  static bool first_time = false;

  if (do_freelook)
  {
    if (!first_time)
    { 
      int width, height;
      panel3d->GetClientSize(&width, &height);
      panel3d->WarpPointer(width / 2, height / 2);

      // Calculations use CS coordinates!
      int widthc, heightc;
      widthc = g2d->GetWidth();
      heightc = g2d->GetHeight();

      bool inverse_mouse = false;

      float speed = 6.0f;

      float x = (float) speed * -((float)(mouse_y - (heightc / 2) )) / (heightc*2)*(1-2*(int)inverse_mouse);
      float y = (float) speed * ((float)(mouse_x - (widthc / 2) )) / (widthc*2);

      csMatrix3 mat = view->GetCamera ()->GetTransform ().GetO2T ();
      if(x)
        mat = csXRotMatrix3(x) * mat;
      if(y)
        mat *= csYRotMatrix3(y);
      view->GetCamera ()->SetTransform ( csOrthoTransform 
          (mat, view->GetCamera ()->GetTransform ().GetOrigin ()));
    }
    else
      first_time = false;
  }
  else
    first_time = true;

  if (mouse_up)
  {
    // End mouselook
    if (csMouseEventHelper::GetButton(&ev) == csmbRight && do_freelook)
    {
      do_freelook = false;
      panel3d->GetParent()->SetCursor(wxCursor(wxCURSOR_ARROW));
    }
  }
  
  return false;
}

csReversibleTransform anvEngine::GetOperationTransform(csVector3 worldDiff, float scalerDiff)
{
  // Apply constraints
  switch (constraint)
  {
    case XConstraint:
      worldDiff.y = worldDiff.z = 0.0;
      break;
    case YConstraint:
      worldDiff.x = worldDiff.z = 0.0;
      break;
    case ZConstraint:
      worldDiff.x = worldDiff.y = 0.0;
      break;
  }
  
  csReversibleTransform transform;
  
  switch (operation)
  {
    case MoveOperation:
      transform.SetOrigin(worldDiff);
      break;
    case RotateOperation:
      if (kbd->GetKeyState (CSKEY_SHIFT))
        speedMultiplier = 25;
      else
        speedMultiplier = 2; 
      transform.RotateThis(constraintAxis, scalerDiff/speedMultiplier);
      break;
    case ScaleOperation:
	// not currently implemented
      //transform.SetO2T((csXScaleMatrix3(amount)*worldDiff.x) * (csYScaleMatrix3(amount)*worldDiff.y) * (csZScaleMatrix3(amount)*worldDiff.z));
      break;
  }
  
  return transform;
}

bool anvEngine::Initialize(iObjectRegistry* object_reg, wxWindow* panel3d)
{
  this->object_reg = object_reg;
  this->panel3d = panel3d;

  iObjectRegistry* r = GetObjectRegistry();

  // Load application-specific configuration file.
  if (!csInitializer::SetupConfigManager(r,
      "/config/anvil.cfg", "Anvil"))
    return ReportError("Failed to initialize configuration manager!");

  // RequestPlugins() will load all plugins we specify.  In addition it will
  // also check if there are plugins that need to be loaded from the
  // configuration system (both the application configuration and CS or global
  // configurations).  It also supports specifying plugins on the command line
  // via the --plugin= option.
  if (!csInitializer::RequestPlugins(r,
    CS_REQUEST_VFS,
    CS_REQUEST_PLUGIN("crystalspace.graphics2d.wxgl", iGraphics2D),
    CS_REQUEST_OPENGL3D,
    CS_REQUEST_ENGINE,
    CS_REQUEST_FONTSERVER,
    CS_REQUEST_IMAGELOADER,
    CS_REQUEST_LEVELLOADER,
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_END))
    return ReportError("Failed to initialize plugins!");

  csBaseEventHandler::Initialize (object_reg);

  // Set up an event handler for the application.  Crystal Space is fully
  // event-driven.  Everything (except for this initialization) happens in
  // response to an event.
  if (!RegisterQueue(r, csevAllEvents (r)))
    return ReportError("Failed to set up event handler!");


  // Now get the pointer to various modules we need.  We fetch them from the
  // object registry.  The RequestPlugins() call we did earlier registered all
  // loaded plugins with the object registry.  It is also possible to load
  // plugins manually on-demand.
  g3d = CS_QUERY_REGISTRY(r, iGraphics3D);
  if (!g3d)
    return ReportError("Failed to locate 3D renderer!");

  engine = CS_QUERY_REGISTRY(r, iEngine);
  if (!engine)
    return ReportError("Failed to locate 3D engine!");

  engine->SetSaveableFlag (true);
  
  vc = CS_QUERY_REGISTRY(r, iVirtualClock);
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  kbd = CS_QUERY_REGISTRY(r, iKeyboardDriver);
  if (!kbd) return ReportError("Failed to locate Keyboard Driver!");
  
  loader = CS_QUERY_REGISTRY(r, iLoader);
  if (!loader) return ReportError("Failed to locate Loader!");
  

  // Get the shared string repository
  strings = CS_QUERY_REGISTRY_TAG_INTERFACE (object_reg, "crystalspace.shared.stringset", iStringSet);

  panel3d->GetParent()->Show(true);

  g2d = g3d->GetDriver2D();
  wxwin = SCF_QUERY_INTERFACE(g2d, iWxWindow);
  if( !wxwin )
  {
    csReport (object_reg, CS_REPORTER_SEVERITY_ERROR,
              "crystalspace.application.wxtest",
              "Canvas is no iWxWindow plugin!");
    return false;
  }
  
  wxwin->SetParent(panel3d);

  view.AttachNew(new csView (engine, g3d));
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());


  // Open the main system. This will open all the previously loaded plugins
  // (i.e. all windows will be opened).
  if (!csInitializer::OpenApplication(r))
    return ReportError("Error opening system!");
  
/*
  // Load our own renderloop
  csRef<iRenderLoop> rl = engine->GetRenderLoopManager()->Load("/peragro/anvil/visuals_rloop.xml");

  if (rl->GetStepCount() > 0)
  {
    // Add first renderstep for rendering visuals to the current renderloop
    if (!editSector->GetRenderLoop())
    {
      editSector->SetRenderLoop(engine->GetCurrentDefaultRenderloop());
    }
  
    editSector->GetRenderLoop()->AddStep(rl->GetStep(0));
  }*/

  visualsRegion = engine->CreateRegion("anvil_visuals");
  
  loader->SetAutoRegions (false);
  loader->LoadLibraryFile("/peragro/anvil/visuals.xml", visualsRegion);
  
  // Create some arrow meshes from the factories
  arrow_x = visualsRegion->FindMeshFactory("__arrow_x")->CreateMeshWrapper();
  arrow_y = visualsRegion->FindMeshFactory("__arrow_y")->CreateMeshWrapper();
  arrow_z = visualsRegion->FindMeshFactory("__arrow_z")->CreateMeshWrapper();
  arrow_x2 = visualsRegion->FindMeshFactory("__arrow_x")->CreateMeshWrapper();
  arrow_y2 = visualsRegion->FindMeshFactory("__arrow_y")->CreateMeshWrapper();
  arrow_z2 = visualsRegion->FindMeshFactory("__arrow_z")->CreateMeshWrapper();

  arrow_x2->QueryObject()->SetName ("__arrow_x2");
  arrow_y2->QueryObject()->SetName ("__arrow_y2");
  arrow_z2->QueryObject()->SetName ("__arrow_z2");

  // Flip the opposite arrows
  arrow_x2->GetMovable()->GetTransform().RotateOther (csVector3(0,1,0), PI);
  arrow_y2->GetMovable()->GetTransform().RotateOther (csVector3(0,0,1), PI);
  arrow_z2->GetMovable()->GetTransform().RotateOther (csVector3(0,1,0), PI);

  arrow_x2->GetMovable()->UpdateMove();
  arrow_y2->GetMovable()->UpdateMove();
  arrow_z2->GetMovable()->UpdateMove();

  visualsRegion->Add(arrow_x->QueryObject());
  visualsRegion->Add(arrow_y->QueryObject());
  visualsRegion->Add(arrow_z->QueryObject());  
  visualsRegion->Add(arrow_x2->QueryObject());
  visualsRegion->Add(arrow_y2->QueryObject());
  visualsRegion->Add(arrow_z2->QueryObject());

  selectionMesh = visualsRegion->FindMeshFactory("__selection_mesh")->CreateMeshWrapper();
  visualsRegion->Add(selectionMesh->QueryObject());

  // Load default world
  LoadWorld("/lev/flarge/", "world"); 

  view->GetCamera ()->SetSector (editSector);
  view->GetCamera ()->GetTransform ().SetOrigin (pos);

  engine->Prepare();
  engine->PrecacheDraw();

  // Initialize flashlight
  flashlight = engine->CreateLight("", view->GetCamera()->GetTransform().GetOrigin(), 3, csColor(1,1,1), CS_LIGHT_DYNAMICTYPE_DYNAMIC);

  return true;
}

int anvEngine::GetEditMode()
{
  return editmode;
}

void anvEngine::SetEditMode(anvEditMode mode)
{
  editmode = mode;
}

anvSelection& anvEngine::GetSelection()
{
  return selections[editmode];
}

void anvEngine::SetSelection(anvSelection selection)
{
  selections[editmode] = selection;
  
  if (editmode == anvEngine::MeshEditMode)
  {
	// Compute the bounding box of the selection
    anvSelection::Iterator iterator = selection.GetIterator();

    // Total selection bounding box
    csBox3 bbox;

    csPrintf("Selection:\n");
    while (iterator.HasNext())
    {
      csRef<iMeshWrapper> child = SCF_QUERY_INTERFACE(iterator.Next(), iMeshWrapper);
      csPrintf("\t%s\n", child->QueryObject()->GetName());

      csBox3 cbox;
      child->GetWorldBoundingBox(cbox);
      bbox += cbox;
    }
    
    UpdateSelection(bbox);
  }
}

void anvEngine::ClearSelection()
{
  anvSelection emptySelection;
  SetSelection(emptySelection);
}

void anvEngine::UpdateSelection(csBox3 bbox)
{
  uint32 visible = 1;
  if (selections[editmode].GetSize() == 0)
  {
    visible = 0;
  }

  selectionMesh->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_x->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_y->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_z->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_x2->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_y2->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);
  arrow_z2->SetFlagsRecursive(CS_ENTITY_INVISIBLE | CS_ENTITY_NOSHADOWS, visible);

  if (selections[editmode].GetSize() == 0)
  {
    return;
  }

  csVector3 max = bbox.Max();
  csVector3 min = bbox.Min();
  csVector3 center = bbox.GetCenter();
  
  // Scale the selection mesh to the selection's bounding box
  csBox3 sbox;
  selectionMesh->GetWorldBoundingBox(sbox);
  
  float bw = bbox.MaxX()-bbox.MinX();
  float bh = bbox.MaxY()-bbox.MinY();
  float bd = bbox.MaxZ()-bbox.MinZ();
  
  float sw = sbox.MaxX()-sbox.MinX();
  float sh = sbox.MaxY()-sbox.MinY();
  float sd = sbox.MaxZ()-sbox.MinZ();
  
  csMatrix3 scaleMatrix (bw/sw, 0, 0, 0, bh/sh, 0, 0, 0, bd/sd);
  //selectionMesh->HardTransform(csReversibleTransform(scaleMatrix, center));
  
  // Move the selection mesh into position
  selectionMesh->GetMovable()->Transform(scaleMatrix);
  selectionMesh->GetMovable()->UpdateMove();
  selectionMesh->GetMovable()->SetPosition(editSector, center);
  selectionMesh->GetMovable()->UpdateMove();
  
  // Move the arrows into position
  arrow_x->GetMovable()->SetPosition(editSector, csVector3(center.x, center.y, max.z));
  arrow_y->GetMovable()->SetPosition(editSector, csVector3(center.x, max.y, center.z));
  arrow_z->GetMovable()->SetPosition(editSector, csVector3(max.x, center.y, center.z));
  arrow_x2->GetMovable()->SetPosition(editSector, csVector3(center.x, center.y, min.z));
  arrow_y2->GetMovable()->SetPosition(editSector, csVector3(center.x, min.y, center.z));
  arrow_z2->GetMovable()->SetPosition(editSector, csVector3(min.x, center.y, center.z));

  arrow_x->GetMovable()->UpdateMove();
  arrow_y->GetMovable()->UpdateMove();
  arrow_z->GetMovable()->UpdateMove();
  arrow_x2->GetMovable()->UpdateMove();
  arrow_y2->GetMovable()->UpdateMove();
  arrow_z2->GetMovable()->UpdateMove();

  
}

bool anvEngine::LoadWorld(const char* path, const char* world)
{
  csRef<iVFS> VFS (CS_QUERY_REGISTRY (object_reg, iVFS));

  editRegion = engine->CreateRegion ("anvil_edit_region");
  
  loader->SetAutoRegions (true);
  
  ClearUndoRedoHistory();
  ClearSelection();

  VFS->ChDir(path);
  if (!loader->LoadMapFile (world, true, editRegion, false))
    ReportError("Error couldn't load level!");
  
  loader->SetAutoRegions (false);

  if (engine->GetCameraPositions ()->GetCount () > 0)
  {
    iCameraPosition* campos = engine->GetCameraPositions ()->Get (0);
    editSector = engine->GetSectors ()->FindByName (campos->GetSector ());
    pos = campos->GetPosition ();
  }
  else
  {
    editSector = engine->GetSectors ()->FindByName ("room");
    pos = csVector3 (0, 0, 0);
  }
  if (!editSector)
    ReportError("Can't find a valid starting position!");  

  return true;
}

void anvEngine::SaveWorld(const char* path, const char* world)
{
  csRef<iVFS> VFS (CS_QUERY_REGISTRY (object_reg, iVFS));
  VFS->ChDir(path);

  csRef<iPluginManager> pluginManager =
    CS_QUERY_REGISTRY (object_reg, iPluginManager);
  csRef<iSaver> saver =
    CS_QUERY_PLUGIN_CLASS(pluginManager, "crystalspace.level.saver", iSaver);
  if (!saver)
  {
    saver = CS_LOAD_PLUGIN(pluginManager, "crystalspace.level.saver", iSaver);
  }
  if (saver)
  {
    saver->SaveRegionFile(editRegion, world, CS_SAVER_FILE_WORLD);
  }
}

void anvEngine::PushCommand(iAnvCommand* command)
{
  undostack.Push(command);
  redostack.Empty();
}

void anvEngine::PerformAndPush(iAnvCommand* command)
{
  command->Perform();
  PushCommand(command);
}

void anvEngine::Undo()
{
  if (!undostack.IsEmpty ())
  {
    csRef<iAnvCommand> command = undostack.Pop();
    command->Revert();
    redostack.Push(command);
  }
}

void anvEngine::Redo()
{
  if (!redostack.IsEmpty ())
  {
    csRef<iAnvCommand> command = redostack.Pop();
    command->Perform();
    undostack.Push(command);
  }
}

void anvEngine::ClearUndoRedoHistory()
{
  undostack.Empty();
  redostack.Empty();
}
