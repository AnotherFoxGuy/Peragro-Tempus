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

#include "meshobjectfactory.h"
#include "genmeshloader.h"
#include "material.h"
#include "texture.h"
#include "properties.h"

//---------------------------------------------------------------------------

FileLoader::LoaderJob::LoaderJob (const std::string& path, const std::string& fileName)
  : scfImplementationType (this)
{
  LoaderJob::path = path;
  LoaderJob::fileName = fileName;
  finished = false;
}

FileLoader::LoaderJob::~LoaderJob()
{
  textures.DeleteAll();
  materials.DeleteAll();
  prototypes.DeleteAll();
}

static std::string PathHelper(iVFS* vfs, const std::string& path, const std::string& fileName)
{
  csArray<std::string> paths;
  paths.Push("");
  paths.Push("/peragro/");
  paths.Push("/peragro/tiles/");
  paths.Push("/peragro/factories/");

  for (size_t i = 0; i < paths.GetSize(); i++)
  {
    std::string pathFile = paths[i] + path + fileName;
    if (vfs->Exists(pathFile.c_str()))
      return pathFile;
  }

  return "";

} // PathHelper()

void FileLoader::LoaderJob::Run()
{
  //{//to measure time
    //CS::MeasureTime lTimeMeasurer ("Loading file");

    std::string pathFile = PathHelper(vfs, path, fileName);
    csRef<iDataBuffer> buf = vfs->ReadFile(pathFile.c_str());
    if (!buf.IsValid())
    {
      printf("E: Failed to open file '%s'!\n", fileName.c_str());
      finished = true;
      return;
    }
    //lTimeMeasurer.PrintIntermediate ("Opening file ");

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
    //lTimeMeasurer.PrintIntermediate ("XML parsing");

    csRef<iDocumentNode> root = doc->GetRoot();
    csRef<iDocumentNode> libnode = root->GetNode("library");
     if (!libnode.IsValid())
    {
      printf("E: Failed to get library node for file '%s'\n!", pathFile.c_str());
      finished = true;
      return;
    }
    //lTimeMeasurer.PrintIntermediate ("Getting node");

    ParseLibrary(libnode);

  //}//block, to measure time.

  finished = true;

} // end Run()

void FileLoader::LoaderJob::ParseLibrary(iDocumentNode* libraryNode)
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
        csString shaderFile = fileNode->GetContentsValue();
        shaders.Push(shaderFile);
      }
    }
  }

  //Parse the textures.
  csRef<iDocumentNode> textures = libraryNode->GetNode("textures");
  if (textures)
  {
    csRef<iDocumentNodeIterator> nodes = textures->GetNodes("texture");
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> fact = nodes->Next();
      ParseTexture(fact);
    }
  }

  //Parse the materials.
  csRef<iDocumentNode> materials = libraryNode->GetNode("materials");
  if (materials)
  {
    csRef<iDocumentNodeIterator> nodes = materials->GetNodes("material");
    while (nodes->HasNext())
    {
      csRef<iDocumentNode> fact = nodes->Next();
      ParseMaterial(fact);
    }
  }

  //Parse the meshfacts.
  csRef<iDocumentNodeIterator> nodes = libraryNode->GetNodes("meshfact");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> fact = nodes->Next();
    csObjectPrototype proto;
    ParseMeshFact(fact, proto, 0);
    if (!proto.failedLoading) prototypes.Push(proto);
  }
} // end ParseLibrary()

