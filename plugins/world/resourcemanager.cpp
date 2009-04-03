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
#include <iengine/sector.h>
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

#include <csutil/csmd5.h>

#include <imap/loader.h>
#include <iengine/engine.h>

#include <ivaria/stdrep.h>

#include "common/util/printhelper.h"

#include "common/util/geomhelper.h"

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
  using namespace Common::World;
  {
    boost::shared_ptr<Object> object(new Object());
    object->id = 1;
    object->name = "test1";
    object->factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object->factoryName = "genscythe001";
    object->position = WFMath::Point<3>(642, 14, 371);
    object->rotation.identity();
    object->sector = "World";
    worldManager->AddLookUp(object, false);
  }
  {
    boost::shared_ptr<Object> object(new Object());
    object->id = 2;
    object->name = "test2";
    object->factoryFile = "/peragro/art/3d_art/props/others/scythes/scythe001/library.xml";
    object->factoryName = "genscythe001";
    object->position = WFMath::Point<3>(642, 14, 376);
    object->rotation.identity();
    object->sector = "World";
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

std::vector<Common::World::Factory> ResourceManager::FindMeshFacts(const std::string& file, bool returnAll)
{
  std::vector<Common::World::Factory> meshFacts;

  if (!vfs->Exists (file.c_str())) return meshFacts;

  csRef<iDataBuffer> xmlfile = vfs->ReadFile (file.c_str());
  csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (object_reg));
  csRef<iDocument> doc (docsys->CreateDocument());
  const char* error = doc->Parse (xmlfile, true);
  if (error) return meshFacts;
  csRef<iDocumentNode> xml = doc->GetRoot();
  if (!xml) return meshFacts;
  xml = xml->GetNode ("library");
  if (!xml) return meshFacts;
  csRef<iDocumentNodeIterator> nodes = xml->GetNodes("meshfact");

  //TODO: make hash per node.
  std::string hash = csMD5::Encode(xmlfile->GetData()).HexString().GetData();
  if (!returnAll) printf("%s\n", file.c_str());
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> node = nodes->Next();
    std::string name = node->GetAttributeValue("name");

    if ((worldManager->GetMD5(file, name).compare(hash) == 0) && !returnAll)
    {
      printf(" - '%s' Up to date.\n", name.c_str());
      continue; // Factory entry is up to date.
    }

    using namespace Common::World;
    Factory factory;
    factory.factoryName = name;
    factory.factoryFile = file;
    if (!returnAll) factory.boundingBox = GetBB(node);
    factory.detailLevel = 0; // TODO
    factory.hash = hash;

    if (!returnAll) printf(" - '%s' Updating.\n", name.c_str());

    meshFacts.push_back(factory);
  }

  return meshFacts;
}

WFMath::AxisBox<3> ResourceManager::GetBB(iDocumentNode* node)
{
  WFMath::AxisBox<3> bbox;
  std::string factoryName = node->GetAttributeValue("name");

  //-------------------------------------------------
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  csRef<iCollection> coll = engine->CreateCollection("worldmanager.BBcalc");
  csRef<iVFS> vfs = csQueryRegistry<iVFS>(object_reg);
  csRef<iThreadReturn> tr = loader->LoadNodeWait(vfs->GetCwd(), node, coll);
  csRef<iMeshFactoryWrapper> mfw = scfQueryInterfaceSafe<iMeshFactoryWrapper>(tr->GetResultRefPtr());
  if (mfw)
    if (mfw->GetMeshObjectFactory())
      if (mfw->GetMeshObjectFactory()->GetObjectModel())
      {
        csRef<iObjectModel> obj = mfw->GetMeshObjectFactory()->GetObjectModel();
        csBox3 box = obj->GetObjectBoundingBox();
        bbox = WFMath::AxisBox<3>(VectorHelper::Convert(box.Min()), VectorHelper::Convert(box.Max()));
      }
  coll->ReleaseAllObjects();
  //-------------------------------------------------

  return bbox;
}

void ResourceManager::ScanFactories(const std::string& path)
{
  // Disable Popups.
  csRef<iStandardReporterListener> rep = csQueryRegistry<iStandardReporterListener>(object_reg);
  rep->SetNativeWindowManager(0);

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
        std::vector<Common::World::Factory> facts = FindMeshFacts(path);
        //printf("%s (%d)\n", path.c_str(), facts.size());
        std::vector<Common::World::Factory>::iterator it;
        for (it = facts.begin(); it != facts.end(); it++ )
        {
          if (!(*it).boundingBox.isValid())
          {
            (*it).boundingBox = WFMath::AxisBox<3>(WFMath::Point<3>(0, 0, 0), WFMath::Point<3>(1, 1, 1));
            printf("E: invalid boundingbox for %s (%s)\n", (*it).factoryName.c_str(), (*it).factoryFile.c_str());
          }
          worldManager->Add(*it);
        }
      }
  }

  // Enable Popups.
  rep->SetDefaults();

} // end ScanFactories()

std::string GetFactoryName(iDocumentNode* node)
{
  csRef<iDocumentNode> params = node->GetNode ("params");
  if (!params) return "Error in node";
  csRef<iDocumentNode> factory = params->GetNode ("factory");
  if (!factory) return "Error in node";
  return factory->GetContentsValue();
}

