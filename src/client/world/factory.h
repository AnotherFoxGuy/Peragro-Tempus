#ifndef FACTORY_H
#define FACTORY_H

#include <string>

struct iObjectRegistry;
class FileLoader;

struct iModelFactory : public virtual iBase
{
  SCF_INTERFACE(iModelFactory, 1,0,0);
};

struct Factory : public scfImplementation1<Factory, iModelFactory>
{
private:
  csRef<iRegion> region;
  FileLoader* fileLoader;
  iObjectRegistry* object_reg;
  bool isPrecached;
  bool isAdded;

public:
  Factory(const std::string& fileName, iObjectRegistry* object_reg);
  ~Factory();
  std::string fileName;
  /// Don't call this function yourself!
  void Load();
  /// @return True if finished loading, false otherwise.
  bool IsReady() const;
  /// Return true if all textures have been precached.
  bool IsPrecached() const { return isPrecached; }
  /// Check if everything has been added to the engine.
  bool IsAdded() const { return isAdded; }
  /** Add the contents to the engine.
   *  Careful, this will block when it isn't ready yet!
   */
  void AddToEngine();
  /** Precaches 'one' texture (if any) and returns.
   *  Call this repeatetly over a period of time
   *  to smooth out texture loading.
   *  Call IsPrecached() to see if everything has been precached.
   */
  void Precache();
  /// Get the region that is used internally for holding the data.
  iRegion* GetRegion() { return region; }
};

#endif // FACTORY_H
