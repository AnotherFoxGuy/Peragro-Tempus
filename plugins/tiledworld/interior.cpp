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

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/objreg.h>
#include <csutil/refarr.h>
#include <csutil/scf_implementation.h>
#include <iengine/collection.h>
#include <iutil/document.h>
#include <iengine/sector.h>
#include <csutil/csstring.h>
#include <csgeom/transfrm.h>
#include <iengine/scenenode.h>
#include <iengine/movable.h>
#include <iengine/portalcontainer.h>
#include <iengine/mesh.h>
//#include <ivaria/collider.h>
#include <cstool/collider.h>
#include <csgeom/plane3.h>
#include <iengine/portal.h>
#include <csgeom/sphere.h>
#include <imesh/object.h>

#include "interior.h"

namespace PT
{
  namespace World
  {
    InteriorFactory::InteriorFactory(const std::string& fileName, WorldManager* world)
      : scfImplementationType (this), Level(world)
    {
      this->fileName = fileName;

      OpenFile("", fileName);
    } // end InteriorFactory()

    InteriorFactory::~InteriorFactory()
    {
      printf("I: Unloading InteriorFactory %s\n", fileName.c_str());
    } // end ~InteriorFactory()

    void InteriorFactory::LoadInstance(iDocumentNode* meshNode)
    {
      instanceNodes.Push(meshNode);
    } // end LoadInstance()

    //--------------------------------------------------------------

    Interior::Interior(iDocumentNode* interiorNode, InteriorFactory* factory)
      : scfImplementationType (this)
    {
      this->interiorNode = interiorNode->GetNode("interior");
      this->factory = factory;
      this->interiorName = this->interiorNode->GetAttributeValue("name");
      loading = false;
      finished = false;
    } // end Interior()

    Interior::~Interior()
    {
      printf("I: Unloading Interior %s\n", interiorName.c_str());

      // Remove callback.
      if (cb.IsValid())
      {
        csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);
        engine->RemoveEngineFrameCallback(cb);
      }
    } // end ~Interior()

    void Interior::Load()
    {
      if (loading) return;

      printf("I: Loading Interior %s\n", interiorName.c_str());
      loading = true;

      csRef<iEngine> engine = csQueryRegistry<iEngine> (factory->object_reg);

      instances = engine->CreateCollection(interiorName.c_str());
      sector = engine->CreateSector(interiorName.c_str());
      instances->Add(sector->QueryObject());

      iSector* world = engine->FindSector("World");
      if (world)
        sector->SetRenderLoop(world->GetRenderLoop());

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
      csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (factory->object_reg);

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
      if (!portalMesh.IsValid())
      {
        printf("E: Failed to load instance for %s\n", interiorName.c_str());
        return;
      }

      float portalRot = GetPortalYRotation(portalMesh);
      csVector3 portalCenter = GetPortalPosition(portalMesh);

      // Load our mesh into the instances collection and ONLY look for
      // meshfactories in our own factory list.
      // If it's a portal allow it to look for sectors beyond the collection.
      csRef<iThreadReturn> rc;
      if (meshNode && meshNode->GetNode("portals").IsValid())
        rc = loader->LoadNode((csRef<iDocumentNode>) meshNode, instances, 0, factory->missingData, KEEP_ALL, true);
      else
        rc = loader->LoadNode((csRef<iDocumentNode>) meshNode, instances, 0, factory->missingData, KEEP_ALL, true);

      // Wait for the meshobj to be loaded.
      rc->Wait();

      if (!rc->WasSuccessful())
      {
        csRef<iDocumentNode> meshN = meshNode->GetNode("meshobj");
        if (meshN)
          printf("E: Failed to load instance '%s' for %s\n", meshN->GetAttributeValue("name"), interiorName.c_str());
        else
          printf("E: Failed to load instance: Invalid node? for %s\n", interiorName.c_str());
        return;
      }

      csRef<iSceneNode> node = scfQueryInterface<iSceneNode>(rc->GetResultRefPtr ());
      if (!node)
      {
        printf("E: Failed to load instance for %s\n", interiorName.c_str());
        return;
      }

      // Rotating to face the portal.
      csReversibleTransform trans = node->GetMovable()->GetFullTransform();
      csReversibleTransform transPortal;
      transPortal.Identity();
      transPortal.SetOrigin(csVector3(0, 0, 0));
      transPortal.RotateOther(csVector3(0, 1, 0), portalRot);
      transPortal.Translate(portalCenter);
      trans = trans * transPortal;

      csRef<iMeshWrapper> mesh = scfQueryInterface<iMeshWrapper>(rc->GetResultRefPtr ());
      csRef<iLight> light = scfQueryInterface<iLight>(rc->GetResultRefPtr ());
      if (mesh && sector)
      {
        // If it's a portal we have to do a hardtransform.
        csRef<iPortalContainer> pc =
          scfQueryInterface<iPortalContainer>(mesh->GetMeshObject ());
        if (pc.IsValid())
        {
          mesh->HardTransform(trans);
        }
        else
        {
          mesh->GetMovable()->SetTransform(trans);

          // Create collider.
          csRef<iCollideSystem> cdsys =
            csQueryRegistry<iCollideSystem> (factory->object_reg);
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
      else
      {
        printf("E: Failed to load instance for %s\n", interiorName.c_str());
        return;
      }

    } // end LoadInstance()

    float Interior::GetPortalYRotation(iMeshWrapper* portalMesh)
    {
      csRef<iPortalContainer> pc =
        scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
      csRef<iPortal> portal = pc->GetPortal(0);

      csPlane3 plane = portal->GetWorldPlane();
      float portalrot = atan2 ( plane.Normal().x, plane.Normal().z );

      return portalrot;
    } // end GetPortalYRotation()

    csVector3 Interior::GetPortalPosition(iMeshWrapper* portalMesh)
    {
      csRef<iPortalContainer> pc =
        scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
      csRef<iPortal> portal = pc->GetPortal(0);
      csVector3 portalcenter = portal->GetWorldSphere().GetCenter();

      csBox3 worldbb = portalMesh->GetWorldBoundingBox();

      portalcenter.y = worldbb.MinY();

      return portalcenter;
    } // end GetPortalCenter()

    bool Interior::IsPortal(iMeshWrapper* portalMesh)
    {
      if (!portalMesh) return false;
      csRef<iPortalContainer> pc =
        scfQueryInterface<iPortalContainer>(portalMesh->GetMeshObject ());
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
          csRef<iMeshWrapper> portalmesh =
            engine->FindMeshObject(portalName.c_str());
          if (!IsPortal(portalmesh)) return false;
        }
        else
          return false;
      }

      for (size_t i = 0; i < factory->instanceNodes.GetSize(); i++)
      {
        csRef<iDocumentNode> inst = factory->instanceNodes.Get(i);
        LoadInstance(inst);
        //Report(PT::Error, "Loading instance %s",
        //  inst->GetAttributeValue("name"));
      }

      printf("I: Finished loading Interior %s\n", interiorName.c_str());

      finished = true;

      return true;

    } // end CheckResources()

    void Interior::FrameCallBack::StartFrame(iEngine* engine,
                                             iRenderView* rview)
    {
      if (!inter || inter->CheckResources())
      {
        engine->RemoveEngineFrameCallback(this);
      }
    } // end StartFrame()

  } // World namespace
} // PT namespace
