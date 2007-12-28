
#include "colltest.h"

#include "server/database/database.h"
#include "server/database/sqlite/sqlite.h"
#include "server/database/table-sectors.h"
#include "server/database/table-meshes.h"
#include "server/database/table-triangles.h"
#include "server/database/table-vertices.h"

CS_IMPLEMENT_APPLICATION

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
  //g3d->SetRenderState(G3DRENDERSTATE_EDGES, 1);
  //g3d->SetRenderState(G3DRENDERSTATE_LIGHTINGENABLE, 0);

  LoadWorld();

  engine->SetClearZBuf(true);

  engine->Prepare ();

  rotY = rotX = 0;

  view->GetCamera ()->SetSector (  engine->GetSectors()->Get(0) );
  view->GetCamera ()->GetTransform ().SetOrigin (csVector3 (0, 5, -3));

  drawer.AttachNew(new FrameBegin3DDraw (GetObjectRegistry (), view));
  printer.AttachNew(new FramePrinter (GetObjectRegistry ()));

  return true;
}

void CollisionDetectionTest::LoadWorld()
{
  dbSQLite db;
  db.init();

  //Load all Sectors from Database
  Array<SectorsTableVO*> loadedSectors = db.getSectorsTable()->getAll();
  for (size_t i = 0; i < loadedSectors.getCount(); i++)
  {
    engine->CreateSector(*loadedSectors[0]->name);
  }

  iSector* sector = engine->GetSectors()->Get(0);

  iMaterialWrapper* mat = engine->CreateMaterial("none", 0);

  // Create Mesh
  Array<MeshesTableVO*> meshes = db.getMeshesTable()->getAll();
  for (size_t i = 0; i < meshes.getCount(); i++)
  {
    using namespace CS::Geometry;

    csRef<iMeshFactoryWrapper> fact ( 
      GeneralMeshBuilder::CreateFactory(engine, *meshes[0]->name, 0)
    );

    csRef<iGeneralFactoryState> gfact = 
      scfQueryInterface<iGeneralFactoryState> (fact->GetMeshObjectFactory ());

    Array<VerticesTableVO*> vertices = 
      db.getVerticesTable()->getAllByMesh(meshes.get(i)->id);

    for (size_t j = 0; j < vertices.getCount(); j++)
    {
      csVector3 v1 ( vertices[0]->x, vertices[0]->y, vertices[0]->z );
      gfact->AddVertex(v1, csVector2(0), csVector3(0,1,0), csColor4(0));
    }

    Array<TrianglesTableVO*> triangles = 
      db.getTrianglesTable()->getAllByMesh(meshes.get(i)->id);

    for (size_t j = 0; j < triangles.getCount(); j++)
    {
      csTriangle tri(triangles[0]->a, triangles[0]->b, triangles[0]->c);
      gfact->AddTriangle(tri);
    }

    csRef<iMeshWrapper> mesh ( 
      GeneralMeshBuilder::CreateMesh(engine, sector, *meshes[0]->name, fact)
    );

    mesh->GetMeshObject ()->SetMaterialWrapper(mat);
  }
}

/*-------------------------------------------------------------------------*
 * Main function
 *-------------------------------------------------------------------------*/
int main (int argc, char* argv[])
{
  return csApplicationRunner<CollisionDetectionTest>::Run (argc, argv);
}
