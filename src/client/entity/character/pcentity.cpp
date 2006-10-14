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

void PtPcEntity::Create()
{
  csRef<iCelEntity> entity = pl->CreateEntity();

  char buffer[32];
  cs_snprintf(buffer, 32, "player_%d", ent->getId());
  entity->SetName(buffer);

  pl->CreatePropertyClass(entity, "pcmesh");
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  csRef<iPcLinearMovement> pclinmove;
  pl->CreatePropertyClass(entity, "pcsolid");
  csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(entity, iPcSolid);

  pl->CreatePropertyClass(entity, "pcactormove");
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT (entity, iPcActorMove);
  pcactormove->SetMovementSpeed (2.0f);
  pcactormove->SetRunningSpeed (5.0f);
  pcactormove->SetRotationSpeed (1.75f);
  pcactormove->SetJumpingVelocity (6.31f);

  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(*ent->getMesh(), "/peragro/meshes/all.xml");

  // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
  csRef<iSpriteCal3DState> sprcal3d =
    SCF_QUERY_INTERFACE (pcmesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
  if (sprcal3d) sprcal3d->SetVelocity(0);

  pl->CreatePropertyClass(entity, "pclinearmovement");
  pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region)
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  pl->CreatePropertyClass(entity, "pcproperties");
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
  pcprop->SetProperty("Entity Type", (long)ent->getType());
  pcprop->SetProperty("Entity ID", (long)ent->getId());
  pcprop->SetProperty("Entity Name", *ent->getName());

}