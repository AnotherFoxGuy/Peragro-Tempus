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

#include "client/reporter/reporter.h"

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
      name = ev.entityName;
      meshName = ev.meshName;
      ///@todo This is an ugly hack. The server seems to send some impossible
      ///sector id from time to time.
      PT::Data::Sector* sector = PointerLibrary::getInstance()->
        getSectorDataManager()->GetSectorById(ev.sectorId);
      if (sector) sectorName = sector->GetName();
      //End of ugly hack
      pos = ev.position;
      celEntity = 0;
    }

    void Entity::CreateCelEntity()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl = csQueryRegistry<iCelPlLayer> (obj_reg);

      celEntity = pl->CreateEntity();

      pl->CreatePropertyClass(celEntity, "pcobject.mesh");
      pl->CreatePropertyClass(celEntity, "pcmove.solid");
      pl->CreatePropertyClass(celEntity, "pctools.properties");

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
      csRef<iPcProperties> pcprop =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcProperties);

      // Add some properties.
      pcprop->SetProperty("Entity Type", (long)type);
      pcprop->SetProperty("Entity ID", (long)id);
      pcprop->SetProperty("Entity Name", name.c_str());
    }
  }
}
