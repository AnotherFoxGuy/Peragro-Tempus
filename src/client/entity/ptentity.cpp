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

#include "ptentity.h"

#include "client/pointer/pointer.h"

void PtEntity::CreateCelEntity()
{
  csRef<iCelEntity> entity = pl->CreateEntity();
  celentity = entity;

  pl->CreatePropertyClass(celentity, "pcobject.mesh");
  pl->CreatePropertyClass(celentity, "pcmove.solid");
  pl->CreatePropertyClass(celentity, "pctools.properties");

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  //csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
  //csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(celentity, iPcSolid);
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(celentity, iPcProperties);

  // Place the entity in the world.
  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region.IsValid())
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  // Add some properties.
  pcprop->SetProperty("Entity Type", (long)type);
  pcprop->SetProperty("Entity ID", (long)id);
  pcprop->SetProperty("Entity Name", name);
}
