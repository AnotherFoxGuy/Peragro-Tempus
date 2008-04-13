#ifndef _TESTING_H
#define _TESTING_H

#include <crystalspace.h>

class Simple : public csApplicationFramework, public csBaseEventHandler
{
private:
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iVFS> vfs;

  void ProcessFrame ();
  void FinishFrame ();

public:
  Simple ();
  ~Simple ();

  void OnExit ();
  bool OnInitialize (int argc, char* argv[]);

  bool Application ();

//  int Main(int argc, char *argv[]);

  CS_EVENTHANDLER_NAMES("application.simple1")
  CS_EVENTHANDLER_NIL_CONSTRAINTS
};

#endif
