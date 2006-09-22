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

#include "csgeom/polymesh.h"
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

#include "common/entity/entity.h"

Cursor::Cursor(Client* client)
: client(client)
{
  selent = 0;

  CEGUI::WindowManager* winMgr = client->GetGuiManager()->GetCEGUI()->GetWindowManagerPtr();
  CEGUI::Window* root = winMgr->getWindow("Root");
  nametag = winMgr->createWindow("Peragro/Entity", "NameTag");
  root->addChildWindow(nametag);
}

Cursor::~Cursor()
{
}

void Cursor::Draw()
{
  if (selent.IsValid())
  {
    csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(selent, iPcProperties);
    if (!pcprop) return;

    int type = pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type"));
    switch(type)
    {
    case Entity::PlayerEntity:
      nametag->setProperty("NormalTextColour", "FF05AA05");
      break;
    case Entity::NPCEntity:
      nametag->setProperty("NormalTextColour", "FFFF00FF");
      break;
    case Entity::DoorEntity:
      nametag->setProperty("NormalTextColour", "FF550505");
      break;
    case Entity::ItemEntity:
      nametag->setProperty("NormalTextColour", "FF0505AA");
      break;
    default:
      nametag->setVisible(false);
    }
    
    CEGUI::String name = pcprop->GetPropertyString(pcprop->GetPropertyIndex("Entity Name"));
    CEGUI::Font* font = nametag->getFont();
    float height = font->getFontHeight(1.3f);
    float width = font->getFormattedTextExtent(name, CEGUI::Rect(0,0,5,5),CEGUI::Centred, 1.1f);
    nametag->setText(name);
    nametag->setPosition(CEGUI::UVector2(CEGUI::UDim(0,mousex-(width/2)), CEGUI::UDim(0,mousey-25)));
    nametag->setSize( CEGUI::UVector2(CEGUI::UDim(0,width), CEGUI::UDim(0,height)));
    nametag->setVisible(true);
  }
  else
  {
    nametag->setVisible(false);
  }
}

void Cursor::MouseMove(iCelPlLayer* pl, iCamera* camera, int x, int y)
{
  selent = pl->GetHitEntity(camera, x, y);
  mousex = x;
  mousey = y;
}

iCelEntity* Cursor::getSelectedEntity()
{
  return selent;
}

iMeshWrapper* Cursor::Get3DPointFrom2D(int x, int y, iCamera* camera, csVector3 * worldCoord, csVector3 * untransfCoord)
{
  cdsys = CS_QUERY_REGISTRY(client->GetObjectRegistry(), iCollideSystem);
  if (!cdsys) return 0;

  csVector3 vc, vo, vw;

  csVector2 perspective( x, camera->GetShiftY() * 2 - y );
  vc = camera->InvPerspective( perspective, 1);
  vw = camera->GetTransform().This2Other( vc );

  iSector* sector = camera->GetSector();

  if ( sector )
  {
    vo = camera->GetTransform().GetO2TTranslation();
    csVector3 isect;
    csVector3 end = vo + (vw-vo)*600;
    csIntersectingTriangle closest_tri;
    iMeshWrapper* sel = 0;
    float dist = csColliderHelper::TraceBeam (cdsys, sector,
      vo, end, true, closest_tri, isect, &sel);

    if (worldCoord != 0)
      *worldCoord = isect;
    if (untransfCoord)
      *untransfCoord = vo + (vw-vo).Unit()*csQsqrt(dist);
    return sel;
  }
  return 0;

}