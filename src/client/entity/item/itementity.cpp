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

#include "client/reporter/reporter.h"

PtItemEntity::PtItemEntity() : PtEntity(PtEntity::ItemEntity) 
{
  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine =  csQueryRegistry<iEngine> (obj_reg);
  pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
}

void PtItemEntity::Create()
{
  PT::Data::Item* item = PointerLibrary::getInstance()->getItemManager()->GetItemById(itemid);
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
    if (!pcmesh->SetMesh(meshname.GetData(), item->GetFileName().GetData()))
    {
      Report(PT::Error,  "PtItemEntity: Failed to load mesh: %s", meshname.GetData());
      pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
    }
    pl->CreatePropertyClass(celentity, "pcmove.linear");
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

    pclinmove->InitCD(
      csVector3(0.5f,0.8f,0.5f),
      csVector3(0.5f,0.8f,0.5f),
      csVector3(0,0,0));

    iSector* sector = engine->FindSector(sectorname);
    pclinmove->SetPosition(pos,0,sector);
  }
  else
    Report(PT::Error, "PtItemEntity: Couldn't find mesh for item %s!\n", name.GetData());

}
