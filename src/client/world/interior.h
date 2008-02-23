#ifndef INTERIOR_H
#define INTERIOR_H

#include "level.h"

struct iInterior : public virtual iBase
{
  SCF_INTERFACE(iInterior, 1,0,0);
};

struct Interior;
struct InteriorFactory : public scfImplementation1<InteriorFactory, iInterior>, Level
{
private:
  csRefArray<iDocumentNode> instanceNodes;
  void LoadInstance(iDocumentNode* meshNode);

public:
  InteriorFactory(const std::string& fileName, World* world);
  ~InteriorFactory();

  std::string fileName;

  friend struct Interior;
};

struct Interior : public scfImplementation1<Interior, iInterior>
{
private:
  bool finished;
  bool loading;
  csRef<iDocumentNode> interiorNode;
  csRef<InteriorFactory> factory;
  csRef<iRegion> region;
  csRef<iSector> sector;
  csVector3 position;

  float GetPortalYRotation(iMeshWrapper* portalMesh);
  csVector3 GetPortalPosition(iMeshWrapper* portalMesh);
  bool IsPortal(iMeshWrapper* portalMesh); 

  void LoadInstance(iDocumentNode* meshNode);

  bool CheckResources();
  // Check the status and process the resources each frame.
  struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
  {
    Interior* inter;
    FrameCallBack (Interior* interior) : scfImplementationType (this) { inter = interior; }
    virtual void StartFrame (iEngine* engine, iRenderView* rview);
  };
  friend struct FrameCallBack;
  csRef<FrameCallBack> cb;

public:
  Interior(iDocumentNode* interiorNode, InteriorFactory* factory);
  ~Interior();

  std::string interiorName;

  void Load();

  /// @return True if finished loading, false otherwise.
  bool IsReady() const { return finished; }
};

#endif // INTERIOR_H
