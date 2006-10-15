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

#include "doorentity.h"

PtDoorEntity::PtDoorEntity() : PtEntity(PtEntity::DoorEntity)
{
  open=false;
  locked=false;

  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  pl = CS_QUERY_REGISTRY (obj_reg, iCelPlLayer);
}

void PtDoorEntity::Create()
{
  CreateCelEntity();

  celentity->SetName(name.GetData());

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  csRef<iMeshWrapper> doormesh = engine->FindMeshObject(meshname.GetData());
  if (doormesh)
    pcmesh->SetMesh(doormesh);
  else
  {
    printf("PtDoorEntity: ERROR Couldn't find mesh for door %s!\n", name.GetData());
    return;
  }

  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celentity, iPcProperties);
  pcprop->SetProperty("Door Open", open);
  pcprop->SetProperty("Door Locked", locked);

  pl->CreatePropertyClass(celentity, "pcquest");
  csRef<iPcQuest> pcquest = CEL_QUERY_PROPCLASS_ENT(celentity, iPcQuest);
  celQuestParams parameters;
  pcquest->NewQuest("PropDoor",parameters);
  pcquest->GetQuest()->SwitchState("closed");
  
}