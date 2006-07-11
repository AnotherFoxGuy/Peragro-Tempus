/*
    Copyright (C) 2006 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "cssysdef.h"
#include "cstool/initapp.h"
#include "csutil/syspath.h"

#include "anvil.h"
#include "engine.h"

CS_IMPLEMENT_APPLICATION

#ifdef USE_IDLE
BEGIN_EVENT_TABLE(AnvilApp, wxApp)
  EVT_IDLE(AnvilApp::OnIdle)
END_EVENT_TABLE()
#endif


IMPLEMENT_APP(AnvilApp)

/*---------------------------------------------------------------------*
 * Main function
 *---------------------------------------------------------------------*/
bool AnvilApp::OnInit(void)
{
  engine = new anvEngine();

#if defined(wxUSE_UNICODE) && wxUSE_UNICODE
  char** csargv;
  csargv = (char**)malloc(sizeof(char*) * argc);
  for(int i = 0; i < argc; i++) 
  {
    csargv[i] = strdup (wxString(argv[i]).mb_str().data());
  }
  object_reg = csInitializer::CreateEnvironment (argc, csargv);
  setenv("APPDIR", csInstallationPathsHelper::GetAppDir(csargv[0]), true);
#else
  object_reg = csInitializer::CreateEnvironment (argc, argv);
  setenv("APPDIR", csInstallationPathsHelper::GetAppDir(argv[0]), true);
#endif

  wxScreenDC dc;
  wxCoord w, h;
  dc.GetSize(&w, &h);

  anvMainFrame *frame = new anvMainFrame(_T("Anvil"), engine, wxDefaultPosition, wxSize(w/2, h/2));
  engine->Initialize(object_reg, frame->GetPanel3d());

  SetTopWindow(frame);

#ifdef USE_TIMER
  /* This triggers a timer event every 20 milliseconds, which will yield
   a maximum framerate of 1000/20 = 50 FPS.  Obviously if it takes longer
   than 20 ms to render a frame the framerate will be lower :-)
   You may wish to tweak this for your own application.  Note that
   this also lets you throttle the CPU usage of your app, because
   the application will yield the CPU and wait for events in the
   time between when it completes rendering the current frame and
   the timer triggers the next frame.
  */

  pump = new Pump(frame);
  pump->Start(20);
#endif


  return true;
}

#ifdef USE_IDLE
void AnvilApp::OnIdle() {
  frame->PushFrame();
}
#endif

int AnvilApp::OnExit()
{
  delete engine;
  delete frame;

#ifdef USE_TIMER
  delete pump;
#endif

  csInitializer::DestroyApplication (object_reg);
  return 0;
}
