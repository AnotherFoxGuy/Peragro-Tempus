#include <crystalspace.h>

#include "interior.h"

InteriorFactory::InteriorFactory(const std::string& fileName, World* world) : scfImplementationType (this), Level(world)
{
  this->fileName = fileName;

  OpenFile("", fileName);
}

InteriorFactory::~InteriorFactory() 
{
  printf("Unloading InteriorFactory %s\n", fileName.c_str());
}

void InteriorFactory::LoadInstance(iDocumentNode* meshNode) 
{ 
  instanceNodes.Push(meshNode); 
}

//--------------------------------------------------------------

Interior::Interior(iDocumentNode* interiorNode, InteriorFactory* factory) : scfImplementationType (this) 
{
  this->interiorNode = interiorNode->GetNode("interior");
  this->factory = factory;
  this->interiorName = this->interiorNode->GetAttributeValue("name");;
  loading = false;
  finished = false;
}

Interior::~Interior() 
{
  printf("Unloading Interior %s\n", interiorName.c_str());

  csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);

  if (region.IsValid())
  {
    region->DeleteAll();
    iRegionList* list = engine->GetRegions();
    list->Remove(region);
  }

  // Remove callback.
  if (cb.IsValid())
  {
    engine->RemoveEngineFrameCallback(cb);
  }
}

void Interior::Load() 
{
  if (loading) return;

  printf("PT: Loading Interior %s\n", interiorName.c_str());
  loading = true;

  csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);

  region = engine->CreateRegion(interiorName.c_str());
  sector = engine->CreateSector(interiorName.c_str());
  region->Add(sector->QueryObject());

  // Wait for the resources to be loaded.
  if (!cb.IsValid())
  {
    cb.AttachNew(new FrameCallBack(this));
    engine->AddEngineFrameCallback(cb);
  }

} // end Load()

void Interior::LoadInstance(iDocumentNode* meshNode) 
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);
  csRef<iLoader> loader = csQueryRegistry<iLoader> (factory->object_reg);

  csRef<iMeshWrapper> portalMesh;
  csRef<iDocumentNodeIterator> nodes = interiorNode->GetNodes("portal");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csString offsetBool = current->GetAttributeValue("UseThisForPosition");
    if (offsetBool.CompareNoCase("true"))
    {
      std::string portalName = current->GetContentsValue();
      csRef<iMeshWrapper> mesh = engine->FindMeshObject(portalName.c_str());
      if (IsPortal(mesh)) 
      {
        portalMesh = mesh;
        break;
      }
    }
  }
  if (!portalMesh.IsValid()) return;

  float portalRot = GetPortalYRotation(portalMesh);
  csVector3 portalCenter = GetPortalPosition(portalMesh);

  // Load our mesh into the instances region and ONLY look for 
  // meshfactories in our own factory list.
  // If it's a portal allow it to look for sectors beyond the region.
  csLoadResult rc;
  if(meshNode && meshNode->GetNode("portals").IsValid())
    rc = loader->Load(meshNode, region, false, false, 0, 0, factory->missingData, true);
  else
    rc = loader->Load(meshNode, region, true, false, 0, 0, factory->missingData, true);

  if (!rc.success) return;
  csRef<iSceneNode> node = scfQueryInterface<iSceneNode>(rc.result);
  if (!node) return;
  
  // Rotating to face the portal.
  csReversibleTransform trans = node->GetMovable()->GetFullTransform();
  csReversibleTransform transPortal;
  transPortal.Identity();
  transPortal.SetOrigin(csVector3(0, 0, 0));
  transPortal.RotateOther(csVector3(0, 1, 0), portalRot);
  transPortal.Translate(portalCenter);
  trans = trans * transPortal;

  csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper>(rc.result);
  csRef<iLight> light = scfQueryInterface<iLight>(rc.result);
  if (mesh && sector)
  {
    // If it's a portal we have to do a hardtransform.
    csRef<iPortalContainer> pc = scfQueryInterface<iPortalContainer>(mesh->GetMeshObject ());
    if (pc.IsValid())
    {
      mesh->HardTransform(trans);
    }
    else
    {
      mesh->GetMovable()->SetTransform(trans);

      // Create collider.
      csRef<iCollideSystem> cdsys = csQueryRegistry<iCollideSystem> (factory->object_reg);
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
    light->GetMovable()->SetTransform(trans);
    light->GetMovable()->UpdateMove();
  }

} // end LoadInstance()

float Interior::GetPortalYRotation(iMeshWrapper* portalMesh) 
{
  csRef<iPortalContainer> pc = scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
  csRef<iPortal> portal = pc->GetPortal(0);

  csPlane3 plane = portal->GetWorldPlane();
  float portalrot = atan2 ( plane.Normal().x, plane.Normal().z );

  return portalrot;
} // end GetPortalYRotation()

csVector3 Interior::GetPortalPosition(iMeshWrapper* portalMesh) 
{
  csRef<iPortalContainer> pc = scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
  csRef<iPortal> portal = pc->GetPortal(0);
  csVector3 portalcenter = portal->GetWorldSphere().GetCenter();

  csBox3 worldbb = portalMesh->GetWorldBoundingBox();

  portalcenter.y = worldbb.MinY();

  return portalcenter;
} // end GetPortalCenter()

bool Interior::IsPortal(iMeshWrapper* portalMesh) 
{
  if (!portalMesh) return false;
  csRef<iPortalContainer> pc = scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
  if (!pc.IsValid()) return false;
  csRef<iPortal> portal = pc->GetPortal(0);
  if (!portal.IsValid()) return false;

  return true;
} // end IsPortal()

bool Interior::CheckResources()
{
  if (!factory->IsReady()) return false;

  csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);

  csRef<iDocumentNodeIterator> nodes = interiorNode->GetNodes("portal");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csString offsetBool = current->GetAttributeValue("UseThisForPosition");
    if (offsetBool.CompareNoCase("true"))
    {
      std::string portalName = current->GetContentsValue();
      csRef<iMeshWrapper> portalmesh = engine->FindMeshObject(portalName.c_str());
      if (!IsPortal(portalmesh)) return false;
    }
    else
      return false;
  }

  for (size_t i = 0; i < factory->instanceNodes.GetSize(); i++)
  {
    csRef<iDocumentNode> inst = factory->instanceNodes.Get(i);
    LoadInstance(inst);
  }

  printf("I: Finished loading Interior %s\n", interiorName.c_str());

  finished = true;
  
  return true;

} // end CheckResources()

void Interior::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
{
  if (!inter || inter->CheckResources())
  {
    engine->RemoveEngineFrameCallback(this);
  }
} // end StartFrame()
