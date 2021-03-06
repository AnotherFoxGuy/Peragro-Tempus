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

#include "equipeditem.h"

#include <iutil/string.h>
#include <csutil/ref.h>
#include <csutil/csstring.h>

#include <iutil/objreg.h>
#include <iengine/mesh.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <propclass/mesh.h>

#include "client/serversetup/serversetupmanager.h" //remove

#include "client/data/item/itemdatamanager.h"
#include "client/data/item/item.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

namespace PT
{
  namespace Entity
  {

    EquipedItem::EquipedItem(::Client::Entity::Entity* entity, unsigned int slotId, unsigned int itemId, const std::string& meshname, const std::string& meshfile)
    {
      this->id         = itemId;
      this->slotId     = slotId;
      this->entity     = entity;
      this->itementity = 0;
      this->meshname   = meshname;
      this->meshfile   = meshfile;
    }

    void EquipedItem::ConstructMesh()
    {
      if (itementity) return;
      if (!entity->GetCelEntity()) return;

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      if (!obj_reg) return;
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);
      if (!pl.IsValid()) return;

      csRef<iCelEntity> itement = pl->CreateEntity();
      this->itementity = itement;
      pl->CreatePropertyClass(itementity, "pcobject.mesh");
      csRef<iPcMesh> itempcmesh = CEL_QUERY_PROPCLASS_ENT(itementity, iPcMesh);
      itempcmesh->SetMesh(meshname.c_str(), meshfile.c_str());

      // Get the player's mesh.
      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcMesh);

      iMeshWrapper* mesh = itempcmesh->GetMesh();
      iMeshWrapper* parent = pcmesh->GetMesh();
      if (parent && mesh)
      {
        // Attach the item.
        if (pcmesh->AttachSocketMesh(GetSocketName(slotId).c_str(), mesh))
        {
          Report(PT::Debug, "Equipment attached!");
          return;
        }
      }

      Report(PT::Error, "Failed to construct mesh %d!", id);
    }

    void EquipedItem::DestructMesh()
    {
      if (!itementity) return;

      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcMesh);

      if ( pcmesh->GetMesh() && pcmesh->DetachSocketMesh(GetSocketName(slotId).c_str()) )
      {
        pl->RemoveEntity(itementity);
        itementity = 0;
        Report(PT::Debug, "Equipment destroyed!");
        return;
      }

      Report(PT::Error, "Failed to destruct mesh %d!", id);
    }

    std::string EquipedItem::GetSocketName(unsigned int slotid)
    {
      std::string socketName;

      switch(slotid)
      {
      case 0:
        socketName = "righthand";
        break;
      default: Report(PT::Error, "Unknown slot with ID %d !", slotid); return "";
      }

      return socketName;
    }
  }
}
