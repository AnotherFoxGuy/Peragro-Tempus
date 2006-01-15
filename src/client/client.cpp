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

#include "client.h"

#include "cursor.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "cstool/initapp.h"
#include "csutil/cmdline.h"
#include "csutil/csstring.h"
#include "csutil/csshlib.h"
#include "csutil/event.h"
#include "csutil/sysfunc.h"
#include "csutil/syspath.h"
#include "iengine/camera.h"
#include "iengine/campos.h"
#include "iengine/mesh.h"
#include "iengine/sector.h"
#include "imesh/object.h"
#include "imesh/spritecal3d.h"
#include "isound/renderer.h"
#include "isound/loader.h"
#include "iutil/databuff.h"
#include "iutil/event.h"
#include "iutil/eventq.h"
#include "iutil/object.h"
#include "iutil/vfs.h"
#include "ivaria/collider.h"
#include "ivideo/graph2d.h"
#include "ivideo/natwin.h"

#include "physicallayer/pl.h"
#include "physicallayer/propfact.h"
#include "physicallayer/propclas.h"
#include "physicallayer/entity.h"
#include "physicallayer/persist.h"
#include "celtool/initapp.h"
#include "celtool/persisthelper.h"
#include "celtool/stdparams.h"
#include "behaviourlayer/bl.h"
#include "propclass/test.h"
#include "propclass/mesh.h"
#include "propclass/meshsel.h"
#include "propclass/solid.h"
#include "propclass/inv.h"
#include "propclass/chars.h"
#include "propclass/move.h"
#include "propclass/prop.h"
#include "propclass/tooltip.h"
#include "propclass/defcam.h"
#include "propclass/gravity.h"
#include "propclass/timer.h"
#include "propclass/region.h"
#include "propclass/input.h"
#include "propclass/navgraph.h"
#include "propclass/linmove.h"
#include "propclass/actormove.h"
#include "propclass/quest.h"
#include "propclass/trigger.h"
#include "propclass/zone.h"
#include "propclass/sound.h"

#include "client/network/network.h"
#include "common/entity/entity.h"

#include "common/util/wincrashdump.h"

CS_IMPLEMENT_APPLICATION

Client::Client() : playing(false)
{
  SetApplicationName ("Client");
  walk = 0;
  turn = 0;
  timer = 0;
  limitFPS = 0;
  last_sleep = 0;
  own_char_id = -1;

  cursor = new Cursor(this);
}

Client::~Client()
{
  delete network;
  delete cursor;
}

void Client::ProcessFrame()
{
  loadRegion();
  addEntity();
  delEntity();
  moveEntity();
  DrUpdateEntity();
  chat();

  csTicks ticks = vc->GetElapsedTicks();
  timer += ticks;
  if (limitFPS > 0)
  {
    if (ticks < 1000.0f/limitFPS)
    csSleep(1000.0f/limitFPS - ticks);
  }
  if (timer > 1000)
  {
    timer = 0;
    if (name.GetData() != 0)
    {
      // This has later to be sent by the worldserver, or at least be verified by the server!
      iCelEntity* entity = pl->FindEntity(name.GetData());
      if (entity)
      {
        csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

        bool on_ground;
        float speed, rot, avel;
        csVector3 pos, vel, wvel;
        iSector* sector;

        pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
        //printf("SetDR1: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);

        UpdateDREntityRequestMessage drmsg;
        drmsg.setOnGround(on_ground?1:0);
        drmsg.setSpeed(speed);
        drmsg.setRot(rot);
        drmsg.setAVel(avel);
        drmsg.setPos(pos.x,pos.y,pos.z);
        drmsg.setVel(vel.x,vel.y,vel.z);
        drmsg.setWVel(wvel.x,wvel.y,wvel.z);
        drmsg.setSector(sector->QueryObject()->GetName());
/*
        ByteStream bs;
        drmsg.serialise(&bs);
        UpdateDREntityMessage drmsg2;
        drmsg2.deserialise(&bs);
        printf("SetDR1.2: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", drmsg2.getSpeed(), drmsg2.getPos()[0], drmsg2.getPos()[1], drmsg2.getPos()[2], drmsg2.getRot());
*/
        network->send(&drmsg);
      }
    }
  }
}