void FileLoader::LoaderJob::ParseTexture(iDocumentNode* node)
{
  TexturePrototype tex;

  if (!Texture::ParseTexture (node, tex, SyntaxService)) return;

  // Test for .dds
  size_t pos = tex.fileName.find_last_of(".");
  std::string file = tex.fileName.substr(0,pos)+ ".dds";
  std::string pathFileDDS = PathHelper(vfs, path, file);
  // Test if the file exist.
  if (!pathFileDDS.empty())
  {
    // Update filename.
    tex.usedFileName = pathFileDDS;
    //printf("I: Using dds for %s!\n", tex.name.c_str());
    //Load the image data
    tex.buf = vfs->ReadFile (pathFileDDS.c_str(), false);
    // Push it on our list.
    textures.Push(tex);
  }
  else
  {
    std::string pathFile = PathHelper(vfs, path, tex.fileName);
    // Update filename.
    tex.usedFileName = pathFile;
    //Load the image data
    tex.buf = vfs->ReadFile (pathFile.c_str(), false);
    // Push it on our list.
    textures.Push(tex);
  }
} // end ParseTexture()

void FileLoader::LoaderJob::ParseMaterial(iDocumentNode* node)
{
  MaterialPrototype mat;

  if (!Material::ParseMaterial (node, mat, SyntaxService)) return;

  // Push it on our list.
  materials.Push(mat);
} // end ParseMaterial()

csPtr<iMeshObjectFactory> FileLoader::LoaderJob::NewFactory (const std::string& plug)
{
  // Get the right plugin.
  csRef<iMeshObjectType> type;
  for (size_t i = 0; i < plugins.GetSize(); i++)
  {
    if (plugins[i].name == plug || plugins[i].classId == plug)
    {
      type = plugins[i].plugin;
      break;
    }
  }
  if (!type.IsValid())
  {
    printf("E: Don't know how to load %s\n", plug.c_str());
    return 0;
  }

  return type->NewFactory ();
}

void FileLoader::LoaderJob::ParseMeshFact(iDocumentNode* node, csObjectPrototype& proto, csObjectPrototype* parent)
{
  proto.name = node->GetAttribute("name")->GetValue();

  std::string plug = "nullmesh";
  if (node->GetNode("plugin")) plug = node->GetNode("plugin")->GetContentsValue();

  csRef<iMeshObjectFactory> fact = NewFactory (plug);
  proto.object = fact;

  if (!proto.object.IsValid())
  {
    printf("E: Failed to load MeshObjectFactory %s\n", proto.name.c_str());
    return;
  }

  if (!MeshObjectFactory::LoadMeshObjectFactory (node, parent, proto, SyntaxService))
  {
    printf("E: Failed to load MeshObjectFactory %s\n", proto.name.c_str());
    return;
  }

  if (proto.nullMesh)
  {
    csRef<iMeshObjectFactory> fact = NewFactory ("nullmesh");
    proto.object = fact;
    csRef<iNullFactoryState> nullmesh = scfQueryInterface<iNullFactoryState> (fact);
    nullmesh->SetBoundingBox (proto.boundingbox);
  }
  else if (!GenMesh::Parse(node->GetNode("params"), proto, SyntaxService))
  {
    printf("E: Failed to load GenMesh %s\n", proto.name.c_str());
    return;
  }

  // Parse the children.
  for (size_t i = 0; i < proto.childNodes.GetSize(); i++)
  {
    csObjectPrototype child;
    ParseMeshFact(proto.childNodes.Get(i), child, &proto);
    if (!child.failedLoading) proto.children.Push(child);
  }
  proto.childNodes.DeleteAll();

  // Create the collider for this mesh.
  iObjectModel* fact_objmodel = fact->GetObjectModel ();
  if (fact_objmodel)
  {
    iTriangleMesh* obj_trimesh;
    // Check if a CD mesh has been set, trimesh.
    bool obj_trimesh_set = fact_objmodel->IsTriangleDataSet (trianglemesh_id);
    if (obj_trimesh_set)
      obj_trimesh = fact_objmodel->GetTriangleData (trianglemesh_id);
    else
      obj_trimesh = fact_objmodel->GetTriangleData (basemesh_id);
    // Create a collider with the triangle data.
    proto.collider = collide_system->CreateCollider(obj_trimesh);
  }

  proto.failedLoading = false;

} // end ParseMeshFact()

//---------------------------------------------------------------------------

