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
#include "client.h"

#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/movable.h"
#include "iengine/sector.h"
//#include "imesh/objmodel.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"
#include "csqsqrt.h"

#include "physicallayer/propclas.h"
#include "propclass/mesh.h"
#include "propclass/tooltip.h"
#include "propclass/prop.h"


#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"
#include "client/gui/guimanager.h"

#include "client/pointer/pointer.h"

#include "client/entity/entitymanager.h"

Cursor::Cursor(PT::Client* client)
: client(client)
{
  selectedEntity = 0;

  CEGUI::WindowManager* winMgr = PointerLibrary::getInstance()->getGUIManager()->GetCEGUI()->GetWindowManagerPtr();
  CEGUI::Window* root = winMgr->getWindow("Root");
  nameTag = winMgr->createWindow("Peragro/Entity", "NameTag");
  root->addChildWindow(nameTag);
}

Cursor::~Cursor()
{
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
    case PT::Entity::PCEntityType:
      nameTag->setProperty("NormalTextColour", "FF05AA05");
      break;
    case PT::Entity::NPCEntityType:
      nameTag->setProperty("NormalTextColour", "FFFF00FF");
      break;
    case PT::Entity::DoorEntityType:
      nameTag->setProperty("NormalTextColour", "FF550505");
      break;
    case PT::Entity::ItemEntityType:
      nameTag->setProperty("NormalTextColour", "FF0505AA");
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

void Cursor::MouseMove(int x, int y)
{
  mouseX = x;
  mouseY = y;
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
