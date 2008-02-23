#include <crystalspace.h>

#include "factory.h"
#include "factoryloader/fileloader.h"


Factory::Factory(const std::string& fileName, 
                 iObjectRegistry* object_reg) : scfImplementationType (this)
{
  this->fileName = fileName;
  this->object_reg = object_reg;
  fileLoader = 0;
  isPrecached = false;
  isAdded = false;
}

Factory::~Factory()
{
  if (fileLoader)
  {
    delete fileLoader;
    fileLoader = 0;
  }

  if (region.IsValid())
  {
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    iRegionList* list = engine->GetRegions();
    list->Remove(region);
  }
}

void Factory::Load()
{
  fileLoader = new FileLoader(object_reg);

  // Seperate the path of the filename.
  size_t p = fileName.find_last_of("/");
  std::string path = fileName.substr(0,p+1);
  std::string file = fileName.substr(p+1,fileName.length());

  // Create our region.
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  region = engine->CreateRegion(fileName.c_str());

  fileLoader->Load(path, file, region);
} // end Load()

bool Factory::IsReady() const
{
  if (fileLoader) 
    return fileLoader->IsReady();
  else
    return true;
} // end IsReady()

void Factory::AddToEngine()
{
  if (!IsReady()) return;

  fileLoader->AddToEngine();
  delete fileLoader;
  fileLoader = 0;
  isAdded = true;
} // end AddToEngine()

void Factory::Precache()
{
  if (!IsReady()) return;
  if (!region.IsValid()) return;
  if (IsPrecached()) return;

  // Precaches "one" texture and then returns, so you have to call 
  // this repeately and check IsPrecached() wether 'everything' is done.
  csRef<iObjectIterator> iter = region->QueryObject()->GetIterator();
  while (iter->HasNext ())
  {
    csRef<iTextureWrapper> csth (scfQueryInterface<iTextureWrapper> (iter->Next ()));
    if (csth)
    {
      if (csth->GetTextureHandle ()) 
      {
        if (csth->GetTextureHandle ()->IsPrecached())
          continue;
        else
        {
          csth->GetTextureHandle()->Precache();
          printf("I: Precached texture '%s'\n", csth->QueryObject()->GetName());
          return;
        }
      }
    }
  }

  isPrecached = true;
} // end Precache()
