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

#include "client/network/network.h"
#include "client/gui/gui.h"
#include "client/gui/guimanager.h"
#include "client/data/effect/effectsmanager.h"
#include "client/combat/combatmanager.h"
#include "common/data/itemdatamanager.h"
#include "client/entity/player/playerentity.h"

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
    PT::Client* client = PointerLibrary::getInstance()->getClient();

    csRef<iVirtualClock> vc =  csQueryRegistry<iVirtualClock> (obj_reg);
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

class cmdGuiMgr : public scfImplementation1<cmdGuiMgr, iCelConsoleCommand>
{
private:
  iCelConsole* parent;

public:
  cmdGuiMgr (iCelConsole* parent) : scfImplementationType (this),
    parent (parent) { }
  virtual ~cmdGuiMgr () { }
  virtual const char* GetCommand () { return "guimanager"; }
  virtual const char* GetDescription () { return "Control the guimanager."; }
  virtual void Help ()
  {
    parent->GetOutputConsole ()->PutText ("Usage: \n");
    parent->GetOutputConsole ()->PutText (
      "  guimanager <window> <command> OR guimanager <command> <arg>\n");
    parent->GetOutputConsole ()->PutText (
      " windows: buddy buy chat confirm servers hud inventory login npcdialog ok options selectchar sell status trade\n");
    parent->GetOutputConsole ()->PutText (
      " commands: show hide / create(file arg)...\n");
  }
  virtual void Execute (const csStringArray& args)
  {
    GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
    if(!guimanager) return;

    GUIWindow* window = 0;
    if (args.GetSize() < 2)
    {
      Help();
      return;
    }
    else if(strcmp(args[1],"buddy")==0)
      window = guimanager->GetBuddyWindow();
    else if (strcmp(args[1],"buy")==0)
      window = guimanager->GetBuyWindow();
    else if (strcmp(args[1],"chat")==0)
      window = guimanager->GetChatWindow();
    else if (strcmp(args[1],"confirm")==0)
      window = guimanager->GetConfirmWindow();
    else if (strcmp(args[1],"servers")==0)
      window = guimanager->GetServerWindow();
    else if (strcmp(args[1],"hud")==0)
      window = guimanager->GetHUDWindow();
    else if (strcmp(args[1],"inventory")==0)
      window = guimanager->GetInventoryWindow();
    else if (strcmp(args[1],"login")==0)
      window = guimanager->GetLoginWindow();
    else if (strcmp(args[1],"npcdialog")==0)
      window = guimanager->GetNpcDialogWindow();
    else if (strcmp(args[1],"ok")==0)
      window = guimanager->GetOkWindow();
    else if (strcmp(args[1],"options")==0)
      window = guimanager->GetOptionsWindow();
    else if (strcmp(args[1],"selectchar")==0)
      window = guimanager->GetSelectCharWindow();
    else if(strcmp(args[1],"sell")==0)
      window = guimanager->GetSellWindow();
    else if (strcmp(args[1],"status")==0)
      window = guimanager->GetStatusWindow();
    else if(strcmp(args[1],"trade")==0)
      window = guimanager->GetTradeWindow();

    else if(strcmp(args[1],"create")==0)
    {
      if(args.GetSize() > 2)
      {
        window = new GUIWindow(guimanager);
        window->CreateGUIWindow(args[2]);
      }
      else
        Help();
      return;
    }
    else
    {
      parent->GetOutputConsole ()->PutText ("Unknown window or command!\n");
      return;
    }

    if(window)
    {
      if (strcmp(args[2],"show")==0)
        window->ShowWindow();
      else if(strcmp(args[2],"hide")==0)
        window->HideWindow();
      else
      {
        parent->GetOutputConsole ()->PutText ("Unknown command!\n");
        return;
      }
    }
    else
      parent->GetOutputConsole ()->PutText ("Window hasn't been created yet!\n");

  }
};
//--------------------------------------------------------------------------

class cmdEquip : public scfImplementation1<cmdEquip, iCelConsoleCommand>
{
private:
  iCelConsole* parent;

public:
  cmdEquip (iCelConsole* parent) : scfImplementationType (this),
    parent (parent) { }
  virtual ~cmdEquip () { }
  virtual const char* GetCommand () { return "equipment"; }
  virtual const char* GetDescription () { return "Equip items on your own entity, for testing only."; }
  virtual void Help ()
  {
    parent->GetOutputConsole ()->PutText ("Usage: \n");
    parent->GetOutputConsole ()->PutText (
      " equipment <(un)equip> <slotid> <itemid>\n");
  }
  virtual void Execute (const csStringArray& args)
  {
    PT::Entity::PlayerEntity* ent = PT::Entity::PlayerEntity::Instance();
    if(!ent)
    {
      parent->GetOutputConsole ()->PutText ("Your entity hasn't been created yet!\n");
      return;
    }

    if (args.GetSize() < 4)
    {
      Help();
      return;
    }
    else if(strcmp(args[1],"equip")==0)
      ent->GetEquipment().Equip(atoi(args[3]), atoi(args[2]));
    else if(strcmp(args[1],"unequip")==0)
      ent->GetEquipment().UnEquip(atoi(args[3]));
    else
    {
      parent->GetOutputConsole ()->PutText ("Unknown command!\n");
      return;
    }
  }
};
//--------------------------------------------------------------------------

class cmdItem : public scfImplementation1<cmdItem, iCelConsoleCommand>
{
private:
  iCelConsole* parent;

public:
  cmdItem (iCelConsole* parent) : scfImplementationType (this),
    parent (parent) { }
  virtual ~cmdItem () { }
  virtual const char* GetCommand () { return "itemmanager"; }
  virtual const char* GetDescription () { return "Equip items on your own entity, for testing only."; }
  virtual void Help ()
  {
    parent->GetOutputConsole ()->PutText ("Usage: \n");
    parent->GetOutputConsole ()->PutText (
      " itemmanager <reload>\n");
  }
  virtual void Execute (const csStringArray& args)
  {
    PT::Data::ItemDataManager* itemDataMgr = PointerLibrary::getInstance()->getItemDataManager();
    if(!itemDataMgr) return;

    if (args.GetSize() < 2)
    {
      Help();
      return;
    }
    else if(strcmp(args[1],"reload")==0)
      itemDataMgr->LoadItemData();
    else
    {
      parent->GetOutputConsole ()->PutText ("Unknown command!\n");
      return;
    }
  }
};
//--------------------------------------------------------------------------

bool PtConsole::Initialize ()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

  console =  csQueryRegistry<iCelConsole> (obj_reg);
  if(!console) return false;

  console->GetInputConsole()->SetPrompt ("PT>");

  // Register the commands.
  csRef<iCelConsoleCommand> cmd;
  cmd.AttachNew (new cmdPerfTest (console)); console->RegisterCommand (cmd);
  cmd.AttachNew (new cmdGuiMgr (console)); console->RegisterCommand (cmd);
  cmd.AttachNew (new cmdEquip (console)); console->RegisterCommand (cmd);
  cmd.AttachNew (new cmdItem (console)); console->RegisterCommand (cmd);

  return true;
}
