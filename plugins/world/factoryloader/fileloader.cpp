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

#include "fileloader.h"

#include <crystalspace.h>

//#include "texture.h"
//#include "properties.h"

#include "../utils.h"
//---------------------------------------------------------------------------
FileLoader::CDJob::CDJob (iObjectRegistry* object_reg, iMeshFactoryWrapper* fact)
  : scfImplementationType (this)
{
  CDJob::fact = fact;
  CDJob::object_reg = object_reg;
  finished = false;
}

FileLoader::CDJob::~CDJob()
{
}

void FileLoader::CDJob::Pre()
{
  if (fact && fact->GetMeshObjectFactory())
  {
    csRef<iObjectModel> fact_objmodel = fact->GetMeshObjectFactory()->GetObjectModel();
    if (fact_objmodel)
    {
      csRef<iCollideSystem> collide_system = csQueryRegistry<iCollideSystem> (object_reg);
      csStringID basemesh_id = collide_system->GetBaseDataID ();
      csStringID trianglemesh_id = collide_system->GetTriangleDataID ();

      // Check if a CD mesh has been set, trimesh.
      bool obj_trimesh_set = fact_objmodel->IsTriangleDataSet (trianglemesh_id);
      if (obj_trimesh_set)
        obj_trimesh = fact_objmodel->GetTriangleData (trianglemesh_id);
      else
        obj_trimesh = fact_objmodel->GetTriangleData (basemesh_id);
    }
  }
} // end Pre()

void FileLoader::CDJob::Run()
{
  if (obj_trimesh && collide_system)
  {
    // Create a collider with the triangle data.
    collider = collide_system->CreateCollider(obj_trimesh);
  }

  finished = true;
} // end Run()

void FileLoader::CDJob::Post()
{
  if (fact && collider && collide_system)
  {
    csRef<csColliderWrapper> childcw;
    childcw.AttachNew(new csColliderWrapper (fact->QueryObject(), collide_system, collider));
    if (childcw) childcw->SetName (fact->QueryObject ()->GetName());
  }
} // end Post()

//---------------------------------------------------------------------------

FileLoader::ParserJob::ParserJob (const std::string& path, const std::string& fileName)
  : scfImplementationType (this)
{
  ParserJob::path = path;
  ParserJob::fileName = fileName;
  finished = false;
}

FileLoader::ParserJob::~ParserJob()
{
}

void FileLoader::ParserJob::Run()
{
  std::string pathFile = PT::Utils::PathHelper(vfs, path, fileName);
  csRef<iDataBuffer> buf = vfs->ReadFile(pathFile.c_str());
  if (!buf.IsValid())
  {
    printf("E: Failed to open file '%s'!\n", fileName.c_str());
    finished = true;
    return;
  }

  csRef<iDocument> doc;
  csRef<iDocumentSystem> docsys;

  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    printf("E: Error while parsing file '%s'\n!", pathFile.c_str());
    finished = true;
    return;
  }

  csRef<iDocumentNode> root = doc->GetRoot();
  csRef<iDocumentNode> libnode = root->GetNode("library");
  if (!libnode.IsValid())
  {
    printf("E: Failed to get library node for file '%s'\n!", pathFile.c_str());
    finished = true;
    return;
  }

  ParseLibrary(libnode);

  finished = true;

} // end Run()

void FileLoader::ParserJob::ParseLibrary(iDocumentNode* libraryNode)
{
  //Parse the shaders.
  csRef<iDocumentNode> shaderNodes = libraryNode->GetNode("shaders");
  if (shaderNodes)
  {
    csRef<iDocumentNodeIterator> nodes = shaderNodes->GetNodes("shader");
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> fact = nodes->Next();
      csRef<iDocumentNode> fileNode = fact->GetNode("file");
      if (fileNode)
      {
        std::string shaderFile = fileNode->GetContentsValue();
        shaders.Push(shaderFile);
      }
    }
  }

  //Parse the textures.
  csRef<iDocumentNode> texturesNode = libraryNode->GetNode("textures");
  if (texturesNode)
  {
    csRef<iDocumentNodeIterator> nodes = texturesNode->GetNodes("texture");
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> current = nodes->Next();
      csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
      textures.Push(newNode);
    }
  }

  //Parse the materials.
  csRef<iDocumentNode> materialsNode = libraryNode->GetNode("materials");
  if (materialsNode)
  {
    csRef<iDocumentNode> newNode = PT::Utils::WrapAsLibNode(materialsNode, "");
    materials.Push(newNode);
    /*
    csRef<iDocumentNodeIterator> nodes = materialsNode->GetNodes("material");
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> current = nodes->Next();
      csRef<iDocumentNode> newNode = PT::Utils::WrapAsLibNode(current, "materials");
      materials.Push(newNode);
    }
    */
  }

  //Parse the meshfacts.
  csRef<iDocumentNodeIterator> nodes = libraryNode->GetNodes("meshfact");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    factories.Push(newNode);
  }
} // end ParseLibrary()

