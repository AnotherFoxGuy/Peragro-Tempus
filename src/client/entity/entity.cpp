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

#include "entity.h"

#include "client/pointer/pointer.h"
#include "client/event/entityevent.h"
#include "common/data/sectordatamanager.h"
#include "common/data/sector.h"

namespace PT
{
  namespace Entity
  {
    Entity::Entity(const Events::EntityAddEvent& ev)
    {
      id = ev.entityId;
      type = ev.entityType;
      name = ev.entityName.c_str();
      meshname = ev.meshName.c_str();
      ///@todo This is an ugly hack. The server seems to send some impossible sector id from time to time.
      PT::Data::Sector* sector = PointerLibrary::getInstance()->getSectorDataManager()->GetSectorById(ev.sectorId);
      if (sector) sectorname = sector->GetName().c_str();
      //End of ugly hack
      pos = ev.position;
      celentity = 0;
    }

    void Entity::CreateCelEntity()
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      celentity = pl->CreateEntity();

      pl->CreatePropertyClass(celentity, "pcobject.mesh");
      pl->CreatePropertyClass(celentity, "pcmove.solid");
      pl->CreatePropertyClass(celentity, "pctools.properties");

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celentity, iPcMesh);
      //TODO: Why is this part commented out and not removed?
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
  }
}
