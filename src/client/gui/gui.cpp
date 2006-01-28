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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

/*==================//
//     GUIBase      //
//==================*/

GUIWindow::GUIWindow(GUIManager* guimanager)
: guimanager(guimanager)
{
  cegui = guimanager->GetCEGUI ();
  vfs = guimanager->GetClient ()->getVFS ();
  network = guimanager->GetClient ()->getNetwork ();
}
GUIWindow::~GUIWindow()
{
}

void GUIWindow::CreateGUIWindow(const char* layoutFile)
{
  //cegui->GetSystemPtr ()->setGUISheet(LoadLayout (layoutFile));
  CEGUI::Window* root = cegui->GetWindowManagerPtr ()->getWindow("Root");
  root->addChildWindow(LoadLayout (layoutFile));
}

CEGUI::Window* GUIWindow::LoadLayout(const char* layoutFile)
{
  winMgr = cegui->GetWindowManagerPtr ();

  // Load layout and set as root
  vfs->ChDir ("/client/gui/");
  return winMgr->loadWindowLayout(layoutFile);
}

void GUIWindow::HideWindow()  
{
  rootwindow->setVisible(false);
}

void GUIWindow::ShowWindow() 
{
  rootwindow->setVisible(true);
}

void GUIWindow::DisableWindow() 
{
  rootwindow->setEnabled(false);
}

void GUIWindow::EnableWindow() 
{
  rootwindow->setEnabled(true);
}

/*====================//
// GUIConnectWindow   //
//====================*/
ConnectWindow::ConnectWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ConnectWindow::~ConnectWindow()
{
}

bool ConnectWindow::ConnectButtonPressed(const CEGUI::EventArgs& e) 
{
  // Get the connect window and disable it
  GUIWindow::DisableWindow();

  ConnectRequestMessage msg;
  SocketAddress addr = Socket::getSocketAddress(GetServer(), 12345);
  network->setServerAddress(addr);
  network->send(&msg);

  return true;
}

csString ConnectWindow::GetServer()  
{
  btn = winMgr->getWindow("Server");
  CEGUI::String str = btn->getText();
  csString serverip = str.c_str();
  return serverip;
}

void ConnectWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("connect.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("Connect");

  // Register the button events.
  btn = winMgr->getWindow("Connect_Button");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConnectWindow::ConnectButtonPressed, this));
}
/*====================//
//   GUILoginWindow   //
//====================*/

LoginWindow::LoginWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

LoginWindow::~LoginWindow()
{
}

bool LoginWindow::LoginButtonPressed(const CEGUI::EventArgs& e) 
{
  // Get the login window and disable it
  GUIWindow::DisableWindow();

  LoginRequestMessage answer_msg;
  CEGUI::String login = GetLogin();
  CEGUI::String password = GetPassword();
  if (login.empty() || password.empty()) return true;
  answer_msg.setName(login.c_str());
  answer_msg.setPwHash(password.c_str());
  network->send(&answer_msg);

  return true;
}

bool LoginWindow::RegisterButtonPressed(const CEGUI::EventArgs& e)
{
  RegisterRequestMessage answer_msg;
  CEGUI::String login = GetLogin();
  CEGUI::String password = GetPassword();
  if (login.empty() || password.empty()) return true;
  answer_msg.setName(login.c_str());
  answer_msg.setPwHash(password.c_str());
  network->send(&answer_msg);

  return true; 
}

CEGUI::String LoginWindow::GetLogin() 
{
  return winMgr->getWindow("Name")->getText();
}

CEGUI::String LoginWindow::GetPassword() 
{
  return winMgr->getWindow("Password")->getText();
}

bool LoginWindow::RememberLogin()  
{
  return true;
}

void LoginWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("login.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("LoginUI");

  // Register the button events.
  btn = winMgr->getWindow("Login_Button");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoginWindow::LoginButtonPressed, this));

  btn = winMgr->getWindow("Register_Button");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoginWindow::RegisterButtonPressed, this));

}

/*====================//
// SelectCharWindow   //
//====================*/
SelectCharWindow::SelectCharWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

SelectCharWindow::~SelectCharWindow()
{
}

bool SelectCharWindow::SelectChar(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("Characters");
  if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
    return true;

  CEGUI::ListboxItem* item = ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

  if (!item->isSelected()) return true;

  int own_char_id = atoi(item->getText().c_str());

  CharacterSelectionRequestMessage answer_msg;
  answer_msg.setCharId(own_char_id);
  network->send(&answer_msg);

  GUIWindow::DisableWindow();

  return true;
}


