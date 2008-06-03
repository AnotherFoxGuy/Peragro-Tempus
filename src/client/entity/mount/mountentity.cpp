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

#include <imesh/object.h>
#include <iengine/mesh.h>
#include <imesh/spritecal3d.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>

#include "client/entity/player/playerentity.h"

#include "common/reporter/reporter.h"
#include "client/event/entityevent.h"
#include "client/network/network.h"
#include "common/network/entitymessages.h"

namespace PT
{
  namespace Entity
  {

    MountEntity::MountEntity(const iEvent& ev) :
      CharacterEntity(ev)
    {
      mounted = false;

      Create();
    }

    void MountEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine = csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl = csQueryRegistry<iCelPlLayer> (obj_reg);
      csRef<iVFS> vfs = csQueryRegistry<iVFS> (obj_reg);

      CreateCelEntity();

      csString buffer;
      buffer.Format("mount_%d", id);
      celEntity->SetName(buffer);

      pl->CreatePropertyClass(celEntity, "pcmove.actor.standard");
      pl->CreatePropertyClass(celEntity, "pcmove.linear");

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);

      // Load and assign the mesh to the entity.
      vfs->ChDir("/cellib/objects/");
      if (!pcmesh->SetMesh(meshName.c_str(), "/peragro/meshes/all.xml"))
      {
        Report(PT::Error,  "PtMountEntity: Failed to load mesh: %s",
          meshName.c_str());
        pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
      }

      // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
      csRef<iSpriteCal3DState> sprcal3d =
        scfQueryInterface<iSpriteCal3DState> (pcmesh->GetMesh()->GetMeshObject());

      if (sprcal3d) sprcal3d->SetVelocity(0);

      pclinmove->InitCD(csVector3(0.5f,0.8f,0.5f), csVector3(0.5f,0.8f,0.5f),
        csVector3(0,0,0));

      SetFullPosition(pos, rot, sectorName.c_str());
    }

    void MountEntity::Mount(Entity* player)
    {
      if (mounted) return;
      if (!player->GetCelEntity()) return;

      bool on_ground;
      float speed, rot, avel;
      csVector3 pos, vel, wvel;
      iSector* sector;

      PcEntity* ptplayer = static_cast<PcEntity*>(player);

      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(ptplayer->GetCelEntity(), iPcLinearMovement);
      csRef<iPcLinearMovement> mtlinmove =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
      csRef<iPcMesh> mtmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);

      // Anchor the player.
      mtlinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      pos.y += 1.0f;
      on_ground = true;
      pclinmove->SetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      pclinmove->SetAnchor(mtmesh);

      ptplayer->PlayAnimation("horse_mount", true, true);

      mounted = true;
      ptplayer->SetHasMount(true);
    }

    void MountEntity::UnMount(Entity* player)
    {
      if (!mounted) return;
      if (!player->GetCelEntity()) return;

      bool on_ground;
      float speed, rot, avel;
      csVector3 pos, vel, wvel;
      iSector* sector;

      PcEntity* ptplayer = static_cast<PcEntity*>(player);

      csRef<iPcLinearMovement> pclinmove =
        CEL_QUERY_PROPCLASS_ENT(ptplayer->GetCelEntity(), iPcLinearMovement);

      // Anchor the player.
      pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      pos.y -= 1.0f;
      pos.x -= 1.0f;
      pclinmove->SetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      pclinmove->SetAnchor(0);

      ptplayer->PlayAnimation("idle", true, true);

      mounted = false;
      ptplayer->SetHasMount(false);
    }

    void MountEntity::Interact()
    {
      if (!mounted)
      {
        MountRequestMessage msg;

        msg.setMountEntityId(id);

        PointerLibrary::getInstance()->getNetwork()->send(&msg);

        Report(PT::Notify, "OnMouseDown: Mounting.");
      }
      else
      {
        UnmountRequestMessage msg;

        msg.setMountEntityId(id);

        PointerLibrary::getInstance()->getNetwork()->send(&msg);

        Report(PT::Notify, "OnMouseDown: UnMounting.");
      }
    }
  }
}