void Client::FinishFrame()
{
  g3d->BeginDraw(CSDRAW_3DGRAPHICS);

  // Render our GUI on top
  cegui->Render ();
  
  cursor->Draw();

  g3d->FinishDraw();
  g3d->Print(0);

  g3d->BeginDraw(engine->GetBeginDrawFlags());
}

bool Client::OnInitialize(int argc, char* argv[])
{
  if (!csInitializer::SetupConfigManager(GetObjectRegistry(),
      "/client/config/client.cfg", GetApplicationName()))
    return ReportError("Failed to initialize configuration manager!");

  if (!celInitializer::RequestPlugins(GetObjectRegistry(),
    CS_REQUEST_REPORTER,
    CS_REQUEST_REPORTERLISTENER,
    CS_REQUEST_END))
    return ReportError("Failed to initialize plugins!");

  csBaseEventHandler::Initialize(GetObjectRegistry());

  if (!RegisterQueue(GetObjectRegistry(), csevAllEvents(GetObjectRegistry())))
    return ReportError("Failed to set up event handler!");

  return true;
}

void Client::OnExit()
{
}

bool Client::Application()
{
  if (!OpenApplication(GetObjectRegistry()))
    return ReportError("Error opening system!");

  vfs = CS_QUERY_REGISTRY(GetObjectRegistry(), iVFS);
  if (!vfs) return ReportError("Failed to locate VFS!");

  g3d = CS_QUERY_REGISTRY(GetObjectRegistry(), iGraphics3D);
  if (!g3d) return ReportError("Failed to locate 3D renderer!");

  engine = CS_QUERY_REGISTRY(GetObjectRegistry(), iEngine);
  if (!engine) return ReportError("Failed to locate 3D engine!");

  vc = CS_QUERY_REGISTRY(GetObjectRegistry(), iVirtualClock);
  if (!vc) return ReportError("Failed to locate Virtual Clock!");

  //kbd = CS_QUERY_REGISTRY(GetObjectRegistry(), iKeyboardDriver);
  //if (!kbd) return ReportError("Failed to locate Keyboard Driver!");

  cmdline = CS_QUERY_REGISTRY(GetObjectRegistry(), iCommandLineParser);
  if (!cmdline) return ReportError("Failed to locate CommandLineParser plugin");

  cegui = CS_QUERY_REGISTRY(GetObjectRegistry(), iCEGUI);
  if (!cegui) return ReportError("Failed to locate CEGUI plugin");

  // Initialize CEGUI wrapper
  cegui->Initialize ();

  // Set the logging level
  cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

  vfs->ChDir ("/client/skin/");

  // Load the ice skin (which uses Falagard skinning system)
  cegui->GetSchemeManagerPtr ()->loadScheme("TaharezLook.scheme");

  cegui->GetSystemPtr ()->setDefaultMouseCursor("TaharezLook", "MouseArrow");
  cegui->GetFontManagerPtr ()->createFont("CommonWealth", "/client/skin/Commonv2c.ttf", 10, 
    CEGUI::Default);
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();

  // Load layout and set as root
  vfs->ChDir ("/client/gui/");
  cegui->GetSystemPtr ()->setGUISheet(winMgr->loadWindowLayout("login.xml"));

  CEGUI::Window* btn;

  if (cmdline)
  {
    const char* fpslim = cmdline->GetOption("fpsLimit");
    if (fpslim) limitFPS = atoi(fpslim);

    bool gui = cmdline->GetBoolOption("gui", true);

    if (gui)
    {
      btn = winMgr->getWindow("Connect_Button");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUIConnect, this));

      btn = winMgr->getWindow("Register");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUIRegister, this));

      btn = winMgr->getWindow("Login");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUILogin, this));

      btn = winMgr->getWindow("CharSelOk");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUICharSel, this));
      btn = winMgr->getWindow("CharSelNew");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUICharNew, this));

      btn = winMgr->getWindow("Say");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUISay, this));
      btn = winMgr->getWindow("Shout");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUIShout, this));
      btn = winMgr->getWindow("Whisper");
      btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&Client::ceGUIWhisper, this));
    }
  }

  btn = winMgr->getWindow("Characters");
  CEGUI::String str_id("Id");
  CEGUI::String str_name("Name");
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_id,0,0.1f);
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_name,1,0.5f);
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

  engine->SetClearScreen(true);

  InitializeCEL();

  network = new Network(this);
  network->init();

  if (cmdline)
  {
    const char* host = cmdline->GetOption("host");
    if (host)
    {
      ConnectRequestMessage msg;
      SocketAddress addr = Socket::getSocketAddress(host, 12345);
      network->setServerAddress(addr);
      network->send(&msg);
    }
  }

  Run();

  return true;
}

