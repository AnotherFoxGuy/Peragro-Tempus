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

#include "cursor.h"

#include <cstool/collider.h>
#include <iengine/camera.h>
#include <iengine/sector.h>
#include <csutil/event.h>
#include <csutil/eventnames.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/prop.h>

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"
#include "client/gui/guimanager.h"

#include "client/pointer/pointer.h"
#include "client/entity/player/playerentity.h"

Cursor::Cursor(PointerLibrary* pl) : scfImplementationType (this)
{
  selectedEntity = 0;
  pointerlib = pl;

  CEGUI::WindowManager* winMgr = pointerlib->getGUIManager()->GetCEGUI()->GetWindowManagerPtr();
  CEGUI::Window* root = winMgr->getWindow("Root");
  nameTag = winMgr->createWindow("Peragro/Entity", "NameTag");
  root->addChildWindow(nameTag);

  iObjectRegistry* object_reg = pointerlib->getObjectRegistry();

  // Register for mouse and frame events.
  mouse = csevMouseEvent (object_reg);
  frame = csevFrame (object_reg);
  csEventID events[] = { frame, mouse, CS_EVENTLIST_END };
  csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
  q->RegisterListener (this, events);
}

Cursor::~Cursor()
{
  iObjectRegistry* object_reg = pointerlib->getObjectRegistry();
  csRef<iEventQueue> q (csQueryRegistry<iEventQueue> (object_reg));
  q->RemoveListener (this);
}

bool Cursor::HandleEvent(iEvent& event)
{
  iObjectRegistry* object_reg = pointerlib->getObjectRegistry();

  if (event.Name == frame)
  {
    Frame(event);
    return false;
  }
  else if (CS_IS_MOUSE_EVENT(object_reg, event))
  {
    switch(csMouseEventHelper::GetEventType(&event))
    {
    case csMouseEventTypeMove:
      return OnMouseMove(event);
      break;
    case csMouseEventTypeUp:
    case csMouseEventTypeDown:
    case csMouseEventTypeClick:
    case csMouseEventTypeDoubleClick:
      break;
    }
  }
  return false;
}

void Cursor::Frame(iEvent& e)
{
  static int x = 0;
  static int y = 0;

  PT::Entity::PlayerEntity* player = pointerlib->getPlayer();
  iObjectRegistry* object_reg = pointerlib->getObjectRegistry();
  csRef<iCelPlLayer> pl = csQueryRegistry<iCelPlLayer> (object_reg);
  if (player && pl)
  {
    if (x != mouseX  && y != mouseY)
    {
      csRef<iPcDefaultCamera> cam = player->GetCamera();
      if (cam && cam->GetCamera()->GetSector())
        UpdateSelected(pl, cam->GetCamera());
    }
  }

  x = mouseX;
  y = mouseY;

  Draw();
}

bool Cursor::OnMouseMove(iEvent& e)
{
  mouseX = csMouseEventHelper::GetX(&e);
  mouseY = csMouseEventHelper::GetY(&e);

  // This returns false so the event is also handled by CEGUI.
  return false;
}

void Cursor::Draw()
{
  if (selectedEntity.IsValid())
  {
    csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(selectedEntity, iPcProperties);
    if (!pcprop) return;

    int type = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type"));
    switch(type)
    {
    case PT::Common::Entity::PCEntityType:
      nameTag->setProperty("NormalTextColour", "FF05AA05");
      break;
    case PT::Common::Entity::NPCEntityType:
      nameTag->setProperty("NormalTextColour", "FFFF00FF");
      break;
    case PT::Common::Entity::DoorEntityType:
      nameTag->setProperty("NormalTextColour", "FF550505");
      break;
    case PT::Common::Entity::ItemEntityType:
      nameTag->setProperty("NormalTextColour", "FF0505AA");
      break;
    case PT::Common::Entity::PlayerEntityType:
      nameTag->setProperty("NormalTextColour", "FF05AA05");
      break;
    default:
      nameTag->setVisible(false);
    }

    CEGUI::String name = pcprop->GetPropertyString(pcprop->GetPropertyIndex("Entity Name"));
    CEGUI::Font* font = nameTag->getFont();
    float height = font->getFontHeight(1.3f);
    float width = font->getFormattedTextExtent(name, CEGUI::Rect(0,0,5,5),CEGUI::Centred, 1.1f);
    nameTag->setText(name);
    nameTag->setPosition(CEGUI::UVector2(CEGUI::UDim(0,mouseX-(width/2)), CEGUI::UDim(0,mouseY-25)));
    nameTag->setSize( CEGUI::UVector2(CEGUI::UDim(0,width), CEGUI::UDim(0,height)));
    nameTag->setVisible(true);
  }
  else
  {
    nameTag->setVisible(false);
  }
}

void Cursor::UpdateSelected(iCelPlLayer* pl, iCamera* camera)
{
  selectedEntity = pl->GetHitEntity(camera, mouseX, mouseY);
}

iCelEntity* Cursor::GetSelectedEntity()
{
  return selectedEntity;
}

iMeshWrapper* Cursor::Get3DPointFrom2D(iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector)
{
  return Get3DPointFrom2D(mouseX, mouseY, camera, worldCoord, untransfCoord, sector);
}

iMeshWrapper* Cursor::Get3DPointFrom2D(int x, int y, iCamera* camera, csVector3* worldCoord, csVector3* untransfCoord, iSector** sector)
{
  csVector3 vc, vo, vw;

  csVector2 perspective( x, camera->GetShiftY() * 2 - y );
  vc = camera->InvPerspective( perspective, 1);
  vw = camera->GetTransform().This2Other( vc );

  iSector* camsector = camera->GetSector();

  if ( camsector )
  {
    vo = camera->GetTransform().GetO2TTranslation();
    csVector3 end = vo + (vw-vo)*600;

    csSectorHitBeamResult result = camsector->HitBeamPortals(vo, end);

    csVector3 isect = result.isect;
    iMeshWrapper* sel = result.mesh;

    if (worldCoord != 0)
      *worldCoord = isect;
    if (untransfCoord)
      *untransfCoord = vo + (vw-vo).Unit()/**csQsqrt(dist)*/;

    *sector = result.final_sector;

    return sel;
  }
  return 0;

}
