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

#include "npcentity.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {

    NpcEntity::NpcEntity(const Events::EntityAddEvent& ev) : CharacterEntity(ev)
    {
      Create();
    }

    void NpcEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
      csRef<iVFS> vfs =  csQueryRegistry<iVFS> (obj_reg);

      CreateCelEntity();

      char buffer[32];
      cs_snprintf(buffer, 32, "npc_%d", id);
      celEntity->SetName(buffer);

      pl->CreatePropertyClass(celEntity, "pcmove.actor.standard");
      pl->CreatePropertyClass(celEntity, "pcmove.linear");

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celEntity,
        iPcLinearMovement);

      // Load and assign the mesh to the entity.
      vfs->ChDir("/cellib/objects/");
      if (!pcmesh->SetMesh(meshName.c_str(), "/peragro/meshes/all.xml"))
      {
        Report(PT::Error, "PtNpcEntity: Failed to load mesh: %s",
          meshName.c_str());
        pcmesh->CreateEmptyGenmesh("EmptyGenmesh");
      }

      // Forcing the speed on the Cal3d mesh, so it will go in idle animation.
      csRef<iSpriteCal3DState> sprcal3d =
        scfQueryInterface<iSpriteCal3DState> (pcmesh->GetMesh()->GetMeshObject());
      if (sprcal3d) sprcal3d->SetVelocity(0);

      pclinmove->InitCD(csVector3(0.5f,0.8f,0.5f), csVector3(0.5f,0.8f,0.5f),
        csVector3(0,0,0));

      iSector* sector = engine->FindSector(sectorName.c_str());
      if (!sector)
      {
        sector = engine->FindSector("Default_Sector");
        Report(PT::Warning,
        "NpcEntity: Failed to find sector switching to default!");
      }
      if (sector) pclinmove->SetPosition(pos,0,sector);
      else Report(PT::Error,
        "NpcEntity: Failed to set position, sector '%s' unknown!",
        sectorName.c_str());
    }

    void NpcEntity::Interact()
    {
      using namespace PT::Events;
      InterfaceInteract* interfaceEvent = new InterfaceInteract();
      interfaceEvent->entityId              = id;
      interfaceEvent->actions               = "Talk";
      PointerLibrary::getInstance()->getEventManager()->AddEvent(interfaceEvent);
    }
  }
}