bool SelectCharWindow::NewChar(const CEGUI::EventArgs& e) 
{
  CEGUI::String NewCharName = GetNewCharName();
  if (NewCharName.empty()) return true;
  CharacterCreationRequestMessage answer_msg;
  answer_msg.setName(NewCharName.c_str());
  network->send(&answer_msg);
  return true;
}

bool SelectCharWindow::DelChar(const CEGUI::EventArgs& e) 
{
  return true;
}

CEGUI::String SelectCharWindow::GetNewCharName()  
{
  return winMgr->getWindow("CharNewName")->getText();
}


void SelectCharWindow::CreateGUIWindow()
{
  GUIWindow::CreateGUIWindow ("charselect.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("CharSelectUI");

  // Register the button events.
  btn = winMgr->getWindow("CharSelOk");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::SelectChar, this));

  btn = winMgr->getWindow("CharSelNew");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::NewChar, this));

  btn = winMgr->getWindow("Characters");
  CEGUI::String str_id("Id");
  CEGUI::String str_name("Name");
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_id,0,0.1f);
  ((CEGUI::MultiColumnList*)btn)->addColumn(str_name,1,0.5f);
  ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

}

void SelectCharWindow::AddCharacter(unsigned int charId, const char* name)
{
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


/*=================//
//    ChatWindow   //
//=================*/
ChatWindow::ChatWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ChatWindow::~ChatWindow ()
{
}

void ChatWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("chat.xml");

  winMgr = cegui->GetWindowManagerPtr ();

  //CreateDropList();

  // Get the root window
  rootwindow = winMgr->getWindow("Chat");

  btn = winMgr->getWindow("Say");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnSay, this));
  btn = winMgr->getWindow("Shout");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnShout, this));
  btn = winMgr->getWindow("Whisper");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnWhisper, this));

  //btn = winMgr->getWindow("ChatDropList");
  //btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&ConnectWindow::DropList, this));
}


bool ChatWindow::OnSay (const CEGUI::EventArgs& e)
{
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  if (text.empty()) return true;
  printf("Say: %s\n", text.c_str());
  ChatMessage msg;
  msg.setType(0);
  msg.setMessage(text.c_str());
  network->send(&msg);
  btn->setText(text.erase());
  return true;
}

bool ChatWindow::OnShout (const CEGUI::EventArgs& e)
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
  if (text.empty()) return true;
  printf("Shout: %s\n", text.c_str());
  ChatMessage msg;
  msg.setType(1);
  msg.setMessage(text.c_str());
  network->send(&msg);
  btn->setText(text.erase());
  return true;
}

bool ChatWindow::OnWhisper (const CEGUI::EventArgs& e)
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
  if (text.empty()) return true;
  printf("!!TDB!! Whisper: %s\n", text.c_str());
  btn->setText(text.erase());
  return true;
}

bool ChatWindow::OnDropList(const CEGUI::EventArgs& e) 
{
  printf("success \n");
  return true;
}
void ChatWindow::CreateDropList()
{

  btn = winMgr->getWindow("ChatDropList");
  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"FrameWindow", 0);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);
  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"hello", 1);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);
  ((CEGUI::Combobox*)btn)->setReadOnly(true);

}
void ChatWindow::AddChatMessage (csRef<iString> msg)
{
  CEGUI::Listbox* dialog = 
    static_cast<CEGUI::Listbox*> (winMgr->getWindow("MessageDialog"));

  //add text to list
  CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(msg->GetData());
  dialog->addItem ( item );
  dialog->ensureItemIsVisible(dialog->getItemCount());
}


InventoryWindow::InventoryWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

InventoryWindow::~InventoryWindow()
{
}

