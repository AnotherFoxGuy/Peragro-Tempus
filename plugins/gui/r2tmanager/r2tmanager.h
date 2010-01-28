/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#ifndef R2TMANAGER_H_
#define R2TMANAGER_H_

#include <cssysdef.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <iengine/mesh.h>
#include <cstool/procmesh.h>
#include <iutil/virtclk.h>

#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>
#include <csutil/eventnames.h>

#include <list>

#include "ir2tmanager.h"

class R2TWidgetManager
  : public scfImplementation3<R2TWidgetManager, iR2TWidgetManager,
      iComponent, iEventHandler>
{
private:
  iObjectRegistry* object_reg;
  csRef<iVirtualClock> vc;
  csRef<iEngine> engine;

  /// The event queue.
  csRef<iEventQueue> eventQueue;
  /// The name registry.
  csRef<iEventNameRegistry> nameRegistry;

  CS_EVENTHANDLER_NAMES ("peragro.r2tmanager")
  CS_EVENTHANDLER_NIL_CONSTRAINTS

  bool HandleEvent(iEvent& ev);

private:
  struct R2TWidget
  {
    R2TWidgetManager* manager;
    CEGUI::Window* window;
    csRef<iMeshWrapper> mesh;
    csReversibleTransform otrans;
    bool OnEventDestructionStarted (const CEGUI::EventArgs& e);
    bool EventWindowUpdated (const CEGUI::EventArgs& e);
    bool OnEventSized (const CEGUI::EventArgs& e);
    bool OnEventHidden (const CEGUI::EventArgs& e);
    bool OnEventShown (const CEGUI::EventArgs& e);

    bool OnEventMouseEnters (const CEGUI::EventArgs& e);
    bool OnEventMouseLeaves (const CEGUI::EventArgs& e);

    bool rotate;
    csRef<iTextureHandle> texh;
    CEGUI::Imageset* imageSet;
    csMeshOnTexture renderer;
    int cur_w;
    int cur_h;
    void SizeChanged(int w, int h);

    R2TWidget(R2TWidgetManager*, CEGUI::Window*, iMeshWrapper*);
    ~R2TWidget();
  };
  friend struct R2TWidget;
  std::list<R2TWidget*> widgets;
  std::list<R2TWidget*> deadpool;

public:
  R2TWidgetManager(iBase* parent);
  virtual ~R2TWidgetManager();

  virtual bool Initialize(iObjectRegistry*);

  virtual bool Register (CEGUI::Window*, iMeshWrapper*);
  virtual bool UnRegister (CEGUI::Window*);
};

#endif // R2TMANAGER_H_