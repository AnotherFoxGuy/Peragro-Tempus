
#include "colltest.h"

#include "server/database/database.h"
#include "server/database/sqlite/sqlite.h"
#include "server/database/table-sectors.h"
#include "server/database/table-meshes.h"
#include "server/database/table-triangles.h"
#include "server/database/table-vertices.h"

#include "ext/bullet/btBulletDynamicsCommon.h"

CS_IMPLEMENT_APPLICATION

//---------------------------------------------------------------------------

static csReversibleTransform BulletToCS (const btTransform& trans)
{
  const btVector3& trans_o = trans.getOrigin ();
  csVector3 origin (trans_o.getX (), trans_o.getY (), trans_o.getZ ());
  const btMatrix3x3& trans_m = trans.getBasis ();
  const btVector3& row0 = trans_m.getRow (0);
  const btVector3& row1 = trans_m.getRow (1);
  const btVector3& row2 = trans_m.getRow (2);
  csMatrix3 m (
      row0.getX (), row1.getX (), row2.getX (),
      row0.getY (), row1.getY (), row2.getY (),
      row0.getZ (), row1.getZ (), row2.getZ ());
  return csReversibleTransform (m, origin);
}

static btTransform CSToBullet (const csReversibleTransform& tr)
{
  const csVector3& origin = tr.GetOrigin ();
  btVector3 trans_o (origin.x, origin.y, origin.z);
  const csMatrix3& m = tr.GetO2T ();
  btMatrix3x3 trans_m (
      m.m11, m.m21, m.m31,
      m.m12, m.m22, m.m32,
      m.m13, m.m23, m.m33);
  return btTransform (trans_m, trans_o);
}

//---------------------------------------------------------------------------

CollisionDetectionTest::CollisionDetectionTest ()
{
  SetApplicationName ("Peragro.CollisionDetectionTest");
}

CollisionDetectionTest::~CollisionDetectionTest ()
{
}

void CollisionDetectionTest::Frame ()
{
  csTicks elapsed_time = vc->GetElapsedTicks ();

  float speed = (elapsed_time / 1000.0) * (0.06 * 20);

  iCamera* c = view->GetCamera();

  if (kbd->GetKeyState (CSKEY_SHIFT))
  {
    if (kbd->GetKeyState (CSKEY_RIGHT))
      c->Move (CS_VEC_RIGHT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_LEFT))
      c->Move (CS_VEC_LEFT * 4 * speed);
    if (kbd->GetKeyState (CSKEY_UP))
      c->Move (CS_VEC_UP * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN))
      c->Move (CS_VEC_DOWN * 4 * speed);
  }
  else if (kbd->GetKeyState (CSKEY_CTRL))
  {
    btVector3 v(0,0,0);
    btVector3 r(0,0,0);
    if (kbd->GetKeyState (CSKEY_RIGHT))
      r.setY(4);
    if (kbd->GetKeyState (CSKEY_LEFT))
      r.setY(-4);
    if (kbd->GetKeyState (CSKEY_UP))
      v.setX(4);
    if (kbd->GetKeyState (CSKEY_DOWN))
      v.setX(-4);
    pc->activate(true);
    pc->setLinearVelocity(v * pc->getInterpolationWorldTransform().getRotation());
    pc->setAngularVelocity(r);
  }
  else
  {
    if (kbd->GetKeyState (CSKEY_RIGHT))
      rotY += speed;
    if (kbd->GetKeyState (CSKEY_LEFT))
      rotY -= speed;
    if (kbd->GetKeyState (CSKEY_PGUP))
      rotX += speed;
    if (kbd->GetKeyState (CSKEY_PGDN))
      rotX -= speed;
    if (kbd->GetKeyState (CSKEY_UP))
      c->Move (CS_VEC_FORWARD * 4 * speed);
    if (kbd->GetKeyState (CSKEY_DOWN))
      c->Move (CS_VEC_BACKWARD * 4 * speed);
  }

  static btClock clock;

  // slow motion, should be: clock.getTimeMicroseconds() * 0.000001f
  float dt = clock.getTimeMicroseconds() * 0.000001f;
  world->stepSimulation(dt, 1);

  mesh->GetMovable()->SetTransform(BulletToCS(pc->getInterpolationWorldTransform()));

  // reset position if we fell through the floor...
  csVector3 pos = mesh->GetMovable()->GetPosition();
  if (pos.y < 0)
  {
    pos.y = 5;
    mesh->GetMovable()->SetPosition(pos);
    pc->setLinearVelocity(btVector3(0,0,0));
    pc->setWorldTransform(CSToBullet(mesh->GetMovable()->GetTransform()));
  }

  csMatrix3 rot = csXRotMatrix3 (rotX) * csYRotMatrix3 (rotY);
  csOrthoTransform ot (rot, c->GetTransform().GetOrigin ());
  c->SetTransform (ot);
}

