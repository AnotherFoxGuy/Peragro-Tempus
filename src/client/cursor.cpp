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
#include "igeom/objmodel.h"
#include "imesh/object.h"
#include "ivideo/graph3d.h"
#include "ivideo/graph2d.h"

#include "physicallayer/propclas.h"
#include "propclass/mesh.h"

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
    csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(selent, iPcMesh);

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
  }
}

void Cursor::MouseMove(iCelPlLayer* pl, iCamera* camera, int x, int y)
{
  selent = pl->GetHitEntity(camera, x, y);
}

iCelEntity* Cursor::getSelectedEntity()
{
  return selent;
}