void FileLoader::AddToCollection(iBase* base)
{
  csRef<iObject> object = scfQueryInterfaceSafe<iObject> (base);
  if (object)
    collection->Add(object);
  else
    printf("ERROR: failed to add to collection!\n");
}

void FileLoader::ParseShader(const std::string& fileName)
{
  csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager> (object_reg);
  csRef<iShader> shader;
  // Check if the shader was already loaded previously.
  const csRefArray<iShader>& list = shaderMgr->GetShaders();
  for (size_t i = 0; i < list.GetSize(); i++)
  {
    csRef<iShader> s = list[i];
    if (s && s->GetFileName() &&strcmp(s->GetFileName(), fileName.c_str()) == 0)
    {
      shader = s;
      AddToCollection(shader);
      break;
    }
  }

  // Else load it.
  if (!shader)
  {
    csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
    csRef<iThreadReturn> tr = loader->LoadShader(fileName.c_str());
    shaders.Push(tr);
  }

} // end ParseShader()

void FileLoader::ParseTexture(iDocumentNode* node)
{
  csRef<iDocumentNode> fileNode;
  std::string texName;
  std::string texFileName;
  csRef<iDocumentNode> texNode = node->GetNode("texture");
  if (texNode)
  {
    texName = texNode->GetAttributeValue("name");
    fileNode = texNode->GetNode("file");
    if (fileNode)
    {
      texFileName = fileNode->GetContentsValue();
      // Find the node containing the actual text, so we can change that later.
      csRef<iDocumentNodeIterator> nodes = fileNode->GetNodes();
      while (nodes->HasNext())
      {
        csRef<iDocumentNode> current = nodes->Next();
        if (current->GetType() == CS_NODE_TEXT)
        { fileNode = current; break;}
      }
    }
  }

  if (!texNode || !fileNode) 
  {
    printf("E: Failed to parse texture!\n");
    return;
  }

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  printf("I: (%s) Parsing texture %s!\n", fileName.c_str(), texFileName.c_str());

  texFileName = PT::Utils::PathHelper(vfs, path, texFileName);
  std::string realPathFile = PT::Utils::RealPath(vfs, texFileName);
/*
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  iTextureList* list = engine->GetTextureList();
  for (size_t i = 0; i < list->GetCount(); i++)
  {
    printf("  has %s \n", list->Get(i)->QueryObject()->GetName());
  }
*/
  //printf("-- Real path %s!\n", realPathFile.c_str());

  csRef<iTextureWrapper> texw = FindTextureInEngine(texName, realPathFile);
  if (texw) // Texture file has already been loaded.
  {
    printf("-- Texture %s found in engine!\n", texw->QueryObject()->GetName());
    AddToCollection(texw);
    return;
  }

  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);

  printf("-- Loading Texture %s!\n", fileNode->GetValue());

  // Test for .dds
  size_t pos = texFileName.find_last_of(".");
  std::string pathFileDDS = texFileName.substr(0,pos)+ ".dds";
  // Test if the file exist.
  if (PT::Utils::FileExists(vfs,pathFileDDS))
  {
    //printf("I: Using dds for %s!\n", tex.name.c_str());
    fileNode->SetValue(pathFileDDS.c_str());
  }
  else
  {
    fileNode->SetValue(texFileName.c_str());
  }

  csRef<iThreadReturn> tr = loader->LoadNode(node, collection);
  textures.Push(tr);
  textureFileNames.Push(realPathFile);

} // end ParseTexture()