bool CollisionDetectionTest::OnKeyboard(iEvent& ev)
{
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
  if (eventtype == csKeyEventTypeDown)
  {
    utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
    if (code == CSKEY_ESC)
    {
      csRef<iEventQueue> q =
        csQueryRegistry<iEventQueue> (GetObjectRegistry());
      if (q.IsValid()) q->GetEventOutlet()->Broadcast(
        csevQuit(GetObjectRegistry()));
    }
  }
  return false;
}

bool CollisionDetectionTest::OnInitialize(int /*argc*/, char* /*argv*/ [])
{
  if (!csInitializer::RequestPlugins(GetObjectRegistry(),
    CS_REQUEST_VFS,
    CS_REQUEST_OPENGL3D,
    CS_REQUEST_ENGINE,
    CS_REQUEST_FONTSERVER,
    CS_REQUEST_IMAGELOADER,
    CS_REQUEST_LEVELLOADER,
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_END))
    return ReportError("Failed to initialize plugins!");

  csBaseEventHandler::Initialize(GetObjectRegistry());

  csEventID events[] = {
    csevFrame (GetObjectRegistry()),
    csevKeyboardEvent (GetObjectRegistry()),
    CS_EVENTLIST_END
  };
  if (!RegisterQueue(GetObjectRegistry(), events))
    return ReportError("Failed to set up event handler!");

  return true;
}

void CollisionDetectionTest::OnExit()
{
  drawer.Invalidate();
  printer.Invalidate();
}

bool CollisionDetectionTest::Application()
{
  if (!OpenApplication(GetObjectRegistry()))
    return ReportError("Error opening system!");

  if (SetupModules()) {
    Run();
  }

  return true;
}

bool CollisionDetectionTest::SetupModules ()
{
  g3d = csQueryRegistry<iGraphics3D> (GetObjectRegistry());
  if (!g3d) return ReportError("Failed to locate 3D renderer!");

  engine = csQueryRegistry<iEngine> (GetObjectRegistry());
  if (!engine) return ReportError("Failed to locate 3D engine!");

  vc = csQueryRegistry<iVirtualClock> (GetObjectRegistry());
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  kbd = csQueryRegistry<iKeyboardDriver> (GetObjectRegistry());
  if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

  loader = csQueryRegistry<iLoader> (GetObjectRegistry());
  if (!loader) return ReportError("Failed to locate Loader!");

  view.AttachNew(new csView (engine, g3d));
  iGraphics2D* g2d = g3d->GetDriver2D ();
  view->SetRectangle (0, 0, g2d->GetWidth (), g2d->GetHeight ());

  engine->SetLightingCacheMode (0);

  dbSQLite db;
  db.init();

  LoadWorld(&db);
  SetupPhysics(&db);
  AddCharacter();

  engine->Prepare ();

  rotY = rotX = 0;

  if (engine->GetSectors()->GetCount() == 0) return false;
  view->GetCamera ()->SetSector (  engine->GetSectors()->Get(0) );
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (86.8f, 2.0f, 11.2f));

  drawer.AttachNew(new FrameBegin3DDraw (GetObjectRegistry (), view));
  printer.AttachNew(new FramePrinter (GetObjectRegistry ()));

  return true;
}

void CollisionDetectionTest::LoadWorld(Database* db)
{
  //Load all Sectors from Database
  Array<SectorsTableVO*> loadedSectors = db->getSectorsTable()->getAll();

  if (loadedSectors.getCount() == 0) return;

  for (size_t i = 0; i < loadedSectors.getCount(); i++)
  {
    engine->CreateSector(*loadedSectors[0]->name);
  }

  iSector* sector = engine->GetSectors()->Get(0);

  if (!loader->LoadTexture ("stone", "/lib/std/stone4.gif"))
    ReportError("Error loading 'stone4' texture!");

  iMaterialWrapper* mat = engine->GetMaterialList ()->FindByName ("stone");

  // Create Mesh
  Array<MeshesTableVO*> meshes = db->getMeshesTable()->getAll();
  for (size_t i = 0; i < meshes.getCount(); i++)
  {
    using namespace CS::Geometry;

    csRef<iMeshFactoryWrapper> fact (
      GeneralMeshBuilder::CreateFactory(engine, *meshes[0]->name, 0)
    );

    csRef<iGeneralFactoryState> gfact =
      scfQueryInterface<iGeneralFactoryState> (fact->GetMeshObjectFactory ());

    Array<VerticesTableVO*> vertices =
      db->getVerticesTable()->getAllByMesh(meshes.get(i)->id);

    for (size_t j = 0; j < vertices.getCount(); j++)
    {
      csVector3 v1 ( vertices[j]->x, vertices[j]->y, vertices[j]->z );
      gfact->AddVertex(v1, csVector2(j % 2, (j / 2) % 2), csVector3(0,0,0), csColor4(1));
    }

    Array<TrianglesTableVO*> triangles =
      db->getTrianglesTable()->getAllByMesh(meshes.get(i)->id);

    for (size_t j = 0; j < triangles.getCount(); j++)
    {
      csTriangle tri(triangles[j]->a, triangles[j]->b, triangles[j]->c);
      gfact->AddTriangle(tri);
    }

    gfact->CalculateNormals();
    gfact->SetColor(csColor(1, 1, 1));

    csRef<iMeshWrapper> mesh (
      GeneralMeshBuilder::CreateMesh(engine, sector, *meshes[0]->name, fact)
    );

    mesh->GetMeshObject ()->SetMaterialWrapper(mat);
  }
}

