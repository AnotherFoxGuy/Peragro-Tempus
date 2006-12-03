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

#include "pcentity.h"

PtPcEntity::PtPcEntity() : PtCharacterEntity(PlayerEntity)
{
  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine =  csQueryRegistry<iEngine> (obj_reg);
  pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
  vfs =  csQueryRegistry<iVFS> (obj_reg);
}

void PtPcEntity::Create()
{
  CreateCelEntity();

  char buffer[32];
  cs_snprintf(buffer, 32, "player_%d", id);
  celentity->SetName(buffer);

  if (IsOwnEntity())
  {
    pl->CreatePropertyClass(celentity, "pcdefaultcamera");
  }

  pl->CreatePropertyClass(celentity, "pcactormove");
  pl->CreatePropertyClass(celentity, "pclinearmovement");

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

  // Load and assign the mesh to the entity.
  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(meshname.GetData(), "/peragro/meshes/all.xml");

  // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
  csRef<iSpriteCal3DState> sprcal3d =
     scfQueryInterface<iSpriteCal3DState> (pcmesh->GetMesh()->GetMeshObject());
  if (sprcal3d) sprcal3d->SetVelocity(0);

  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));

  iSector* sector = engine->FindSector(sectorname);
  pclinmove->SetPosition(pos,0,sector);

  GetEquipment()->ConstructMeshes();
}