FileLoader::FileLoader (iObjectRegistry* object_reg)
{
  FileLoader::object_reg = object_reg;

  static const char queueTag[] = "crystalspace.jobqueue.fileload";
  jobQueue = csQueryRegistryTagInterface<iJobQueue> (object_reg, queueTag);
  if (!jobQueue.IsValid())
  {
    jobQueue.AttachNew (new CS::Threading::ThreadedJobQueue (1, CS::Threading::THREAD_PRIO_LOW));
    object_reg->Register (jobQueue, queueTag);
  }

  loaderContext.AttachNew(new LoaderContext(this));

}

FileLoader::~FileLoader ()
{
  if (loadJob.IsValid())
    jobQueue->Unqueue (loadJob);
}

bool FileLoader::Load (const std::string& path, const std::string& fileName, iCollection* collection)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  // Create the loadjob.
  loadJob.AttachNew (new LoaderJob (path, fileName));
  this->path = path;
  this->fileName = fileName;
  this->collection = collection;

  // Add the plugins.
  loadJob->vfs = vfs;
  loadJob->SyntaxService = SyntaxService;
  loadJob->collide_system = csQueryRegistry<iCollideSystem> (object_reg);

  // Create the genmesh plugin.
  ObjectPlugin plugin;
  plugin.name = "genmeshfact"; // wrong
  plugin.classId = "crystalspace.mesh.object.genmesh";
  plugin.plugin = csLoadPluginCheck<iMeshObjectType> (object_reg, plugin.classId.c_str(), false);
  plugin.classId = "crystalspace.mesh.loader.genmesh";// wrong
  loadJob->plugins.Push(plugin);

  // Create the nullmesh plugin.
  ObjectPlugin plugin2;
  plugin2.name = "nullmesh"; // wrong
  plugin2.classId = "crystalspace.mesh.object.null";
  plugin2.plugin = csLoadPluginCheck<iMeshObjectType> (object_reg, plugin2.classId.c_str(), false);
  loadJob->plugins.Push(plugin2);

  //Misc
  loadJob->basemesh_id = loadJob->collide_system->GetBaseDataID ();
  loadJob->trianglemesh_id = loadJob->collide_system->GetTriangleDataID ();

  // Add to the queue.
  jobQueue->Enqueue (loadJob);

  return true;
} // end Load()

void FileLoader::WaitForJob() const
{
  jobQueue->PullAndRun (loadJob);
}

bool FileLoader::IsReady() const
{
  return loadJob->finished;
}

iMaterialWrapper* FileLoader::GetMaterial(const std::string& name)
{
  for (size_t i = 0; i < loadJob->materials.GetSize(); i++)
  {
    MaterialPrototype matproto = loadJob->materials[i];
    if (matproto.name == name && matproto.materialWrapper.IsValid())
    {
      return matproto.materialWrapper;
    }
  }

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  csRef<iMaterialWrapper> mat = engine->GetMaterialList ()->FindByName(name.c_str());
  if (!mat.IsValid())
  {
    printf("E: Failed to find material %s! (%s)\n", name.c_str(), fileName.c_str());
    mat = engine->GetMaterialList ()->FindByName("stone2"); // error material
  }

  return mat;
} // end GetMaterial()

iTextureWrapper* FileLoader::GetTexture(const std::string& name)
{
  for (size_t i = 0; i < loadJob->textures.GetSize(); i++)
  {
    TexturePrototype tex = loadJob->textures[i];
    if (tex.name == name && tex.texture.IsValid())
    {
      return tex.texture;
    }
  }

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  csRef<iTextureWrapper> tex = engine->GetTextureList ()->FindByName(name.c_str());
  if (!tex.IsValid())
  {
    printf("E: Failed to find texture %s!\n", name.c_str());
    tex = engine->GetTextureList ()->FindByName("stone"); // error material
  }

  return tex;
} // end GetTexture()