void CollisionDetectionTest::SetupPhysics(Database* db)
{
  Array<VerticesTableVO*> vertices =
    db->getVerticesTable()->getAll();

  btVector3 worldMin, worldMax;
  for (size_t i = 0; i < vertices.getCount(); i++)
  {
    VerticesTableVO* vertex = vertices.get(i);

    if ( i == 0 || worldMin.getX() < vertex->x ) worldMin.setX(vertex->x);
    if ( i == 0 || worldMin.getY() < vertex->y ) worldMin.setY(vertex->y);
    if ( i == 0 || worldMin.getZ() < vertex->z ) worldMin.setZ(vertex->z);

    if ( i == 0 || worldMax.getX() > vertex->x ) worldMax.setX(vertex->x);
    if ( i == 0 || worldMax.getY() > vertex->y ) worldMax.setY(vertex->y);
    if ( i == 0 || worldMax.getZ() > vertex->z ) worldMax.setZ(vertex->z);
  }

  // Setup World
  btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
  btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
  btAxisSweep3* overlappingPairCache = new btAxisSweep3(worldMin,worldMax);
  btSequentialImpulseConstraintSolver* constraintSolver = new btSequentialImpulseConstraintSolver();
  world = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,constraintSolver,collisionConfiguration);

  // Create Mesh
  Array<MeshesTableVO*> meshes = db->getMeshesTable()->getAll();
  for (size_t i = 0; i < meshes.getCount(); i++)
  {
    btCollisionObject* collObj = new btCollisionObject();

    btMatrix3x3 basis;
    basis.setIdentity();
    collObj->getWorldTransform().setBasis(basis);

    //TODO: reduce amount of queries!
    Array<TrianglesTableVO*> triangles =
      db->getTrianglesTable()->getAllByMesh(meshes.get(i)->id);

    if (triangles.getCount() == 0) continue;

    Array<VerticesTableVO*> vertices =
      db->getVerticesTable()->getAllByMesh(meshes.get(i)->id);

    //TODO: Use indexed triangle mesh!
    btTriangleMesh* mesh = new btTriangleMesh();

    mesh->preallocateIndices(triangles.getCount());
    mesh->preallocateVertices(vertices.getCount());

    for (size_t j = 0; j < triangles.getCount(); j++)
    {
      TrianglesTableVO* t = triangles.get(j);

      VerticesTableVO* v0 = vertices.get(t->a);
      btVector3 btV0(v0->x, v0->y, v0->z);

      VerticesTableVO* v1 = vertices.get(t->b);
      btVector3 btV1(v1->x, v1->y, v1->z);

      VerticesTableVO* v2 = vertices.get(t->c);
      btVector3 btV2(v2->x, v2->y, v2->z);

      mesh->addTriangle(btV0, btV1, btV2);
    }

    btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true);

    collObj->setCollisionShape(shape);

    collObj->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);

    world->addCollisionObject(collObj);
  }
}

void CollisionDetectionTest::AddCharacter()
{
  btBoxShape* shape = new btBoxShape(btVector3(1,2,1));

  pc = new btRigidBody(1.0f, 0, shape);
  pc->setFriction(1);

  btTransform& t = pc->getWorldTransform();
  btVector3 p(86.8f, 4.0f, 11.2f);
  t.setOrigin(p);
  pc->setWorldTransform(t);

  world->addRigidBody(pc);

  using namespace CS::Geometry;
  Box box (csVector3 (-1,-2,-1), csVector3 (1,2,1));

  iSector* sector = engine->GetSectors()->Get(0);
  if (!sector) return;

  mesh = GeneralMeshBuilder::CreateFactoryAndMesh (
        engine, sector, "cube", "cubeFact", &box);

  iMaterialWrapper* mat = engine->FindMaterial("stone");
  mesh->GetMeshObject ()->SetMaterialWrapper (mat);
}

/*-------------------------------------------------------------------------*
 * Main function
 *-------------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  return csApplicationRunner<CollisionDetectionTest>::Run (argc, argv);
}
