
#ifndef COLLTEST_H
#define COLLTEST_H

#include <crystalspace.h>

class Database;
class Tables;
class btDiscreteDynamicsWorld;
class btRigidBody;

class CollisionDetectionTest : public csApplicationFramework, public csBaseEventHandler
{
 private:
  csRef<iEngine> engine;
  csRef<iLoader> loader;
  csRef<iGraphics3D> g3d;
  csRef<iKeyboardDriver> kbd;
  csRef<iVirtualClock> vc;
  csRef<iView> view;

  btDiscreteDynamicsWorld* world;
  btRigidBody* pc;
  csRef<iMeshWrapper> mesh;

  /// Current orientation of the camera.
  float rotX, rotY;

  csRef<FrameBegin3DDraw> drawer;
  csRef<FramePrinter> printer;

 public:
  bool SetupModules ();

  bool OnKeyboard (iEvent&);

  void Frame ();

  void LoadWorld (Tables* db);
  void SetupPhysics (Tables* db);
  void AddCharacter ();

  CollisionDetectionTest ();
  ~CollisionDetectionTest ();

  void OnExit ();
  bool OnInitialize (int argc, char* argv[]);

  bool Application ();
};

#endif // COLLTEST_H
