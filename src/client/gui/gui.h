/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef GUI_H
#define GUI_H


#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"

#include "CEGUI.h"
#include "ivaria/icegui.h"

#include "common/network/netmessage.h"
#include "common/util/mutex.h"

class Network;
class GUIManager;

/*================*
|| GUI base class ||
*================*/
class GUIWindow
{
private:

protected:
  CEGUI::Window* rootwindow;
  CEGUI::Window* btn;
  CEGUI::WindowManager* winMgr;
  csRef<iCEGUI> cegui;
  csRef<iVFS> vfs;
  Network* network;

  GUIManager* guimanager;

  CEGUI::Window* LoadLayout(const char* layoutFile);

public:
  GUIWindow(GUIManager* guimanager);
  ~GUIWindow();
  void CreateGUIWindow(const char* layoutFile);
  void HideWindow();
  void ShowWindow();
  void EnableWindow();
  void DisableWindow();
};

/*====================//
// GUIConnectWindow   //
//====================*/
class ConnectWindow : public GUIWindow
{
private:
  bool ConnectButtonPressed(const CEGUI::EventArgs& e);   // Executed when the Connect button is pressed.
  csString GetServer();                                   // Return the selected server.

public:
  ConnectWindow(GUIManager* guimanager);
  virtual ~ConnectWindow();
  void CreateGUIWindow();    // load the Login guilayout and register button events.	  
};

/*====================//
// GUILoginWindow     //
//====================*/
class LoginWindow : public GUIWindow
{
private:
  bool LoginButtonPressed(const CEGUI::EventArgs& e);     // Executed when the Connect button is pressed.
  bool RegisterButtonPressed(const CEGUI::EventArgs& e);  // Executed when the Register button is pressed.
  CEGUI::String GetLogin();                               // Return the login input.
  CEGUI::String GetPassword();                            // Return the password input.
  bool RememberLogin();                                   // Return "Remember login" checkbox value.

public:
  LoginWindow(GUIManager* guimanager);
  virtual ~LoginWindow();
  void CreateGUIWindow();    // load the Login guilayout and register button events.
};

/*======================//
// GUISelectCharWindow  //
//======================*/
class SelectCharWindow : public GUIWindow
{
private:
  bool SelectChar(const CEGUI::EventArgs& e);  // Selects the current character.
  bool NewChar(const CEGUI::EventArgs& e);     // Makes a new character.
  bool DelChar(const CEGUI::EventArgs& e);     // Deletes the selected character.
  CEGUI::String GetNewCharName();              // Returns the character name.
  void ScrollLeft();                           // Scroll left throught the characterlist.
  void ScrollRight();                          // Scroll right throught the characterlist.

public:
  SelectCharWindow(GUIManager* guimanager);
  virtual ~SelectCharWindow();
  void CreateGUIWindow();    // load the Login guilayout and register button events.
  void AddCharacter(unsigned int charId, const char* name);
};

/*================//
// GUIChatWindow  //
//================*/
class ChatWindow : public GUIWindow
{
private:
  bool OnSay(const CEGUI::EventArgs& e);
  bool OnShout(const CEGUI::EventArgs& e);
  bool OnWhisper(const CEGUI::EventArgs& e);
  bool OnDropList(const CEGUI::EventArgs& e);
  void CreateDropList();

public:
  ChatWindow(GUIManager* guimanager);
  virtual ~ChatWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.

  void AddChatMessage(csRef<iString> msg);
};

/*================//
// GUIInvWindow  //
//================*/
class InventoryWindow : public GUIWindow
{
private:
  bool handleDragEnter(const CEGUI::EventArgs& args);
  bool handleDragLeave(const CEGUI::EventArgs& args);
  bool handleDragDropped(const CEGUI::EventArgs& args);
  bool handleDragDroppedRoot(const CEGUI::EventArgs& args);
  bool handleDragDroppedStackable(const CEGUI::EventArgs& args);
  CEGUI::Window* createDragDropSlot(CEGUI::Window* parent, const CEGUI::UVector2& position);
  CEGUI::Window* createItemIcon(CEGUI::String itemname, int itemid, bool stackable);
  void UpdateItemCounter(CEGUI::Window* parent);
  int counter;

public:
  InventoryWindow(GUIManager* guimanager);
  virtual ~InventoryWindow();
  void CreateGUIWindow();    // load the chat guilayout and register button events.
  bool AddItem(CEGUI::String itemname, int itemid, bool stackable);
};


#endif // GUI_H
