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

#include <cssysdef.h>
#include <iutil/job.h>
#include <csutil/scf_implementation.h>
#include <csutil/ref.h>
#include <csutil/refarr.h>

#include <string>

struct iEngine;
struct iVFS;
struct iObjectRegistry;
struct iLoader;
struct iCollection;
struct iMeshFactoryWrapper;
struct iDocumentNode;
struct iTriangleMesh;
struct iCollideSystem;
struct iTextureWrapper;
struct iThreadReturn;
struct iCollider;

//---------------------------------------------------------------------------

class FileLoader
{

protected:
  class ParserJob : public scfImplementation1<ParserJob, iJob>
  {
  public:
    //Plugins.
    csRef<iVFS> vfs;

    //Data
    bool finished;

    std::string path;
    std::string fileName;

    csArray<std::string> shaders;
    csRefArray<iDocumentNode> textures;
    csRefArray<iDocumentNode> materials;
    csRefArray<iDocumentNode> factories;

    // Create new instance.
    ParserJob (const std::string& path, const std::string& fileName);
    virtual ~ParserJob();

    virtual void Run();

    bool IsFinished() const { return finished; }

    void ParseLibrary(iDocumentNode* libraryNode);
  };

  class CDJob : public scfImplementation1<CDJob, iJob>
  {
    bool finished;

    iObjectRegistry* object_reg;
    csRef<iMeshFactoryWrapper> fact;

    csRef<iCollideSystem> collide_system;
    csRef<iTriangleMesh> obj_trimesh;
    csRef<iCollider> collider;

  public:
    // Create new instance.
    CDJob (iObjectRegistry* object_reg, iMeshFactoryWrapper* fact);
    virtual ~CDJob();

    virtual void Pre();
    virtual void Run();
    virtual void Post();

    bool IsFinished() const { return finished; }
  };

protected:
  /// Reference to the job for loading this file.
  mutable csRef<ParserJob> parseJob;
  /// Reference to the job for loading generating CD.
  mutable csRefArray<CDJob> cdJobs;
  /// Reference to job queue.
  mutable csRef<iJobQueue> jobQueue;

  iObjectRegistry* object_reg;

private:
  std::string path;
  std::string fileName;
  csRef<iCollection> collection;
  bool finished;

  void AddToCollection(iBase* base);
  std::string GetFileName(iTextureWrapper* tex);
  void SetFileName(iTextureWrapper* tex, const std::string& fileName);
  iTextureWrapper* FindTextureInEngine(const std::string& texName, const std::string& fileName);

  void ParseShader(const std::string& fileName);
  void ParseTexture(iDocumentNode* node);
  void ParseMaterial(iDocumentNode* node);
  void ParseMeshFact(iDocumentNode* node);

  unsigned int parsedShaders;
  unsigned int parsedTextures;
  unsigned int parsedMaterials;
  unsigned int parsedFactories;

  void Parse(unsigned int TypeId, bool clean);
  bool IsDoneLoading();

  csRefArray<iThreadReturn> shaders;
  csRefArray<iThreadReturn> textures; csArray<std::string> textureFileNames;
  csRefArray<iThreadReturn> materials;
  csRefArray<iThreadReturn> factories;

  csRefArray<iThreadReturn> all;

public:
  FileLoader (iObjectRegistry* object_reg);
  virtual ~FileLoader ();

  virtual bool Load (const std::string& path, const std::string& fileName, iCollection* collection);
  bool Process();

  bool IsReady() const { return finished; }

  iObjectRegistry* GetObjectRegistry() { return object_reg; }
};

#endif // LODTHREAD_H
