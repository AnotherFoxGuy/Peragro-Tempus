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

#include "maptile.h"

#include <cssysdef.h>
#include <iutil/document.h>
#include <iengine/collection.h>
#include <iengine/sector.h>
#include <iengine/movable.h>
#include <iengine/portalcontainer.h>
#include <iengine/mesh.h>
#include <cstool/collider.h>
#include <imesh/object.h>

#include "world.h"
#include "interiormanager.h"
#include "interior.h"

#include "client/reporter/reporter.h"

namespace PT
{
  MapTile::MapTile(int x0, int z0, const std::string& fileName, World* world): Level(world), x(x0), z(z0)
  {
    xbase = x * TILESIZE;
    zbase = z * TILESIZE;

    Report(PT::Debug, "Loading tile %d,%d (%.2f, %.2f)",x0,z0,xbase,zbase);

    char buffer[256];
    sprintf(buffer, "instances-%d-%d",x,z);
    regionName = buffer;

    OpenFile("/peragro/art/tiles/", fileName);

    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    sector = engine->FindSector("World");
  }

  MapTile::~MapTile()
  {
    Report(PT::Debug, "Unloading tile %d,%d", x, z);
  }

  void MapTile::LoadInstance(iDocumentNode* meshNode)
  {
    //{//to measure time
    //CS::MeasureTime lTimeMeasurer ("Creating instance");

    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    csRef<iLoader> loader = csQueryRegistry<iLoader> (object_reg);

    // Load our mesh into the instances region and ONLY look for 
    // meshfactories in our own factory list.
    // If it's a portal allow it to look for sectors beyond the region.
    csLoadResult rc;
    if (meshNode && meshNode->GetNode("portals").IsValid())
      rc = loader->Load(meshNode, instances, false, false, 0, 0, missingData, KEEP_ALL);
    else
      rc = loader->Load(meshNode, instances, true, false, 0, 0, missingData, KEEP_ALL);
    if (!rc.success) 
    {
      printf("BLAAAH %s\n", meshNode->GetNode("portals").IsValid()? "portal":"");
      return;
    }

    csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper>(rc.result);
    csRef<iLight> light = scfQueryInterface<iLight>(rc.result);
    if (mesh && sector)
    {
      // If it's a portal we have to do a hardtransform.
      csRef<iPortalContainer> pc = scfQueryInterface<iPortalContainer>(mesh->GetMeshObject ());
      if (pc.IsValid())
      {
        csReversibleTransform trans;
        trans.Identity();
        trans.SetOrigin(csVector3(xbase, 0, zbase));
        mesh->HardTransform(trans);
      }
      else
      {
        csVector3 pos = mesh->GetMovable()->GetPosition();
        pos.x += xbase; pos.z += zbase;
        mesh->GetMovable()->SetPosition(pos);

        // Create collider.
        csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem> (object_reg);
        csColliderHelper::InitializeCollisionWrapper (cdsys, mesh);
      }

      mesh->GetMovable()->SetSector(sector);
      mesh->GetMovable()->UpdateMove();
    }
    else if (light && sector)
    {
      iLightList* ll = sector->GetLights ();
      ll->Add (light);
      light->DecRef();
      csVector3 pos = light->GetMovable()->GetPosition();
      pos.x += xbase; pos.z += zbase;
      light->GetMovable()->SetPosition(pos);
      light->GetMovable()->UpdateMove();
    }
    //}//to measure time
  }

  void MapTile::SetVisible(bool visible)
  {
    if (this->visible == visible) return;
    this->visible = visible;
    if (!instances.IsValid()) return;

    csRef<iObjectIterator> it = instances->QueryObject()->GetIterator();
    while (it->HasNext())
    {
      iObject* current = it->Next();
      csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper>(current);
      if (mesh)
      {
        if (visible) mesh->SetFlagsRecursive(CS_ENTITY_INVISIBLE, 0);
        else mesh->SetFlagsRecursive(CS_ENTITY_INVISIBLE);
      }
    } // end while
  } // end SetVisible()

} // PT namespace
