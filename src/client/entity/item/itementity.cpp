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

#include "itementity.h"

PtItemEntity::PtItemEntity() : PtEntity(PtEntity::ItemEntity) 
{
  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  pl = CS_QUERY_REGISTRY (obj_reg, iCelPlLayer);
}

void PtItemEntity::Create()
{
  ClientItem* item = PointerLibrary::getInstance()->getItemManager()->GetItemById(itemid);
  if(item)
  {
    name = item->GetName();
    meshname = item->GetMeshName();

    CreateCelEntity();

    char buffer[1024];
    sprintf(buffer, "%s:%d:%d", name.GetData(), type, id);
    celentity->SetName(buffer);

    // Load and assign the mesh to the entity.
    csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
    pcmesh->SetMesh(meshname.GetData(), item->GetFileName().GetData());
    pl->CreatePropertyClass(celentity, "pclinearmovement");
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

    pclinmove->InitCD(
      csVector3(0.5f,0.8f,0.5f),
      csVector3(0.5f,0.8f,0.5f),
      csVector3(0,0,0));

    iSector* sector = engine->FindSector(sectorname);
    pclinmove->SetPosition(pos,0,sector);
  }
  else
    printf("PtItemEntity: ERROR Couldn't find mesh for item %s!\n", name.GetData());

}
