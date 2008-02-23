#include <crystalspace.h>

#include "maptile.h"
#include "world.h"
#include "interiormanager.h"
#include "interior.h"

MapTile::MapTile(int x0, int z0, const std::string& fileName, World* world): Level(world), x(x0), z(z0)
{
  xbase = x * TILESIZE;
  zbase = z * TILESIZE;

  printf("Loading tile %d,%d (%.2f, %.2f)\n",x0,z0,xbase,zbase);

  char buffer[256];
  sprintf(buffer, "instances-%d-%d",x,z);
  regionName = buffer;

  OpenFile("/peragro/tiles/", fileName);

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  sector = engine->FindSector("World");
}

MapTile::~MapTile()
{
  printf("Unloading tile %d,%d\n", x, z);
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
      rc = loader->Load(meshNode, instances, false, false, 0, 0, missingData, true);
    else
      rc = loader->Load(meshNode, instances, true, false, 0, 0, missingData, true);
    if (!rc.success) return;

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