bool InventoryWindow::handleDragEnter(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FF00FF00 tr:FF00FF00 bl:FF00FF00 br:FF00FF00");

  return true;
}
bool InventoryWindow::handleDragLeave(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
  CEGUI::Window* itemcounter = ddea.window->getChild(30);
  int nrofitems = itemcounter->getParent()->getChildCount()-2;
  char buffer[1024];
  sprintf(buffer, "%d", nrofitems);
  itemcounter->setText((CEGUI::String)buffer);
  if (nrofitems < 2) itemcounter->setVisible(false); else itemcounter->setVisible(true);

  return true;
}
bool InventoryWindow::handleDragDropped(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  ddea.window->setProperty("FrameColours", "tl:FFFFFFFF tr:FFFFFFFF bl:FFFFFFFF br:FFFFFFFF");
  ddea.window->addChildWindow(ddea.dragDropItem);
  ddea.window->setUserString("itemtype" , ddea.dragDropItem->getUserString("itemtype"));
  UpdateItemCounter(ddea.window);
  return true;

}
bool InventoryWindow::handleDragDroppedRoot(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);
  int itemid = -1;

  Window* slot = ddea.dragDropItem->getParent();

  if (ddea.dragDropItem->isUserStringDefined("itemtype")) 
  { 
  itemid = atoi(ddea.dragDropItem->getUserString("itemtype").c_str()); 
  printf("About to drop item of type: %d!\n", itemid);
  DropEntityRequestMessage msg;
  msg.setTargetId(itemid);
  network->send(&msg);
  ddea.dragDropItem->destroy();
  printf("Item icon destroyed!\n");
  UpdateItemCounter(slot);
  }
  else 
  {
    //slot->addChildWindow(ddea.dragDropItem);
    printf("Couldn't determine itemID, putting it back!");
  }
  printf("Dropped item to the world!\n");

  return true;
}
bool InventoryWindow::handleDragDroppedStackable(const CEGUI::EventArgs& args)
{
  using namespace CEGUI;

  const DragDropEventArgs& ddea = static_cast<const DragDropEventArgs&>(args);

  bool stackable = false;
  // Check if the slot is occupied by a item with the same id.
  if (ddea.dragDropItem->isUserStringDefined("stackable")) {if(ddea.dragDropItem->getUserString("stackable") == "true") stackable = true;}

  if (stackable)
  {
    if (ddea.window->isUserStringDefined("itemtype")) 
    {
      const char* slottype = ddea.window->getUserString("itemtype").c_str();
      const char* itemtype = ddea.dragDropItem->getUserString("itemtype").c_str();
      printf("Slottype is %s and itemtype is %s \n", slottype, itemtype);

      if ( !strcmp(slottype, itemtype) ) 
      {
        ddea.window->getParent()->addChildWindow(ddea.dragDropItem);
        UpdateItemCounter(ddea.window->getParent());
      }
    }
  }

  return true;
}
CEGUI::Window* InventoryWindow::createDragDropSlot(CEGUI::Window* parent, const CEGUI::UVector2& position)
{
  // Create the slot
  CEGUI::Window* slot = winMgr->createWindow("TaharezLook/StaticImage");
  parent->addChildWindow(slot);
  slot->setWindowPosition(position);
  slot->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.2f), CEGUI::cegui_reldim(0.2f)));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemEnters, CEGUI::Event::Subscriber(&InventoryWindow::handleDragEnter, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves, CEGUI::Event::Subscriber(&InventoryWindow::handleDragLeave, this));
  slot->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDropped, this));

  // Create the itemcounter
  CEGUI::Window* itemcounter = winMgr->createWindow("TaharezLook/Editbox");
  slot->addChildWindow(itemcounter);
  itemcounter->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.45f), CEGUI::cegui_reldim( 0.45f)));
  itemcounter->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.7f), CEGUI::cegui_reldim(0.7f)));
  itemcounter->setVisible(false);
  itemcounter->disable();
  itemcounter->setText("0");
  itemcounter->setAlwaysOnTop(true);
  itemcounter->setAlpha(0.5);
  itemcounter->setID(30);

  return slot;
}
CEGUI::Window* InventoryWindow::createItemIcon(CEGUI::String itemname, int itemtype, bool stackable)
{
  char uniquename[1024];
  counter += 1;
  sprintf(uniquename, "%d_%d_icon", itemtype, counter);

  // create a drag/drop item
  CEGUI::DragContainer* item = static_cast<CEGUI::DragContainer*>(
    winMgr->createWindow("DragContainer", uniquename));
  item->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.05f)));
  item->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.9f), CEGUI::cegui_reldim(0.9f)));
  item->setTooltipText(itemname);
  // Set the itemID.
  char itemtypestr[1024];
  sprintf(itemtypestr, "%d", itemtype);
  item->setUserString("itemtype" , itemtypestr);
  // Set wether or not the item is stackable
  if (stackable)
  item->setUserString("stackable" , "true");
  else item->setUserString("stackable" , "false");

  // set a static image as drag container's contents
  CEGUI::Window* itemIcon = winMgr->createWindow("TaharezLook/StaticImage");
  item->addChildWindow(itemIcon);
  itemIcon->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0), CEGUI::cegui_reldim(0)));
  itemIcon->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(1), CEGUI::cegui_reldim(1)));
  itemIcon->setProperty("Image", "set:TaharezLook image:CloseButtonNormal");
  // disable to allow inputs to pass through.
  itemIcon->disable();

  //if (stackable)
  item->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDroppedStackable, this));

  return item;
}
void InventoryWindow::UpdateItemCounter(CEGUI::Window* parent)
{
  CEGUI::Window* itemcounter = parent->getChild(30);
  int nrofitems = itemcounter->getParent()->getChildCount()-1;
  char buffer[1024];
  sprintf(buffer, "%d", nrofitems);
  itemcounter->setText((CEGUI::String)buffer);
  if (nrofitems < 2) itemcounter->setVisible(false); else itemcounter->setVisible(true);
}
bool InventoryWindow::AddItem(CEGUI::String itemname, int itemtype, bool stackable)
{
  CEGUI::Window* inventoryframe = winMgr->getWindow("inventoryframe");

  CEGUI::Window* freeslot = 0;
  unsigned int i = 0;

  while ((freeslot == 0) && (i < inventoryframe->getChildCount()))
  {
    CEGUI::Window* slot = inventoryframe->getChildAtIdx(i);
    i += 1;
    printf("Checking slot %d \n", i);

    char itemtypestr[1024];
    sprintf(itemtypestr, "%d", itemtype);
    bool sameid = false;
    // Check if the slot is occupied by a item with the same id.
    if (slot->isUserStringDefined("itemtype")) 
      if (slot->getUserString("itemtype") == itemtypestr) 
      {
        sameid = true;
      }

      if ((slot->getChildCount() < 2 ) || sameid)
      {
        printf("slot %s is empty: Item added to slot\n", slot->getName().c_str());
        freeslot = slot;

        freeslot->addChildWindow(createItemIcon(itemname, itemtype, stackable));
        freeslot->setUserString("itemtype" , itemtypestr);

        UpdateItemCounter(slot);

        return true;
      }
  }
  printf("Inventory is full!\n");
  return false;
}