bool Client::ceGUIConnect (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();

  CEGUI::Window* btn;
  btn = winMgr->getWindow("Connect");
  btn->setEnabled(false);

  btn = winMgr->getWindow("Server");
  CEGUI::String str = btn->getText();
  printf("Connecting to %s!!\n", str.c_str());

  ConnectRequestMessage msg;
  SocketAddress addr = Socket::getSocketAddress(str.c_str(), 12345);
  network->setServerAddress(addr);
  network->send(&msg);

  return true; 
}

void Client::connected ()
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();

  CEGUI::Window* btn;
  btn = winMgr->getWindow("Connect");
  btn->setEnabled(false);
  btn->setVisible(false);
  btn = winMgr->getWindow("LoginUI");
  btn->setVisible(true);

  if (cmdline)
  {
    char* user = (char* )cmdline->GetOption("user", 0);
    char* pass = (char* )cmdline->GetOption("pass", 0);

    if (user && pass)
    {
      if (cmdline->GetBoolOption("register", false))
      {
        RegisterRequestMessage reg_msg;
        reg_msg.setName(user);
        reg_msg.setPwHash(pass);
        network->send(&reg_msg);
      }

      LoginRequestMessage answer_msg;
      answer_msg.setName(user);
      answer_msg.setPwHash(pass);
      this->name = user;
      network->send(&answer_msg);
    }
  }

  printf("Connected!!\n");
}

bool Client::ceGUIRegister (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Name");
  CEGUI::String name = btn->getText();
  btn = winMgr->getWindow("Password");
  CEGUI::String pw = btn->getText();

  printf("Register %s:%s!!\n", name.c_str(), pw.c_str());

  RegisterRequestMessage answer_msg;
  answer_msg.setName((char*)name.c_str());
  answer_msg.setPwHash((char*)pw.c_str());
  network->send(&answer_msg);
  return true;
}

bool Client::ceGUILogin (const CEGUI::EventArgs& e) 
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Name");
  CEGUI::String name = btn->getText();
  btn = winMgr->getWindow("Password");
  CEGUI::String pw = btn->getText();

  this->name = name.c_str();

  printf("Login %s:%s!!\n", name.c_str(), pw.c_str());

  LoginRequestMessage answer_msg;
  answer_msg.setName((char*)name.c_str());
  answer_msg.setPwHash((char*)pw.c_str());
  network->send(&answer_msg);
  return true;
}

bool Client::ceGUICharNew (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("CharNewName");
  CEGUI::String name = btn->getText();

  CharacterCreationRequestMessage answer_msg;
  answer_msg.setName((char*)name.c_str());
  network->send(&answer_msg);
  return true;
}

bool Client::ceGUICharSel (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Characters");
  if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
    return true;

  CEGUI::ListboxItem* item = ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();
  
  own_char_id = atoi(item->getText().c_str());

  CharacterSelectionRequestMessage answer_msg;
  answer_msg.setCharId(own_char_id);
  network->send(&answer_msg);
  return true;
}

bool Client::ceGUISay (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  printf("Say: %s\n", text.c_str());
  ChatMessage msg;
  msg.setType(0);
  msg.setMessage(text.c_str());
  network->send(&msg);
  return true;
}

bool Client::ceGUIShout (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  printf("Shout: %s\n", text.c_str());
  ChatMessage msg;
  msg.setType(1);
  msg.setMessage(text.c_str());
  network->send(&msg);
  return true;
}

bool Client::ceGUIWhisper (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  printf("!!TDB!! Whisper: %s\n", text.c_str());
  return true;
}