void FileLoader::ParseMaterial(iDocumentNode* node)
{
  csRef<iDocumentNode> libNode = node->GetNode("library");
  if (libNode)
  {
    csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
    csRef<iThreadReturn> tr = loader->LoadLibrary(libNode, collection);
    materials.Push(tr);
  }
} // end ParseMaterial()

void FileLoader::ParseMeshFact(iDocumentNode* node)
{
  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  csRef<iThreadReturn> tr = loader->LoadNode(node, collection);
  factories.Push(tr);
} // end ParseMeshFact()

//---------------------------------------------------------------------------

FileLoader::FileLoader (iObjectRegistry* object_reg): finished(false)
{
  FileLoader::object_reg = object_reg;

  static const char queueTag[] = "peragro.jobqueue.factoryparser";
  jobQueue = csQueryRegistryTagInterface<iJobQueue> (object_reg, queueTag);
  if (!jobQueue.IsValid())
  {
    jobQueue.AttachNew (new CS::Threading::ThreadedJobQueue (1, CS::Threading::THREAD_PRIO_LOW));
    object_reg->Register (jobQueue, queueTag);
  }

  parsedShaders = 0;
  parsedTextures = 0;
  parsedMaterials = 0;
  parsedFactories = 0;
}

FileLoader::~FileLoader ()
{
  if (parseJob.IsValid())
    jobQueue->Unqueue (parseJob);
}

bool FileLoader::Load (const std::string& path, const std::string& fileName, iCollection* collection)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  // Create the loadjob.
  parseJob.AttachNew (new ParserJob (path, fileName));
  this->path = path;
  this->fileName = fileName;
  this->collection = collection;

  // Add the plugins.
  parseJob->vfs = vfs;

  // Add to the queue.
  jobQueue->Enqueue (parseJob);

  return true;
} // end Load()

std::string FileLoader::GetFileName(iTextureWrapper* tex)
{
  std::string texFileName = "ERROR";
  if (tex && tex->QueryObject())
  {
    csRef<iObjectIterator> iter = tex->QueryObject()->GetIterator();
    while (iter->HasNext())
    {
      csRef<iKeyValuePair> key = scfQueryInterface<iKeyValuePair>(iter->Next());
      if (strcmp(key->GetKey(), "filename") == 0)
      {
        texFileName = key->GetValue();
        break;
      }
    } // end while
  }

  return texFileName;

} // end GetFileName()

void FileLoader::SetFileName(iTextureWrapper* tex, const std::string& texFileName)
{
  if (tex && tex->QueryObject())
  {
    csRef<iKeyValuePair> key; key.AttachNew(new csKeyValuePair ("filename", texFileName.c_str()));
    tex->QueryObject()->ObjAdd(key->QueryObject());
  }
  else
    printf("E: Failed to set filename on texture!\n");

} // end GetFileName()

iTextureWrapper* FileLoader::FindTextureInEngine(const std::string& texName, const std::string& texFileName)
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  iTextureList* list = engine->GetTextureList ();
  for (int i = 0; i < list->GetCount(); i++)
  {
    csRef<iTextureWrapper> texw = list->Get(i);
    if (texw && (GetFileName(texw) == texFileName))
    {
      printf("--1 %s!\n", texFileName.c_str());
      printf("--2 %s!\n", GetFileName(texw).c_str());

      // If the file is the same, but the name is different
      // make a clone, change the name and return that.
      if (texName.compare(texw->QueryObject()->GetName()) != 0)
      {
        printf("--3 clone %s to %s!\n", texw->QueryObject()->GetName(), texName.c_str());
        texw = texw->Clone();
        texw->QueryObject()->SetName(texName.c_str());
      }
      return texw;
    }
  } // end for

  /*
   csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  csRef<iTextureLoaderIterator>  it = loader->GetLoaderTextures ();
  while (it->HasNext())
  {
    csRef<iTextureWrapper> texw = it->Next();
    if (texw && GetFileName(texw) == texFileName)
    {
      printf("B: Found in loader!\n");
      return texw;
    }
  } // end for
  */

  return 0;

} // end FindTextureInEngine()