std::string FileLoader::GetFileName(iTextureWrapper* tex)
{
  std::string texFileName;
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

void FileLoader::SetFileName(iTextureWrapper* tex, const std::string& fileName)
{
  if (tex && tex->QueryObject())
  {
    csRef<iKeyValuePair> key; key.AttachNew(new csKeyValuePair ("filename", fileName.c_str()));
    tex->QueryObject()->ObjAdd(key->QueryObject());
  }
  else
    printf("E: Failed to set filename on texture!\n");

} // end GetFileName()

iTextureWrapper* FileLoader::FindTextureInEngine(TexturePrototype& tex)
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  iTextureList* list = engine->GetTextureList ();
  for (int i = 0; i < list->GetCount(); i++)
  {
    csRef<iTextureWrapper> texw = list->Get(i);
    if (texw && texw->QueryObject())
    {
      std::string name = texw->QueryObject()->GetName();
      if (name == tex.name && ( (GetFileName(texw) == tex.fileName) || (GetFileName(texw) == tex.usedFileName) ))
      {
        return texw;
      }
    }
  } // end for

  return 0;

} // end FindTextureInEngine()

csRef<iShader> FileLoader::LoadShader (const char* filename)
{
  csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager> (object_reg);
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  csVfsDirectoryChanger dirChanger (vfs);

  csRef<iFile> shaderFile = vfs->Open (filename, VFS_FILE_READ);
  if (!shaderFile)
  {
    printf ("Unable to open shader file '%s'!\n", filename);
    return 0;
  }

  csRef<iDocumentSystem> docsys = csQueryRegistry<iDocumentSystem> (object_reg);
  if (docsys == 0)
    docsys.AttachNew (new csTinyDocumentSystem ());

  csRef<iDocument> shaderDoc = docsys->CreateDocument ();
  const char* err = shaderDoc->Parse (shaderFile, false);
  if (err != 0)
  {
    printf ("Could not parse shader file '%s': %s\n", filename, err);
    return 0;
  }
  csRef<iDocumentNode> shaderNode = shaderDoc->GetRoot ()->GetNode ("shader");
  if (!shaderNode)
  {
    printf ("Shader file '%s' is not a valid shader XML file!\n", filename);
    return 0;
  }

  dirChanger.ChangeTo (filename);

  // Check if the shader was already loaded previously.
  const char* shaderName = shaderNode->GetAttributeValue ("name");
  if (shaderName)
  {
    csRef<iShader> shader = shaderMgr->GetShader(shaderName);
    if (shader.IsValid())
      return shader;
  }

  const char* type = shaderNode->GetAttributeValue ("compiler");
  if (type == 0)
    type = shaderNode->GetAttributeValue ("type");
  if (type == 0)
    type = "xmlshader";
  csRef<iShaderCompiler> shcom = shaderMgr->GetCompiler (type);

  csRef<iShader> shader = shcom->CompileShader (loaderContext, shaderNode);
  if (shader)
  {
    shader->SetFileName (filename);
    shaderMgr->RegisterShader (shader);
    return shader;
  }
  else
  {
    return 0;
  }
} // end LoadShader()

