/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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

#include "resourcemanager.h"

#include <iengine/rview.h>
#include <iengine/camera.h>
#include <iengine/scenenode.h>
#include <csgeom/transfrm.h>
#include <ivaria/reporter.h>
#include <iutil/vfs.h>
#include <iutil/cfgmgr.h>
#include <iutil/object.h>
#include <iutil/stringarray.h>

#include <iengine/mesh.h>

#include <iutil/document.h>
#include <imesh/objmodel.h>
#include <imesh/object.h>
#include <ivaria/collider.h>
#include <cstool/collider.h>

#include <imap/loader.h>
#include <iengine/engine.h>

void ResourceManager::Report(int severity, const char* msg, ...)
{
  va_list arg;
  va_start (arg, msg);
  csReportV(object_reg, severity, "peragro.world", msg, arg);
  va_end (arg);
}

ResourceManager::ResourceManager(iObjectRegistry* object_reg, Common::World::WorldManager* worldManager)
  : object_reg(object_reg), worldManager(worldManager)
{

} // end ResourceManager()

ResourceManager::~ResourceManager()
{

} // end ~ResourceManager()

bool ResourceManager::Initialize()
{
  vfs = csQueryRegistry<iVFS> (object_reg);

  return true;
} // end Initialize

void ResourceManager::AddTestObjects()
{
  {
    Common::World::Object object;
    object.id = 0;
    object.name = "terrain";
    object.factoryFile = "/peragro/art/3d_art/buildings/island/factories/0,0";
    object.factoryName = "0,0";
    object.position = Geom::Vector3(300, 0, 300);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
  {
    Common::World::Object object;
    object.id = 1;
    object.name = "test1";
    object.factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object.factoryName = "genscythe001";
    object.position = Geom::Vector3(642, 0, 371);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
  {
    Common::World::Object object;
    object.id = 2;
    object.name = "test2";
    object.factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object.factoryName = "genscythe001";
    object.position = Geom::Vector3(642, 0, 376);
    object.sector = "World";
    worldManager->AddLookUp(object, false);
  }
} // end Initialize

bool EndsWith(const std::string& file, const std::string& extension)
{
  csString f = file.c_str();
  csString ext;
  f.SubString(ext, f.Length()-extension.size());
  return ext.CompareNoCase(extension.c_str());
}

std::map<std::string, Geom::Box> ResourceManager::FindMeshFacts(const std::string& file)
{
  std::map<std::string, Geom::Box> meshFacts;

  csRef<iDataBuffer> xmlfile = vfs->ReadFile (file.c_str());
  csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (object_reg));
  csRef<iDocument> doc (docsys->CreateDocument());
  const char* error = doc->Parse (xmlfile, true);
  if (error) return meshFacts;
  csRef<iDocumentNode> xml = doc->GetRoot ();
  if (!xml) return meshFacts;
  xml = xml->GetNode ("library");
  if (!xml) return meshFacts;
  csRef<iDocumentNodeIterator> nodes = xml->GetNodes("meshfact");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> node = nodes->Next();
    std::string name = node->GetAttributeValue("name");
    meshFacts[name] = GetBB(node);
  }
  return meshFacts;
}

Geom::Box ResourceManager::GetBB(iDocumentNode* node)
{
  Geom::Box bbox;
  std::string factoryName = node->GetAttributeValue("name");

  //-------------------------------------------------
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  csRef<iCollection> coll = engine->CreateCollection("worldmanager.BBcalc");
  csRef<iThreadReturn> tr = loader->LoadNode(node, coll);
  tr->Wait();
  engine->SyncEngineListsNow(loader);
  csRef<iMeshFactoryWrapper> mfw = coll->FindMeshFactory(factoryName.c_str());
  if (mfw)
    if (mfw->GetMeshObjectFactory())
      if (mfw->GetMeshObjectFactory()->GetObjectModel())
      {
        csRef<iObjectModel> obj = mfw->GetMeshObjectFactory()->GetObjectModel();
        csBox3 box = obj->GetObjectBoundingBox();
        bbox = Geom::Box(box.Min(), box.Max());
      }
  coll->ReleaseAllObjects();
  //-------------------------------------------------

  return bbox;
}

void ResourceManager::ScanFactories(const std::string& path)
{
  csRef<iStringArray> paths = vfs->FindFiles(path.c_str());
  for (size_t i = 0; i < paths->GetSize(); i++ )
  {
    std::string path = paths->Get(i);   
    if (EndsWith(path, "/")) // It's a directory
      if (EndsWith(path, ".svn/")) // Ignore;
        continue;
      else
        ScanFactories(path);
    else
      if (1 /*EndsWith(path, ".xml")*/)
      {
        std::map<std::string, Geom::Box> facts = FindMeshFacts(path);
        printf("%s (%d)\n", path.c_str(), facts.size());
        std::map<std::string, Geom::Box>::iterator it;
        for (it = facts.begin(); it != facts.end(); it++ )
        {
          using namespace Common::World;
          Factory factory;
          factory.factoryName = (*it).first;
          factory.factoryFile = path;
          factory.boundingBox = (*it).second;

          if (factory.boundingBox.Empty())
            printf("E: invalid boundingbox for %s (%s)\n", factory.factoryName.c_str(), factory.factoryFile.c_str());
          else
            worldManager->Add(factory);
        }
      }
  }
} // end ScanFactories()