void FileLoader::Parse(unsigned int TypeId, bool clean)
{
  switch (TypeId)
  {
  case 0:
    if (!parsedTextures)
    {
      for (size_t i = 0; i < parseJob->textures.GetSize(); i++)
      {
        ParseTexture(parseJob->textures[i]);
      }
      parsedTextures = 1;
    }
    if (clean)
      parseJob->textures.DeleteAll();
    break;
  case 1:
    if (!parsedShaders)
    {
      for (size_t i = 0; i < parseJob->shaders.GetSize(); i++)
      {
        ParseShader(parseJob->shaders[i]);
      }
      parsedShaders = 1;
    }
    if (clean)
      parseJob->shaders.DeleteAll();
    break;
  case 2:
    if (!parsedMaterials)
    {
      for (size_t i = 0; i < parseJob->materials.GetSize(); i++)
      {
        ParseMaterial(parseJob->materials[i]);
      }
      parsedMaterials = 1;
    }
    if (clean)
      parseJob->materials.DeleteAll();
    break;
  case 3:
    if (!parsedFactories)
    {
      for (size_t i = 0; i < parseJob->factories.GetSize(); i++)
      {
        ParseMeshFact(parseJob->factories[i]);
      }
      parsedFactories = 1;
    }
    if (clean)
      parseJob->factories.DeleteAll();
    break;
  }

} // end Parse()

bool FileLoader::IsDoneLoading()
{
  //===[ Textures ]================================================================
  Parse(0, false);

  while (!textures.IsEmpty())
  { 
    if (textures.Top()->IsFinished()) 
    { 
      if (!textures.Top()->WasSuccessful()) 
        printf("E: Failed to load texture %s\n", textureFileNames.Top().c_str());

      csRef<iTextureWrapper> texw = scfQueryInterfaceSafe<iTextureWrapper> (textures.Top()->GetResultRefPtr());
      if (texw)
      {
        SetFileName(texw, textureFileNames.Top());
        csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
        iTextureList* list = engine->GetTextureList ();
        list->Add(texw);
      }
      csRef<iThreadReturn> p = textures.Pop(); 
      all.Push(p); 
      textureFileNames.Pop();
    }
    else 
    { 
      return false; 
    } 
  }

  Parse(0, true);

  //===[ Shaders ]================================================================
  Parse(1, false);
  
  while (!shaders.IsEmpty())
  { if (shaders.Top()->IsFinished()) { csRef<iThreadReturn> p = shaders.Pop(); all.Push(p); } else { return false; } }

  Parse(1, true);

  //===[ Materials ]================================================================
  Parse(2, false);
  
  while (!materials.IsEmpty())
  { if (materials.Top()->IsFinished()) { csRef<iThreadReturn> p = materials.Pop(); all.Push(p); } else { return false; } }

  Parse(2, true);

  //===[ Factories ]================================================================
  Parse(3, false);

  while (!factories.IsEmpty())
  { 
    if (factories.Top()->IsFinished()) 
    {
      if (!factories.Top()->WasSuccessful()) 
        printf("E: Failed to load factory %s\n", fileName.c_str());

      csRef<iMeshFactoryWrapper> fact = scfQueryInterfaceSafe<iMeshFactoryWrapper> (factories.Top()->GetResultRefPtr());
      if (fact)
      {
        csRef<CDJob> cdJob;
        cdJob.AttachNew (new CDJob (object_reg, fact));
        cdJob->Pre();
        cdJobs.Push(cdJob);
        jobQueue->Enqueue (cdJob);
      }
      csRef<iThreadReturn> p = factories.Pop(); 
      all.Push(p);
    }
    else
      return false; 
  }

  Parse(3, true);

  //===[ Collission Meshes ]================================================================
  while (!cdJobs.IsEmpty())
  { 
    if (cdJobs.Top()->IsFinished())
    {
      cdJobs.Top()->Post();
      csRef<CDJob> (cdJobs.Pop());
    }
    else
      return false;
  }

  return true;
}

bool FileLoader::Process()
{
  if (IsReady()) return true;

  if (!parseJob->IsFinished()) 
    return false;

  if (!IsDoneLoading()) 
    return false;

  finished = true;

  return true;
} // end AddToEngine()