bool FileLoader::AddToEngine()
{
  if (!IsReady())
  {
    printf("W: Requested file didn't complete yet, running loadjob immediately!\n");
    WaitForJob();
  }

  std::string pathfile = path + fileName;
  std::string line;
  //while ((pathfile.length() + line.length()) < 56) { line += "="; }
  //printf("=[%s]%s\n", pathfile.c_str(), line.c_str());
  {//to measure time
    //CS::MeasureTime lTimeMeasurer ("I: Add To Engine");

    // Get some plugins.
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    csRef<iLoader> loader = csQueryRegistry<iLoader> (object_reg);
    csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
    csRef<iTextureManager> textureManager = csQueryRegistry<iTextureManager> (object_reg);

    //printf("Adding shaders:\n");
    for (size_t i = 0; i < loadJob->shaders.GetSize(); i++)
    {
      csString shaderFile = loadJob->shaders[i];
      csRef<iShader> shader = LoadShader (shaderFile.GetData());
      if (collection && shader.IsValid()) collection->Add(shader->QueryObject());
    }

    //printf("Adding Textures:\n");
    for (size_t i = 0; i < loadJob->textures.GetSize(); i++)
    {
      TexturePrototype tex = loadJob->textures[i];
      csString info;
      // Find a texture with the same filename that has already been loaded.
      csRef<iTextureWrapper> texw = FindTextureInEngine(tex);
      if (texw.IsValid())
      {
        info.Format("(From engine(precached: %s))", texw->GetTextureHandle()->IsPrecached()? "T":"F");
        //Delete the buffer.
        tex.buf.Invalidate();
        tex.texture = texw;
      }
      else if (tex.buf.IsValid())
      {
        info = "(From thread)";
        tex.texture = loader->LoadTexture(tex.name.c_str(), tex.buf, tex.flags, textureManager, true, false, !tex.keepImage);
        SetFileName(tex.texture, tex.usedFileName);
      }
      else
      {
        info = "(W: From main thread!)";
        vfs->ChDir(path.c_str());
        tex.texture = loader->LoadTexture(tex.name.c_str(), tex.fileName.c_str(), tex.flags, textureManager, true, false, !tex.keepImage, collection, KEEP_ALL);
        SetFileName(tex.texture, tex.fileName);
      }
      //printf("- %s %s (%s)(%s)\n", tex.name.c_str(), info.GetData(), tex.fileName.c_str(), tex.usedFileName.c_str());
      if ( tex.texture.IsValid())
      {
        SetTextureProperties(this, tex);
        if (collection && !collection->IsParentOf(tex.texture->QueryObject()))
          collection->Add(tex.texture->QueryObject());
      }
    } // end for
    //lTimeMeasurer.PrintIntermediate ("Textures ");

    //printf("Adding Materials:\n");
    for (size_t i = 0; i < loadJob->materials.GetSize(); i++)
    {
      MaterialPrototype mat = loadJob->materials[i];
      SetMaterialProperties(this, mat, GetTexture(mat.textureName));
      //printf("- %s\n", mat.name.c_str());
      if (collection && mat.materialWrapper.IsValid()) collection->Add(mat.materialWrapper->QueryObject());
    } // end for
    //lTimeMeasurer.PrintIntermediate ("Materials ");

    //printf("Adding Meshes:\n");
    for (size_t i = 0; i < loadJob->prototypes.GetSize(); i++)
    {
      csObjectPrototype proto = loadJob->prototypes[i];

      csRef<iMeshObjectFactory> obj = scfQueryInterface<iMeshObjectFactory>(proto.object);
      obj->SetMaterialWrapper(GetMaterial(proto.materialName));
      csRef<iMeshFactoryWrapper> fact = engine->CreateMeshFactory(obj, proto.name.c_str());
      SetFactoryProperties(this, proto, fact);

      // Parse the children.
      for (size_t i = 0; i < proto.children.GetSize(); i++)
      {
        csObjectPrototype child = proto.children[i];

        csRef<iMeshObjectFactory> childobj = scfQueryInterface<iMeshObjectFactory>(child.object);
        childobj->SetMaterialWrapper(GetMaterial(child.materialName));
        csRef<iMeshFactoryWrapper> childfact = engine->CreateMeshFactory(childobj, child.name.c_str());
        SetFactoryProperties(this, child, fact);

        fact->GetChildren ()->Add (childfact);

        if (child.lodLevel > -1)
        {
          fact->AddFactoryToStaticLOD(child.lodLevel, childfact);
        }
      }

      if (collection && fact.IsValid()) collection->Add(fact->QueryObject());

    } // end for
    //lTimeMeasurer.PrintIntermediate ("Meshes ");

    //Clean up.
    loadJob->textures.DeleteAll();
    loadJob->materials.DeleteAll();
    loadJob->prototypes.DeleteAll();

  }//block, to measure time.
  //printf("===========================================================\n");


  return true;
} // end AddToEngine()