bool Client::OnKeyboard(iEvent& ev)
{
  csKeyEventType eventtype = csKeyEventHelper::GetEventType(&ev);
  if (eventtype == csKeyEventTypeDown)
  {
    utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
    if (code == CSKEY_ESC)
    {
      csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (GetObjectRegistry());
      if (q.IsValid()) q->GetEventOutlet()->Broadcast(csevQuit(GetObjectRegistry()));
    }

    if (playing)
    {
      if (csKeyEventHelper::GetAutoRepeat(&ev)) return false;

      utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
      //if (code == CSKEY_TAB)
      //{
        //console_focus=true;
      //}
      if (code == CSKEY_UP)
      {
        walk = 1;
      }
      else if (code == CSKEY_DOWN)
      {
        walk = -1;
      }
      else if (code == CSKEY_LEFT)
      {
        turn = -1;
      }
      else if (code == CSKEY_RIGHT)
      {
        turn = 1;
      }
      /*
      else if (code == CSKEY_SPACE)
      {
        //netclient->Jump();
      }
      else if (code == 's')
      {
        //netclient->Sit();
      }
      */
      else if (code == 'p')
      {
        csRef<iCelEntity> ent = cursor->getSelectedEntity();
        if (!ent)
          return false;

        csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(ent, iPcProperties);
        if (!pcprop)
          return false;

        if (pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity Type")) == Entity::ItemEntity)
        {
          PickEntityRequestMessage msg;
          msg.setTargetId(pcprop->GetPropertyLong(pcprop->GetPropertyIndex("Entity ID")));
          network->send(&msg);
        }
        return false;
      }
      else if (code == 'd')
      {
        DropEntityRequestMessage msg;
        msg.setTargetId(1); //"Apple"
        network->send(&msg);
        return false;
      }
      else
      {
        return false;
      }
      MoveEntityRequestMessage msg;
      msg.setWalk(walk);
      msg.setRot(turn);
      network->send(&msg);
    }
  }
  else if (eventtype == csKeyEventTypeUp)
  {
    if (playing)
    {
      utf32_char code = csKeyEventHelper::GetCookedCode(&ev);
      if (code == CSKEY_UP)
      {
        walk = 0;
      }
      else if (code == CSKEY_DOWN)
      {
        walk = 0;
      }
      else if (code == CSKEY_LEFT)
      {
        turn = 0;
      }
      else if (code == CSKEY_RIGHT)
      {
        turn = 0;
      }
      else
      {
        return false;
      }
      MoveEntityRequestMessage msg;
      msg.setWalk(walk);
      msg.setRot(turn);
      network->send(&msg);
    }
  }
  return false;
}

bool Client::OnMouseMove(iEvent& e)
{
  iCamera* cam = getCamera();
  if (!cam) return false;
  cursor->MouseMove(pl, cam, csMouseEventHelper::GetX(&e), csMouseEventHelper::GetY(&e));
  return false;
}

bool Client::InitializeCEL()
{
  iObjectRegistry* object_reg = this->GetObjectRegistry();

  pl = CS_QUERY_REGISTRY (object_reg, iCelPlLayer);
  if (!pl) return ReportError("Failed to load CEL Physical Layer");

  // Loading property class factories
  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.region"))
    return ReportError("Failed to load CEL Region Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.mesh"))
    return ReportError("Failed to load CEL Mesh Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.solid"))
    return ReportError("Failed to load CEL Solid Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.defaultcamera"))
    return ReportError("Failed to load CEL Camera Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.linmove"))
    return ReportError("Failed to load CEL Linmove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.colldet"))
    return ReportError("Failed to load CEL Colldet Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.actormove"))
    return ReportError("Failed to load CEL Actormove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.npcmove"))
    return ReportError("Failed to load CEL Npcmove Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.trigger"))
    return ReportError("Failed to load CEL Trigger Factory");

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.pccommandinput"))
    return false;

  if (!pl->LoadPropertyClassFactory ("cel.pcfactory.properties"))
    return false;

  return true;
}

void Client::loadRegion(const char* name)
{
  load_region.AttachNew(new scfString(name));
}

void Client::loggedIn()
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("LoginUI");
  btn->setVisible(false);
  btn = winMgr->getWindow("CharSelectUI");
  btn->setVisible(true);

  if (cmdline)
  {
    const char* character = cmdline->GetOption("char");
    if (character)
    {
      CharacterSelectionRequestMessage answer_msg;
      answer_msg.setCharId(0);
      network->send(&answer_msg);
    }
  }
}

