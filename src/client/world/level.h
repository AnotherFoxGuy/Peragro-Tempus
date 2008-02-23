#ifndef LEVEL_H
#define LEVEL_H

#include <string>

class World;
class ModelManager;
struct Factory;
class InteriorManager;
struct Interior;
class LevelLoader;

class Level 
{
private: 
  struct MissingData;

protected:
  bool finished;
  LevelLoader* levelLoader;
  std::string regionName;

  csRef<MissingData> missingData;

  iObjectRegistry* object_reg;
  ModelManager* modelManager;
  InteriorManager* interiorManager;

  // Resources.
  csRefArray<Factory> factories;
  csRef<iRegion> instances;
  csRefArray<iDocumentNode> instanceNodes;
  csRefArray<Interior> interiors;

  void OpenFile(const std::string& path, const std::string& fileName);
  virtual void LoadInstance(iDocumentNode* meshNode) {}

private:
  void ParseFactories();
  void ParseInstances();
  void ParseInteriors();
  bool CheckResources();
 
  // Check the status and process the resources each frame.
  struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
  {
    Level* level;
    FrameCallBack (Level* lev) : scfImplementationType (this) { level = lev; }
    virtual void StartFrame (iEngine* engine, iRenderView* rview);
  };
  friend struct FrameCallBack;
  csRef<FrameCallBack> cb;

  // Search for missing data in our own Factories.
  struct MissingData : public scfImplementation1<MissingData, iMissingLoaderData>
  {
    Level* level;
    MissingData (Level* lev) : scfImplementationType (this) { level = lev; }

    virtual iMaterialWrapper* MissingMaterial (const char* name, const char* filename) { return 0; }
    virtual iTextureWrapper* MissingTexture (const char* name, const char* filename);
    virtual iShader* MissingShader (const char* name) { return 0; }
    virtual iMeshFactoryWrapper* MissingFactory (const char* name);
    virtual iMeshWrapper* MissingMesh (const char* name) { return 0; }
    virtual iSector* MissingSector (const char* name){ return 0; }
    virtual iLight* MissingLight (const char* name) { return 0; }
  };
  friend struct MissingData;

public:
  Level(World* world);
  virtual ~Level();

  /// @return True if finished loading, false otherwise.
  bool IsReady() const { return finished; }
};

#endif // LEVEL_H
