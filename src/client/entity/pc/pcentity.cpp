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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"

PtPcEntity::PtPcEntity() : PtCharacterEntity(PlayerEntity)
{
  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine =  csQueryRegistry<iEngine> (obj_reg);
  pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
  vfs =  csQueryRegistry<iVFS> (obj_reg);
  is_own_entity = false;
}

void PtPcEntity::Create()
{
  CreateCelEntity();

  char buffer[32];
  cs_snprintf(buffer, 32, "player_%d", id);
  celentity->SetName(buffer);

  pl->CreatePropertyClass(celentity, "pcmove.actor.standard");
  pl->CreatePropertyClass(celentity, "pcmove.linear");

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);

  // Load and assign the mesh to the entity.
  vfs->ChDir("/cellib/objects/");
  if (!pcmesh->SetMesh(meshname.GetData(), "/peragro/meshes/all.xml"))
  {
    Report(PT::Error,  "PtPcEntity: Failed to load mesh: %s", meshname.GetData());
    pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
  }

  // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
  csRef<iSpriteCal3DState> sprcal3d =
    scfQueryInterface<iSpriteCal3DState> (pcmesh->GetMesh()->GetMeshObject());
  if (sprcal3d) sprcal3d->SetVelocity(0);

  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));

  iSector* sector =  0;
  sector = engine->FindSector(sectorname);
  if (sector)
    pclinmove->SetPosition(pos,0,sector);
  else
    Report(PT::Error, "PtPcEntity: Failed to set position, sector '%s' unknown!", sectorname.GetData());

  GetEquipment()->ConstructMeshes();

  if (IsOwnEntity())
  {
    pl->CreatePropertyClass(celentity, "pccamera.old");
  }

}

void PtPcEntity::Interact()
{
  //if (IsOwnEntity()) return;

  using namespace PT::Events;
  InterfaceInteract* interfaceEvent = new InterfaceInteract();
  interfaceEvent->entityId              = id;
  interfaceEvent->actions               = "trade,attack,party";
  PointerLibrary::getInstance()->getEventManager()->AddEvent(interfaceEvent);
}