std::string GetFactoryFile(const std::string& factoryName, const std::vector<Common::World::Factory>& facts)
{
  std::vector<Common::World::Factory>::const_iterator it;
  for (it = facts.begin(); it != facts.end(); it++ )
  {
    if ((*it).factoryName == factoryName)
      return (*it).factoryFile;
  }
  return "Error: file for factname not found";
}

WFMath::Point<3> GetPosition(iDocumentNode* node, float xOffset = 0, float zOffset = 0)
{
  WFMath::Point<3> vec;

  csRef<iDocumentNode> move = node->GetNode ("move");
  if (!move) return WFMath::Point<3>();
  csRef<iDocumentNode> v = move->GetNode ("v");
  if (!v) return WFMath::Point<3>();

  vec[0] = v->GetAttributeValueAsFloat("x");
  vec[1] = v->GetAttributeValueAsFloat("y");
  vec[2] = v->GetAttributeValueAsFloat("z");

  vec[0] += xOffset;
  vec[2] += zOffset;

  return vec;
}

WFMath::RotMatrix<3> GetRotation(iDocumentNode* node)
{
  WFMath::RotMatrix<3> rot;
  rot.identity();

  csRef<iDocumentNode> move = node->GetNode ("move");
  if (!move) return rot;

  csRef<iDocumentNode> matrix = move->GetNode ("matrix");
  if (!matrix) return rot;

  WFMath::Vector<3> vec;
  csRef<iDocumentNode> x = matrix->GetNode ("rotx");
  if (x) vec[0] = x->GetContentsValueAsFloat();
  csRef<iDocumentNode> y = matrix->GetNode ("roty");
  if (y) vec[1] = y->GetContentsValueAsFloat();
  csRef<iDocumentNode> z = matrix->GetNode ("rotz");
  if (z) vec[2] = z->GetContentsValueAsFloat();

  rot = rot.rotation(vec);

  return rot;
}

std::vector<Common::World::Object> ResourceManager::FindMeshObjects(const std::string& file)
{
  std::vector<Common::World::Object> meshObjects;

  printf("FindMeshObjects %s\n", file.c_str());
  csRef<iDataBuffer> xmlfile = vfs->ReadFile (file.c_str());
  csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (object_reg));
  csRef<iDocument> doc (docsys->CreateDocument());
  const char* error = doc->Parse (xmlfile, true);
  if (error) return meshObjects;
  csRef<iDocumentNode> xml = doc->GetRoot();
  if (!xml) return meshObjects;
  xml = xml->GetNode ("tile");
  if (!xml) return meshObjects;
  //------------------------------------------------------------
  int xOffset = 0;
  int zOffset = 0;
  size_t p = file.find_last_of("/");
  std::string f = file.substr(p+1, file.length());
  f = f.substr( strlen("tile-"), f.length() );
  sscanf (f.c_str() ,"%d-%d", &xOffset, &zOffset);
  xOffset *= 512;
  zOffset *= 512;
  //------------------------------------------------------------
  std::vector<Common::World::Factory> facts;
  csRef<iDocumentNodeIterator> libnodes = xml->GetNodes("library");
  while (libnodes->HasNext())
  {
    csRef<iDocumentNode> node = libnodes->Next();
    std::string file = node->GetContentsValue();
    std::vector<Common::World::Factory> f = FindMeshFacts(file, true);
    facts.insert(facts.end(), f.begin(), f.end());
  }
  //------------------------------------------------------------
  csRef<iDocumentNodeIterator> nodes = xml->GetNodes("meshobj");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> node = nodes->Next();
    std::string name = node->GetAttributeValue("name");

    using namespace Common::World;
    Object object;
    object.name = name;
    object.factoryName = GetFactoryName(node);
    object.factoryFile = GetFactoryFile(object.factoryName, facts);
    object.position = GetPosition(node, (float)xOffset, (float)zOffset);
    object.rotation = GetRotation(node);
    object.sector = "World";

    meshObjects.push_back(object);
  }

  return meshObjects;
}

void ResourceManager::ScanObjects(const std::string& path)
{
  using namespace Common::World;

  csRef<iStringArray> paths = vfs->FindFiles(path.c_str());
  for (size_t i = 0; i < paths->GetSize(); i++ )
  {
    std::string path = paths->Get(i);
    if (EndsWith(path, "/")) // It's a directory
      if (EndsWith(path, ".svn/")) // Ignore;
        continue;
      else
        ScanObjects(path);
    else
    {
      size_t p = path.find_last_of("/");
      std::string file = path.substr(p+1, path.length());

      csString csfile = file.c_str();
      if (csfile.StartsWith("tile-"))
      {
        std::vector<Object> objs = FindMeshObjects(path);
        std::vector<Object>::iterator it;
        //TODO: Get max id.
        static size_t id = 2;
        for (it = objs.begin(); it != objs.end(); it++ )
        {
          boost::shared_ptr<Object> object(new Object(*it));
          object->id = id;
          worldManager->AddLookUp(object, false);
          printf("ID: %"SIZET"\n", object->id);
          id++;
        }
      }
    }
  }
} // end ScanObjects()
