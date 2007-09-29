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

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iutil/vfs.h>

#include <ivaria/icegui.h>

#include "common/dragdrop-gui.h"

class LoginWindow;
class ConnectWindow;
class SelectCharWindow;
class ChatWindow;
class InventoryWindow;
class HUDWindow;
class OptionsWindow;
class StatusWindow;
class BuddyWindow;
class WhisperWindow;
class NpcDialogWindow;
class TradeWindow;
class ConfirmDialogWindow;
class OkDialogWindow;
class BuyWindow;
class SellWindow;
class InteractDialogWindow;

class DragDrop;

struct iObjectRegistry;

class GUIManager
{
private:
  LoginWindow* loginwindow;
  ConnectWindow* connectwindow;
  SelectCharWindow* selectcharwindow;
  ChatWindow* chatwindow;
  InventoryWindow* inventorywindow;
  HUDWindow* hudwindow;
  OptionsWindow* optionswindow;
  StatusWindow* statuswindow;
  BuddyWindow* buddywindow;
  WhisperWindow* whisperwindow;
  NpcDialogWindow* npcdialogwindow;
  TradeWindow* tradewindow;
  ConfirmDialogWindow* confirmwindow;
  OkDialogWindow* okwindow;
  BuyWindow* buywindow;
  SellWindow* sellwindow;
  InteractDialogWindow* interactwindow;

  DragDrop* dragdrop;

  csRef<iCEGUI> cegui;

public:
  GUIManager ();
  ~GUIManager ();

  bool Initialize ();
  void Render ();

  iCEGUI* GetCEGUI () {return cegui;}
  DragDrop* GetDragDrop (){return dragdrop;}

  ConnectWindow* CreateConnectWindow ();
  LoginWindow* CreateLoginWindow ();
  SelectCharWindow* CreateSelectCharWindow ();
  ChatWindow* CreateChatWindow ();
  InventoryWindow* CreateInventoryWindow();
  HUDWindow* CreateHUDWindow();
  OptionsWindow* CreateOptionsWindow();
  StatusWindow* CreateStatusWindow();
  BuddyWindow* CreateBuddyWindow(); 
  WhisperWindow* CreateWhisperWindow(); 
  NpcDialogWindow* CreateNpcDialogWindow();
  TradeWindow* CreateTradeWindow();
  ConfirmDialogWindow* CreateConfirmWindow();
  OkDialogWindow* CreateOkWindow();
  BuyWindow* CreateBuyWindow();
  SellWindow* CreateSellWindow();
  InteractDialogWindow* CreateInteractDialogWindow();

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

  HUDWindow* GetHUDWindow()
  {
    return hudwindow;
  }

  OptionsWindow* GetOptionsWindow()
  {
    return optionswindow;
  }

  StatusWindow* GetStatusWindow()
  {
    return statuswindow;
  }

  BuddyWindow* GetBuddyWindow()
  {
    return buddywindow;
  }

  WhisperWindow* GetWhisperWindow()
  {
    return whisperwindow;
  }

  NpcDialogWindow* GetNpcDialogWindow()
  {
    return npcdialogwindow;
  }

  TradeWindow* GetTradeWindow()
  {
    return tradewindow;
  }

  ConfirmDialogWindow* GetConfirmWindow()
  {
    return confirmwindow;
  }

  OkDialogWindow* GetOkWindow()
  {
    return okwindow;
  }

  BuyWindow* GetBuyWindow()
  {
    return buywindow;
  }

  SellWindow* GetSellWindow()
  {
    return sellwindow;
  }

  InteractDialogWindow* GetInteractDialogWindow()
  {
    return interactwindow;
  }

};

#endif // GUIMANAGER_H