void Client::addCharacter(unsigned int charId, const char* name)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Characters");
  char charIdstr[10];
  sprintf(charIdstr, "%d", charId);
  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem(charIdstr);
  CEGUI::ListboxItem* charNameItem = new CEGUI::ListboxTextItem(name);

  charIdItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");
  charNameItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");

  unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
  ((CEGUI::MultiColumnList*)btn)->setItem(charIdItem, 0, row);
  ((CEGUI::MultiColumnList*)btn)->setItem(charNameItem, 1, row);
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
}

void Client::loadRegion()
{
  playing = true;

  if (!load_region.IsValid()) return;
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("CharSelectUI");
  btn->setVisible(false);
  btn = winMgr->getWindow("Chat");
  btn->setVisible(true);

  csRef<iCelEntity> entity = pl->CreateEntity();
  pl->CreatePropertyClass(entity, "pcregion");
  csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(entity, iPcRegion);
  entity->SetName("World");
  pcregion->SetRegionName("World");
  pcregion->SetWorldFile (load_region->GetData(), "world");
  pcregion->Load();
  load_region = 0;
}

void Client::addEntity(Entity* entity)
{
  mutex.lock();
  new_entity_name.Push(entity);
  mutex.unlock();
}

void Client::addEntity()
{
  if (!new_entity_name.GetSize()) return;
  mutex.lock();

  Entity* ent = new_entity_name.Pop();
  csRef<iCelEntity> entity = pl->CreateEntity();

  if (ent->getType() == Entity::ItemEntity)
  {
    char buffer[1024];
    sprintf(buffer, "%s:%d:%d", ent->getName(),ent->getType(), ent->getId());
    entity->SetName(buffer);
  }
  else
    entity->SetName(ent->getName());

  pl->CreatePropertyClass(entity, "pcmesh");
  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
  pl->CreatePropertyClass(entity, "pclinearmovement");
  csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
  pl->CreatePropertyClass(entity, "pcsolid");
  csRef<iPcSolid> pctemp = CEL_QUERY_PROPCLASS_ENT(entity, iPcSolid);

  pl->CreatePropertyClass(entity, "pcactormove");
  csRef<iPcActorMove> pcactormove = CEL_QUERY_PROPCLASS_ENT (entity, iPcActorMove);
  pcactormove->SetMovementSpeed (2.0f);
  pcactormove->SetRunningSpeed (5.0f);
  pcactormove->SetRotationSpeed (1.75f);
  pcactormove->SetJumpingVelocity (6.31f);

  iSector* sector = engine->FindSector(ent->getSector());

  printf("Loading Actor\n");
  vfs->ChDir("/cellib/objects/");
  pcmesh->SetMesh(ent->getMesh(), "/client/meshes/all.xml");

  csRef<iCelEntity> region = pl->FindEntity("World");
  if (region)
  {
    csRef<iPcRegion> pcregion = CEL_QUERY_PROPCLASS_ENT(region, iPcRegion);
    pcmesh->MoveMesh(pcregion->GetStartSector(), pcregion->GetStartPosition());
  }

  if (own_char_id == ent->getId())
  {
    printf("Adding Entity '%s' as me\n", entity->GetName());
    pl->CreatePropertyClass(entity, "pcdefaultcamera");
    csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
    pccamera->SetMode(iPcDefaultCamera::thirdperson, true);
    //pccamera->GetCamera()->SetSector(sector);
  }
  else
  {
    printf("Adding Entity '%s'\n", entity->GetName());
  }

  pclinmove->InitCD(
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0.5f,0.8f,0.5f),
    csVector3(0,0,0));

  csVector3 pos(ent->getPos()[0], ent->getPos()[1], ent->getPos()[2]);
  pclinmove->SetPosition(pos,0,sector);

  pl->CreatePropertyClass(entity, "pcproperties");
  csRef<iPcProperties> pcprop = CEL_QUERY_PROPCLASS_ENT(entity, iPcProperties);
  pcprop->SetProperty("Entity Type", (long)ent->getType());
  pcprop->SetProperty("Entity ID", (long)ent->getId());
  pcprop->SetProperty("Entity Name", (long)ent->getName());

  delete ent;

  mutex.unlock();
}

void Client::delEntity(Entity* entity)
{
  mutex.lock();
  del_entity_name.Push(entity);
  mutex.unlock();
}

