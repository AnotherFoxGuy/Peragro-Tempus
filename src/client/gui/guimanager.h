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

#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include "cssysdef.h"
#include "csutil/ref.h"
#include "iutil/vfs.h"

#include "ivaria/icegui.h"

class Client;

class LoginWindow;
class ConnectWindow;
class SelectCharWindow;
class ChatWindow;
class InventoryWindow;

struct iObjectRegistry;

class GUIManager
{
private:
  LoginWindow* loginwindow;
  ConnectWindow* connectwindow;
  SelectCharWindow* selectcharwindow;
  ChatWindow* chatwindow;
  InventoryWindow* inventorywindow;
  
  csRef<iCEGUI> cegui;
  
  Client* client;
  
public:
  GUIManager (Client* client);
  ~GUIManager ();
  
  bool Initialize (iObjectRegistry* obj_reg);
  void Render ();
  
  Client* GetClient () {return client;}
  iCEGUI* GetCEGUI () {return cegui;}
  
  ConnectWindow* CreateConnectWindow ();
  LoginWindow* CreateLoginWindow ();
  SelectCharWindow* CreateSelectCharWindow ();
  ChatWindow* CreateChatWindow ();
  InventoryWindow* CreateInventoryWindow();
  
  ConnectWindow* GetConnectWindow ()
  {
    return connectwindow;
  }
  
  LoginWindow* GetLoginWindow ()
  {
    return loginwindow;
  }
  
  SelectCharWindow* GetSelectCharWindow ()
  {
    return selectcharwindow;
  }
  
  ChatWindow* GetChatWindow ()
  {
    return chatwindow;
  }

  InventoryWindow* GetInventoryWindow ()
  {
    return inventorywindow;
  }
  
  //bool ceGUISay (const CEGUI::EventArgs& e);
  //bool ceGUIShout (const CEGUI::EventArgs& e);
  //bool ceGUIWhisper (const CEGUI::EventArgs& e);
};

#endif // GUIMANAGER_H
