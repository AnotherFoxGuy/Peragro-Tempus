/*
    Copyright (C) 2007 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "teleportentity.h"

#include <iengine/engine.h>
#include <iengine/mesh.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>

#include "client/data/teleportdatamanager.h"
#include "client/data/teleport.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{
  namespace Entity
  {
    TeleportEntity::TeleportEntity(const iEvent& ev)
    {
      Create();
    }

    void TeleportEntity::Create()
    {
    csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      PT::Data::Teleport* teleport =
        PointerLibrary::getInstance()->getTeleportDataManager()
          ->GetTeleportById(teleportId);

      if (teleport)
      {
        name = teleport->GetName();
        meshName = teleport->GetMeshName();

        CreateCelEntity();

        celEntity->SetName(name.c_str());

        // Load and assign the mesh to the entity.
        csRef<iPcMesh> pcMesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        csRef<iMeshWrapper> teleportMesh = engine->FindMeshObject(meshName.c_str());

        if (teleportMesh) pcMesh->SetMesh(teleportMesh);
        else
        {
          Report(PT::Warning,
            "TeleportEntity: Couldn't find mesh for teleport %s!", name.c_str());
          return;
        }
      }
      else Report(PT::Error,
        "TeleportEntity: Couldn't find mesh for teleport %d!\n", id);
    }

    void TeleportEntity::Interact()
    {
    }
  }
}
