/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "client/console/console.h"

#include <iutil/objreg.h>
#include <ivaria/conin.h>
#include <ivaria/conout.h>

#include "client/client.h"

PtConsole::PtConsole ()
{
}

PtConsole::~PtConsole ()
{
}

//--------------------------------------------------------------------------
class cmdPerfTest : public scfImplementation1<cmdPerfTest, iCelConsoleCommand>
{
private:
  iCelConsole* parent;

public:
  cmdPerfTest (iCelConsole* parent) : scfImplementationType (this),
				    parent (parent) { }
  virtual ~cmdPerfTest () { }
  virtual const char* GetCommand () { return "perftest"; }
  virtual const char* GetDescription () { return "Runs a performance test."; }
  virtual void Help ()
  {
    parent->GetOutputConsole ()->PutText ("Usage: perftest\n");
    parent->GetOutputConsole ()->PutText (
     "  Runs a performance test.\n");
  }
  virtual void Execute (const csStringArray& args)
  {
    iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
    Client* client = PointerLibrary::getInstance()->getClient();

    csRef<iVirtualClock> vc = CS_QUERY_REGISTRY(obj_reg, iVirtualClock);
    if(!vc) return;

    csTicks t1, t2;
    parent->GetOutputConsole ()->PutText ("Performance test busy...\n");
    t1 = client->GetTicks ();
    int num = 50;
    int i;
    for (i = 0 ; i < num ; i++)
    {
      client->DrawFrame ();
    }
    t2 = client->GetTicks ();
   char buffer[1024];
   sprintf(buffer, "%f secs to render %d frames: %f fps\n", 
      (float) (t2 - t1) / 1000., num, float (num) * 1000. / (float) (t2 - t1));
    parent->GetOutputConsole ()->PutText (buffer);
    csPrintf (buffer);
  }
};
//--------------------------------------------------------------------------

bool PtConsole::Initialize ()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

  console = CS_QUERY_REGISTRY(obj_reg, iCelConsole);
  if(!console) return false;

  console->GetInputConsole()->SetPrompt ("PT>");

  // Register the commands.
  csRef<iCelConsoleCommand> cmd;
  cmd.AttachNew (new cmdPerfTest (console)); console->RegisterCommand (cmd);

  return true;
}
