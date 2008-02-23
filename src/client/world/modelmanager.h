#ifndef MODELMANAGER_H
#define MODELMANAGER_H

#include <string>

struct iObjectRegistry;
struct Factory;

class ModelManager 
{
private:
  csWeakRefArray<Factory> factories;

private:
  iObjectRegistry* object_reg;

public:
  ModelManager(iObjectRegistry* object_reg);
  ~ModelManager();

  /// Get or create a new Factory. 
  csRef<Factory> Get(const std::string& fileName);

};

#endif // MODELMANAGER_H
