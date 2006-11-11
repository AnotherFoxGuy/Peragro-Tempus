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

#include "mountentity.h"

PtMountEntity::PtMountEntity() : PtCharacterEntity(MountEntity)
{
  // Get the pointers to some common utils.
  this->obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  pl = CS_QUERY_REGISTRY (obj_reg, iCelPlLayer);
  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  mounted = false;
}

void PtMountEntity::Create()
{
  CreateCelEntity();

  char buffer[32];
  cs_snprintf(buffer, 32, "mount_%d", id);
  celentity->SetName(buffer);

  pl->CreatePropertyClass(celentity, "pcactormove");
  pl->CreatePropertyClass(celentity, "pclinearmovement");

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);

  // Load and assign the mesh to the entity.
  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(meshname.GetData(), "/peragro/meshes/all.xml");

  // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
  csRef<iSpriteCal3DState> sprcal3d =
    SCF_QUERY_INTERFACE (pcmesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
  if (sprcal3d) sprcal3d->SetVelocity(0);

  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));

  iSector* sector = engine->FindSector(sectorname);
  pclinmove->SetPosition(pos,0,sector);
}

void PtMountEntity::Mount(PtEntity* player)
{
  if(mounted) return;
  if(!player->GetCelEntity()) return;

  bool on_ground;
  float speed, rot, avel;
  csVector3 pos, vel, wvel;
  iSector* sector;

  PtPcEntity* ptplayer = static_cast<PtPcEntity*>(player);

  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ptplayer->GetCelEntity(), iPcLinearMovement);
  csRef<iPcLinearMovement> mtlinmove = CEL_QUERY_PROPCLASS_ENT(celentity, iPcLinearMovement);
  csRef<iPcMesh> mtmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);

  // Anchor the player.
  mtlinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
  pos.y += 1.0f;
  pclinmove->SetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
  pclinmove->SetAnchor(mtmesh);

  mounted = true;
}

void PtMountEntity::UnMount(PtEntity* player)
{
  if(!mounted) return;
  if(!player->GetCelEntity()) return;
  // Some hacky stuff untill i find the right way.

  bool on_ground;
  float speed, rot, avel;
  csVector3 pos, vel, wvel;
  iSector* sector;

  PtPcEntity* ptplayer = static_cast<PtPcEntity*>(player);

  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(ptplayer->GetCelEntity(), iPcLinearMovement);

  // Anchor the player.
  pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
  pos.y -= 1.0f;
  pos.x -= 1.0f;
  pclinmove->SetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
  pclinmove->SetAnchor(0);

  mounted = false;
}
