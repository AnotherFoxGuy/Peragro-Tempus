#include <crystalspace.h>

#include "modelmanager.h"
#include "factory.h"


ModelManager::ModelManager(iObjectRegistry* object_reg)
{
  this->object_reg = object_reg;
}

ModelManager::~ModelManager()
{
}

csRef<Factory> ModelManager::Get(const std::string& fileName)
{
  // Search.
  factories.Compact();
  for (size_t i = 0; i < factories.GetSize(); i++)
  {
    csRef<Factory> current = csRef<Factory>(factories.Get(i));
    if (current->fileName == fileName)
    {
      return current;
    }
  }

  // Not found: Create a new one.
  csRef<Factory> fact; fact.AttachNew(new Factory(fileName, object_reg));
  fact->Load();
  factories.Push(fact);

  return fact;
} // end Get()
