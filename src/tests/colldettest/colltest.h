
#ifndef COLLTEST_H
#define COLLTEST_H

#include <crystalspace.h>

class CollisionDetectionTest : public csApplicationFramework, public csBaseEventHandler
{
 private:
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iView> view;

  /// Current orientation of the camera.
  float rotX, rotY;

  csRef<FrameBegin3DDraw> drawer;
  csRef<FramePrinter> printer;

 public:
  bool SetupModules ();

  bool OnKeyboard (iEvent&);

  void Frame ();
  
  void LoadWorld ();
    
  CollisionDetectionTest ();
  ~CollisionDetectionTest ();

  void OnExit ();
  bool OnInitialize (int argc, char* argv[]);

  bool Application ();
};

#endif // COLLTEST_H