void InventoryWindow::CreateGUIWindow()
{
  //GUIWindow::CreateGUIWindow ("inventory.xml");

  winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* root = winMgr->getWindow("Root");
  root->subscribeEvent(CEGUI::Window::EventDragDropItemDropped, CEGUI::Event::Subscriber(&InventoryWindow::handleDragDroppedRoot, this));

  // create main rucksack window
  CEGUI::Window* rs = winMgr->createWindow("TaharezLook/FrameWindow","Rucksack");
  root->addChildWindow(rs);
  rs->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.1f), CEGUI::cegui_reldim( 0.25f)));
  rs->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.33f), CEGUI::cegui_reldim( 0.33f)));
  rs->setWindowMaxSize(CEGUI::UVector2(CEGUI::cegui_reldim(1.0f), CEGUI::cegui_reldim( 1.0f)));
  rs->setWindowMinSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.1f), CEGUI::cegui_reldim( 0.1f)));
  rs->setProperty("CloseButtonEnabled","False");

  CEGUI::Window* isf = winMgr->createWindow("TaharezLook/StaticImage", "inventoryframe");
  rs->addChildWindow(isf);
  isf->setWindowPosition(CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim( 0.05f)));
  isf->setWindowSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.9f), CEGUI::cegui_reldim( 0.9f)));
  isf->setWindowMaxSize(CEGUI::UVector2(CEGUI::cegui_reldim(1.0f), CEGUI::cegui_reldim( 1.0f)));
  isf->setWindowMinSize(CEGUI::UVector2(CEGUI::cegui_reldim(0.1f), CEGUI::cegui_reldim( 0.1f)));


  // add slots to rucksack
  CEGUI::Window* startSlot =
    createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.2f)));
  createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.3f), CEGUI::cegui_reldim(0.2f)));
  createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.55f), CEGUI::cegui_reldim(0.2f)));
  createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.05f), CEGUI::cegui_reldim(0.5f)));
  createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.3f), CEGUI::cegui_reldim(0.5f)));
  createDragDropSlot(isf, CEGUI::UVector2(CEGUI::cegui_reldim(0.55f), CEGUI::cegui_reldim(0.5f)));

  // set starting slot for the item.
  // startSlot ->addChildWindow(createItemIcon("apple"));
  // AddItem("apple1");

  // Get the root window
  rootwindow = winMgr->getWindow("Rucksack");

}