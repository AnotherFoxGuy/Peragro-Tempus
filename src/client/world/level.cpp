#include <crystalspace.h>

#include "level.h"
#include "world.h"
#include "modelmanager.h"
#include "factory.h"
#include "interiormanager.h"
#include "interior.h"
#include "levelloader/levelloader.h"


Level::Level(World* world)
{
  this->object_reg = world->GetObjectRegistry();
  modelManager =  world->GetModelManager();
  interiorManager = world->GetInteriorManager();

  levelLoader = 0;
  finished = false;

  missingData.AttachNew(new MissingData(this));
}

Level::~Level()
{
  //printf("Unloading Level %s\n", regionName.c_str());

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  // Remove callback.
  if (cb.IsValid())
  {
    engine->RemoveEngineFrameCallback(cb);
  }

  // Unload instances.
  if (instances.IsValid())
  {
    instances->DeleteAll();
    engine->GetRegions()->Remove(instances);
  }

  // Unload factories.
  // Just empty the array to decrease ref.
  factories.Empty();

  // Delete the tileloader.
  if (levelLoader) delete levelLoader;
  levelLoader = 0;
}

void Level::OpenFile(const std::string& path, const std::string& fileName)
{
  // Create the threaded fileloader.
  levelLoader = new LevelLoader(object_reg);
  levelLoader->Load(path, fileName);

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  // Create instances' region.
  instances = engine->CreateRegion(regionName.c_str());

  // Wait for the resources to be loaded.
  cb.AttachNew(new FrameCallBack(this));
  engine->AddEngineFrameCallback(cb);

} // end OpenFile()

void Level::ParseFactories()
{
  csArray<std::string>* facts = levelLoader->GetFactories();
  if (!facts) return;

  // For each library tag.
  for (size_t i =0; i < facts->GetSize(); i++)
  {
    csRef<Factory> fact = modelManager->Get(facts->Get(i));
    factories.Push(fact);
  }

} // end ParseFactories()

void Level::ParseInstances()
{
  csRefArray<iDocumentNode>* insts = levelLoader->GetInstances();
  if (!insts) return;

  // For each meshobj tag.
  for (size_t i =0; i < insts->GetSize(); i++)
  {
    instanceNodes.Push(insts->Get(i));
  }

} // end ParseInstances()

void Level::ParseInteriors()
{
  csRefArray<iDocumentNode>* ints = levelLoader->GetInteriors();
  if (!ints) return;

  // For each meshobj tag.
  for (size_t i =0; i < ints->GetSize(); i++)
  {
    csRef<Interior> interior = interiorManager->Get(ints->Get(i));
    interiors.Push(interior);
    // TODO: just load it for now.
    interior->Load();
  }

} // end ParseInteriors()

bool Level::CheckResources()
{
  /*
  static int counter = 0;
  counter++;
  if (counter < 2) return false;
  counter = 0;
  */

  if (levelLoader && !levelLoader->IsReady())
    return false;
  else if (levelLoader)
  {
    ParseFactories();
    ParseInstances();
    ParseInteriors();
    delete levelLoader;
    levelLoader = 0;
  }

  // Wait till all loaders have finished.
  for (size_t i = 0; i < factories.GetSize(); i++)
  {
    Factory* fact = factories.Get(i);
    if (fact->IsReady() && !fact->IsAdded())
    {
      fact->AddToEngine();
      return false;
    }
    else if (!fact->IsReady())
      return false;
  }

  // Wait till everything is precached.
  for (size_t i = 0; i < factories.GetSize(); i++)
  {
    Factory* fact = factories.Get(i);
    if (!fact->IsPrecached())
    {
      fact->Precache();
      return false;
    }
  }


  while (!instanceNodes.IsEmpty())
  {
    csRef<iDocumentNode> node = instanceNodes.Get(0);
    instanceNodes.DeleteIndex(0);
    LoadInstance(node);
    return false;
  }

  finished = true;
  
  return true;

} // end CheckResources()

void Level::FrameCallBack::StartFrame(iEngine* engine, iRenderView* rview)
{
  if (!level || level->CheckResources())
  {
    engine->RemoveEngineFrameCallback(this);
  }
} // end StartFrame()

iMeshFactoryWrapper* Level::MissingData::MissingFactory(const char* name)
{
  if (!level) return 0;

  for (size_t i = 0; i < level->factories.GetSize(); i++)
  {
    Factory* fact = level->factories.Get(i);
    if (fact->IsReady() && fact->IsAdded())
    {
      iRegion* region = fact->GetRegion();
      if (region)
      {
        iMeshFactoryWrapper* mesh = region->FindMeshFactory(name);
        if (mesh) return mesh;
      }
    }
  }

  return 0;
} // end MissingFactory()

iTextureWrapper* Level::MissingData::MissingTexture(const char* name, const char* filename)
{
  if (!level) return 0;

  for (size_t i = 0; i < level->factories.GetSize(); i++)
  {
    Factory* fact = level->factories.Get(i);
    if (fact->IsReady() && fact->IsAdded())
    {
      iRegion* region = fact->GetRegion();
      if (region)
      {
        iTextureWrapper* tex = region->FindTexture(name);
        if (tex) return tex;
      }
    }
  }

  return 0;
} // end MissingFactory()
