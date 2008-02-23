#ifndef INTERIORMANAGER_H
#define INTERIORMANAGER_H

#include <string>

struct iObjectRegistry;
struct Interior;
struct InteriorFactory;
class World;

class InteriorManager 
{
private:
  bool finished;
  csWeakRefArray<Interior> interiors;
  csWeakRefArray<InteriorFactory> interiorFactories;
  csArray<std::string> toLoad;

private:
  World* world;
  iObjectRegistry* object_reg;

private:
  bool CheckResources();

  // Check the status and process the resources each frame.
  struct FrameCallBack : public scfImplementation1<FrameCallBack, iEngineFrameCallback>
  {
    InteriorManager* intmgr;
    FrameCallBack (InteriorManager* interiorManager) : scfImplementationType (this) { intmgr = interiorManager; }
    virtual void StartFrame (iEngine* engine, iRenderView* rview);
  };
  friend struct FrameCallBack;
  csRef<FrameCallBack> cb;

public:
  InteriorManager(World* world);
  ~InteriorManager();

  /// Get or create a new Factory. 
  csRef<Interior> Get(iDocumentNode* node);

  /// Registers a interior to be loaded the moment it is created. 
  void RegisterLoad(const std::string& interiorName);

  /// @return True if finished loading, false otherwise.
  bool IsReady() const { return finished; }

};

#endif // INTERIORMANAGER_H