void Client::delEntity()
{
  if (!del_entity_name.GetSize()) return;
  mutex.lock();
  Entity* ent = del_entity_name.Pop();

  csRef<iCelEntity> entity;
  if (ent->getType() == Entity::ItemEntity)
  {
    char buffer[1024];
    sprintf(buffer, "%s:%d:%d", ent->getName(),ent->getType(), ent->getId());
    entity = pl->FindEntity(buffer);
  }
  else
  {
    entity = pl->FindEntity(ent->getName());
  }

  if (entity)
  {
    pl->RemoveEntity(entity);
  }

  delete ent;

  mutex.unlock();
}

void Client::DrUpdateEntity(DrUpdate* drupdate)
{
  mutex.lock();
  if (name.Compare(drupdate->name))
  {
    delete drupdate;
    return;
  }
  drupdate_entity_name.Push(drupdate);
  mutex.unlock();
}

void Client::DrUpdateEntity()
{
  if (!drupdate_entity_name.GetSize()) return;
  mutex.lock();
  DrUpdate* drupdate = drupdate_entity_name.Pop();
  iCelEntity* entity = pl->FindEntity(drupdate->name.GetData());
  if (entity)
  {
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      bool on_ground;
      float speed, rot, avel;
      csVector3 pos, vel, wvel;
      
      iSector* sector = engine->FindSector(drupdate->sector.GetData());

      //pclinmove->GetDRData(on_ground, speed, pos, rot, sector, vel, wvel, avel);
      //printf("SetDR2: %.2f, <%.2f,%.2f,%.2f>, %.2f\n", speed, pos.x, pos.y, pos.z, rot);
      pclinmove->SetDRData(drupdate->on_ground, drupdate->speed, drupdate->pos, drupdate->rot, sector, vel, drupdate->wvel, drupdate->avel);
    }
  }
  delete drupdate;
  mutex.unlock();
}

void Client::moveEntity(const char* name, float walk, float turn)
{
  mutex.lock();
  printf("Add movement for '%s': w(%.2f) r(%.2f)\n", name, walk, turn);
  Movement* movement = new Movement();
  movement->name = name;
  movement->walk = walk;
  movement->turn = turn;
  move_entity_name.Push(movement);
  mutex.unlock();
}

void Client::moveEntity()
{
  if (!move_entity_name.GetSize()) return;
  mutex.lock();
  Movement* movement = move_entity_name.Pop();
  iCelEntity* entity = pl->FindEntity(movement->name.GetData());
  if (entity)
  {
    csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
    if (pclinmove.IsValid())
    {
      pclinmove->SetAngularVelocity(csVector3(0,-movement->turn,0));
      pclinmove->SetVelocity(csVector3(0,0,-movement->walk));
    }
    csRef<iPcMesh> mesh = CEL_QUERY_PROPCLASS_ENT(entity, iPcMesh);
    csRef<iSpriteCal3DState> sprcal3d = 
      SCF_QUERY_INTERFACE (mesh->GetMesh()->GetMeshObject(), iSpriteCal3DState);
    if (sprcal3d)
      sprcal3d->SetVelocity(movement->walk);
  }
  delete movement;
  mutex.unlock();
}

void Client::chat(char type, const char* msg)
{
  mutex.lock();
  chat_msg.Push(new scfString(msg));
  mutex.unlock();
}

void Client::chat()
{
  if (!chat_msg.GetSize()) return;
  mutex.lock();
  csRef<iString> msg = chat_msg.Pop();
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Listbox* dialog = 
    static_cast<CEGUI::Listbox*> (winMgr->getWindow("MessageDialog"));

  //add text to list
  CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(msg->GetData());
  dialog->addItem ( item );
  dialog->ensureItemIsVisible(dialog->getItemCount());

  mutex.unlock();
}

iCamera* Client::getCamera()
{
  if (name.GetData() == 0) 
    return 0;
  iCelEntity* entity = pl->FindEntity(name.GetData());
  if (entity == 0) 
    return 0;
  csRef<iPcDefaultCamera> pccamera = CEL_QUERY_PROPCLASS_ENT(entity, iPcDefaultCamera);
  return pccamera->GetCamera();
}

/*---------------*
 * Main function
 *---------------*/
int main (int argc, char* argv[])
{
  setWinCrashDump(argv[0]);
  return csApplicationRunner<Client>::Run (argc, argv);
}
