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

void PtEntity::CreateCelEntity()
{
  csRef<iCelEntity> entity = pl->CreateEntity();

  // Assign the created entity to the local var.
  celentity = entity;

  pl->CreatePropertyClass(entity, "pcdefaultcamera");
  pl->CreatePropertyClass(entity, "pcmesh");
  pl->CreatePropertyClass(entity, "pcactormove");
  pl->CreatePropertyClass(entity, "pcsolid");
  pl->CreatePropertyClass(entity, "pclinearmovement");
  pl->CreatePropertyClass(entity, "pcproperties");

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
  csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(entity, iPcSolid);
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
  
  // Load and assign the mesh to the entity.
  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(meshname.GetData(), "/peragro/meshes/all.xml");

  // Place the entity in the world.
  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region)
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  // Add some properties.
  pcprop->SetProperty("Entity Type", (long)type);
  pcprop->SetProperty("Entity ID", (long)id);
  pcprop->SetProperty("Entity Name", name);
}