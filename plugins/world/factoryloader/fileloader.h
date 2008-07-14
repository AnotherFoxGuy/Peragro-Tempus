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

#ifndef LODTHREAD_H
#define LODTHREAD_H

#include <crystalspace.h>
#include <iutil/job.h>

#include <string>

struct iEngine;
struct iVFS;
struct iObjectRegistry;
struct iLoader;
struct iCollection;

//---------------------------------------------------------------------------

struct GeneralMeshSubMeshWrapper
{
  csRef<iGeneralMeshSubMesh> submesh;
  std::string materialName;
  csRefArray<iDocumentNode> shaderVariables;
};

struct csObjectPrototype
{
  csObjectPrototype () : failedLoading(true), nullMesh(false), lodLevel(-1) {}
  bool failedLoading;

  bool nullMesh;
  csBox3 boundingbox;

  //LOD stuff.
  csRef<iDocumentNode> staticLodNode;
  int lodLevel;

  //Children
  csRefArray<iDocumentNode> childNodes;
  csArray<csObjectPrototype> children;

  // General
  std::string name;
  csRef<iBase> object;
  std::string materialName;

  // Transforms
  csReversibleTransform move;
  csReversibleTransform hardMove;

  csRefArray<iDocumentNode> shaderVariables;
  csArray<GeneralMeshSubMeshWrapper> submeshes;

  // Flags
  csFlags flags;
  std::string priority;
  csZBufMode zBuffMode;

  csRef<iCollider> collider;
};

struct TexturePrototype
{
  std::string name;
  std::string fileName;
  std::string usedFileName;
  csRef<iTextureWrapper> texture;
  csRef<iDataBuffer> buf;

  std::string type;
  std::string textureClass;
  /*csFlags*/uint32 flags;
  bool keepImage;
  csColor Transparency;
  csAlphaMode::AlphaType alphaType;
  bool alwaysAnimate;
  csRefArray<iDocumentNode> keyValuePairs;
};

struct MaterialPrototype
{
  std::string name;
  std::string textureName;
  csRef<iMaterial> material;
  csRef<iMaterialWrapper> materialWrapper;

  csArray<csString> shadertypes;
  csArray<csString> shaders;
  csRefArray<iDocumentNode> shaderVariables;
};

struct ObjectPlugin
{
  std::string name;
  std::string classId;
  csRef<iMeshObjectType> plugin;
};

//---------------------------------------------------------------------------


class FileLoader
{

protected:

  class LoaderJob : public scfImplementation1<LoaderJob, iJob>
  {
  public:
    //Plugins.
    csRef<iVFS> vfs;
    csRef<iSyntaxService> SyntaxService;
    csRef<iCollideSystem> collide_system;
    csArray<ObjectPlugin> plugins;

    //Misc
    csStringID basemesh_id;
    csStringID trianglemesh_id;

    //Data
    bool finished;

    std::string path;
    std::string fileName;

    csArray<csString> shaders;
    csArray<TexturePrototype> textures;
    csArray<MaterialPrototype> materials;
    csArray<csObjectPrototype> prototypes;

    // Create new instance.
    LoaderJob (const std::string& path, const std::string& fileName);
    virtual ~LoaderJob();

    virtual void Run();

    csPtr<iMeshObjectFactory> NewFactory(const std::string& plug);

    void ParseLibrary(iDocumentNode* libraryNode);
    void ParseTexture(iDocumentNode* node);
    void ParseMaterial(iDocumentNode* node);
    void ParseMeshFact(iDocumentNode* node, csObjectPrototype& proto, csObjectPrototype* parent);
  };

protected:
  /// Reference to the job for loading this file.
  mutable csRef<LoaderJob> loadJob;
  /// Reference to job queue.
  mutable csRef<iJobQueue> jobQueue;

  iObjectRegistry* object_reg;
  csRef<iCollection> collection;

private:
  std::string path;
  std::string fileName;

  std::string GetFileName(iTextureWrapper* tex);
  void SetFileName(iTextureWrapper* tex, const std::string& fileName);
  iTextureWrapper* FindTextureInEngine(TexturePrototype& tex);

private:
  struct LoaderContext : public scfImplementation1<LoaderContext, iLoaderContext>
  {
    FileLoader* fileLoader;
    LoaderContext (FileLoader* loader) : scfImplementationType (this) { fileLoader = loader; }

    virtual iSector* FindSector (const char* name) { return 0; }
    virtual iMaterialWrapper* FindMaterial (const char* name)  { return fileLoader->GetMaterial(name); }
    virtual iMaterialWrapper* FindNamedMaterial (const char* name, const char *filename)  { return 0; }
    virtual iMeshFactoryWrapper* FindMeshFactory (const char* name)  { return 0; }
    virtual iMeshWrapper* FindMeshObject (const char* name)  { return 0; }
    virtual iTextureWrapper* FindTexture (const char* name)  { return fileLoader->GetTexture(name); }
    virtual iTextureWrapper* FindNamedTexture (const char* name, const char* filename)  { return 0; }
    virtual iLight* FindLight (const char* name) { return 0; }
    virtual iShader* FindShader (const char* name) { return 0; }
    virtual bool CheckDupes () const  { return true; }
    virtual iCollection* GetCollection () const  { return 0; }
    virtual bool CurrentCollectionOnly () const { return true; }
    virtual uint GetKeepFlags() const { return 0; }
  };
  csRef<LoaderContext> loaderContext;

public:
  FileLoader (iObjectRegistry* object_reg);
  virtual ~FileLoader ();

  virtual bool Load (const std::string& path, const std::string& fileName, iCollection* collection=0);

  void WaitForJob() const;

  bool IsReady() const;

  bool AddToEngine();

  // Not threadsafe, call when ready only!
  iMaterialWrapper* GetMaterial(const std::string& name);
  iTextureWrapper* GetTexture(const std::string& name);
  iObjectRegistry* GetObjectRegistry() { return object_reg; }
  LoaderContext* GetLoaderContext() { return loaderContext; }

  csRef<iShader> LoadShader (const char* filename);
};

#endif // LODTHREAD_H
