#include "testing.h"
#include <iostream>

using namespace std;

CS_IMPLEMENT_APPLICATION

Simple::Simple ()
{
  SetApplicationName ("CrystalSpace.Simple1");
}

Simple::~Simple ()
{
}

void Simple::ProcessFrame ()
{
}

void Simple::FinishFrame ()
{
}

bool Simple::OnInitialize(int argc, char* argv[])
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
  if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
    return ReportError("Failed to set up event handler!");

  return true;
}

void Simple::OnExit()
{
}

bool Simple::Application()
{
  if (!OpenApplication(GetObjectRegistry()))
    return ReportError("Error opening system!");

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

  vfs = csQueryRegistry<iVFS> (GetObjectRegistry());
  if (!vfs) return ReportError("Failed to locate VFS!");



  csRef<iDocumentSystem> docsys;
  docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem());

  csRef<iDataBuffer> buf = vfs->ReadFile("/this/tile--1--1");

  csRef<iDocument> doc;
  doc = docsys->CreateDocument();
  doc->Parse(buf, true);
  csRef<iDocumentNode> root = doc->GetRoot();

  csRef<iDocument> doc2;
  doc2 = docsys->CreateDocument ();
  csRef<iDocumentNode> root2 = doc2->CreateRoot();
  CS::DocSystem::CloneNode(root, root2);

  doc2->Write(vfs,"/this/doc2.xml");

  return true;


  return true;
  Run();

  return true;
}

/*---------------*
 * Main function
 *---------------*/
int main (int argc, char* argv[])
{
  return csApplicationRunner<Simple>::Run (argc, argv);
}
