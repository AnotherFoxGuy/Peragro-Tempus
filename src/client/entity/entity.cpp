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

#include <iengine/mesh.h>

#include <iutil/objreg.h>
#include <iengine/engine.h>
#include <iengine/movable.h>
#include <iengine/sector.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>
#include <propclass/prop.h>

#include "common/reporter/reporter.h"

#include "client/pointer/pointer.h"
#include "common/eventcs/eventmanager.h"
#include "common/eventcs/entityevent.h"
#include "common/eventcs/regionevent.h"
#include "client/data/sector/sectordatamanager.h"
#include "client/data/sector/sector.h"

#include "include/world.h"

#include "client/component/componentmanager.h"

#include "common/util/geomhelper.h"

namespace Client
{
  namespace Entity
  {
    Entity::Entity(Common::Entity::EntityType type) : ::PT::Entity::Entity(type)
    {
      CreateCelEntity();
      sectorName = "World";
    }

    void Entity::Initialize(const iEvent& ev)
    {
      using namespace PT::Events;
      id = EntityHelper::GetEntityID(&ev);
      name = Helper::GetString(&ev, "entityName");
      meshName = Helper::GetString(&ev, "meshName");
      fileName = Helper::GetString(&ev, "fileName");
      Common::Entity::Entity::SetPosition(PT::Events::EntityHelper::GetPosition(&ev));
      ev.Retrieve("rotation", rotation);
      sectorName = "World";

      csRef<iPcProperties> pcprop =
        CEL_QUERY_PROPCLASS_ENT(celEntity, iPcProperties);

      // Add some properties.
      pcprop->SetProperty("Entity Type", (long)type);
      pcprop->SetProperty("Entity ID", (long)id);
      pcprop->SetProperty("Entity Name", name.c_str());
    }

    void Entity::CreateCelEntity()
    {
      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      pl = csQueryRegistry<iCelPlLayer> (obj_reg);

      celEntity = pl->CreateEntity();

      pl->CreatePropertyClass(celEntity, "pcobject.mesh");
      pl->CreatePropertyClass(celEntity, "pcmove.solid");
      pl->CreatePropertyClass(celEntity, "pctools.properties");
    }

    void Entity::SetFullPosition(const WFMath::Point<3>& pos,
                                 float rotation,
                                 const std::string& sector)
    {
      Common::Entity::Entity::SetFullPosition(pos, rotation, sector);

      csRef<iObjectRegistry> obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEngine> engine =  csQueryRegistry<iEngine> (obj_reg);

      if (celEntity.IsValid())
      {
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        csRef<iSector> sec = engine->FindSector(sector.c_str());

        if (!sec.IsValid())
        {
          sec = engine->FindSector("World");
          Report(PT::Debug,
            "Entity: Failed to find sector '%s' switching to default!", sector.c_str());
        }

        if (pcmesh.IsValid() && sec.IsValid())
        {
          csRef<iMeshWrapper> mesh = pcmesh->GetMesh();
          if (mesh.IsValid())
          {
            csRef<iMovable> mov = mesh->GetMovable();
            if (mov.IsValid())
            {
              mov->SetSector(sec);
              mov->SetPosition(VectorHelper::Convert(pos));
              mov->GetTransform ().SetO2T (csYRotMatrix3(rotation));
              mov->UpdateMove();
            }

            csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
            if (pclinmove.IsValid())
            {
              pclinmove->SetOnGround(false);
            }
          }

          return; // success so return.
        }
      }

      Report(PT::Error, "Entity: SetFullPosition failed for %s(%d)!\n",
             name.c_str(), id);
    }

    void Entity::SetMesh(iMeshWrapper* mesh)
    {
      if (celEntity)
      {
        csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcMesh);
        if (!pcmesh)
        {
          Report(PT::Error,  "SetMesh: Failed to get iPcMesh");
          return;
        }

        if (mesh)
          pcmesh->SetMesh(mesh, true);
        else
          pcmesh->CreateNullMesh("ErrorMesh", csBox3(csVector3(0), csVector3(1)));

        csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(celEntity, iPcLinearMovement);
        if (mesh && pclinmove)
        {
          pclinmove->InitCD(mesh, 50.0f);
        }
        this->SetFullPosition();

        // TODO: Move this to playerentity instead?
        if (GetType() == Common::Entity::PlayerEntityType)
        {
          csRef<iObjectRegistry> object_reg =
             PointerLibrary::getInstance()->getObjectRegistry();
          csRef<iWorld> world =  csQueryRegistry<iWorld> (object_reg);
          world->SetMesh(mesh);
        }
      }
      else
        Report(PT::Error,  "SetMesh: Failed to get iCelEntity");
    }

  } // Entity namespace
} // Client namespace
