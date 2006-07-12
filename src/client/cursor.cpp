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

Cursor::Cursor(Client* client)
: client(client)
{
  selent = 0;
}

Cursor::~Cursor()
{
}

void Cursor::Draw()
{
  if (selent.IsValid())
  {
    prevselent = selent;

    csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(selent, iPcMesh);

    nametag = CEL_QUERY_PROPCLASS_ENT(selent, iPcTooltip);
    if (nametag)
      nametag->Show(mousex-20, mousey-25);

    /*
    float fov = client->getG3D()->GetPerspectiveAspect ();
    int color = client->getG3D()->GetDriver2D ()->FindRGB (0, 255, 255);
    csTransform tr_w2c = client->getCamera()->GetTransform ();
    csReversibleTransform tr_o2c = 
      tr_w2c/pcmesh->GetMesh()->GetMovable()->GetFullTransform();

    csBox3 box;
    pcmesh->GetMesh()->GetMeshObject()->GetObjectModel()->GetObjectBoundingBox(box);

    client->getG3D()->BeginDraw(CSDRAW_2DGRAPHICS);

    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(0),tr_o2c*box.GetCorner(1), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(0),tr_o2c*box.GetCorner(2), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(0),tr_o2c*box.GetCorner(4), fov, color);

    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(3),tr_o2c*box.GetCorner(1), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(3),tr_o2c*box.GetCorner(2), fov, color);

    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(5),tr_o2c*box.GetCorner(1), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(5),tr_o2c*box.GetCorner(4), fov, color);

    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(6),tr_o2c*box.GetCorner(2), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(6),tr_o2c*box.GetCorner(4), fov, color);

    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(7),tr_o2c*box.GetCorner(3), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(7),tr_o2c*box.GetCorner(5), fov, color);
    client->getG3D()->DrawLine(tr_o2c*box.GetCorner(7),tr_o2c*box.GetCorner(6), fov, color);
    */
  }
  else if (nametag && prevselent)
  {
    nametag = CEL_QUERY_PROPCLASS_ENT(prevselent, iPcTooltip);
    nametag->Hide();
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