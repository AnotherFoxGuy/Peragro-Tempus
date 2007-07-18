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

#include "client/gui/guimanager.h"

#include "iutil/objreg.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/client.h"
#include "client/network/network.h"

#include "gui.h"


GUIManager::GUIManager (PT::Client* client)
: client(client)
{
  dragdrop = 0;
  loginwindow = 0;
  connectwindow = 0;
  selectcharwindow = 0;
  chatwindow = 0;
  inventorywindow = 0;
  hudwindow = 0;
  optionswindow = 0;
  statuswindow = 0;
  buddywindow = 0;
  whisperwindow = 0;
  npcdialogwindow = 0;
  confirmwindow = 0;
  okwindow = 0;
  buywindow = 0;
  sellwindow = 0;
}

GUIManager::~GUIManager ()
{
  delete dragdrop;
}

bool GUIManager::Initialize ()
{
    iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

    cegui =  csQueryRegistry<iCEGUI> (obj_reg);
    if (!cegui) return client->ReportError("Failed to locate CEGUI plugin");

    // Initialize CEGUI wrapper
    cegui->Initialize ();

    // Set the logging level
    cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

    csRef<iVFS> vfs =  csQueryRegistry<iVFS> (obj_reg);
    vfs->ChDir ("/peragro/skin/");

    // Load the ice skin (which uses Falagard skinning system)
    cegui->GetSchemeManagerPtr ()->loadScheme("Peragro.scheme");

    cegui->GetSystemPtr ()->setDefaultMouseCursor("Peragro", "MouseArrow");
    CEGUI::Font* font = cegui->GetFontManagerPtr ()->createFont("FreeType", "Commonwealth-10",
      "/peragro/skin/Commonv2c.ttf");
    font->setProperty("PointSize", "10");
    font->load();

    CEGUI::Font* font2 = cegui->GetFontManagerPtr ()->createFont("FreeType", "Commonwealth-8",
      "/peragro/skin/Commonv2c.ttf");
    font2->setProperty("PointSize", "8");
    font2->load();

    cegui->GetSystemPtr ()->setDefaultTooltip("Peragro/Tooltip");

    CEGUI::Window* root = cegui->GetWindowManagerPtr ()->createWindow("DefaultWindow","Root");
    root->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
      CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
    root->setMaxSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
    cegui->GetSystemPtr ()->setGUISheet(root);

    // Set up the drag and drop.
    dragdrop = new DragDrop (this);
    if (!dragdrop) return client->ReportError("Failed to create DragDrop class");

    return true;
}

void GUIManager::Render ()
{
  cegui->Render ();
}

ConnectWindow* GUIManager::CreateConnectWindow ()
{
  connectwindow = new ConnectWindow (this);
  connectwindow->CreateGUIWindow();
  return connectwindow;
}

LoginWindow* GUIManager::CreateLoginWindow ()
{
  loginwindow = new LoginWindow (this);
  loginwindow->CreateGUIWindow();
  return loginwindow;
}

SelectCharWindow* GUIManager::CreateSelectCharWindow ()
{
  selectcharwindow = new SelectCharWindow (this);
  selectcharwindow->CreateGUIWindow();
  return selectcharwindow;
}

ChatWindow* GUIManager::CreateChatWindow ()
{
  chatwindow = new ChatWindow (this);
  chatwindow->CreateGUIWindow();
  return chatwindow;
}

InventoryWindow* GUIManager::CreateInventoryWindow ()
{
  inventorywindow = new InventoryWindow (this);
  inventorywindow->CreateGUIWindow();
  return inventorywindow;
}

HUDWindow* GUIManager::CreateHUDWindow ()
{
  hudwindow = new HUDWindow (this);
  hudwindow->CreateGUIWindow();
  return hudwindow;
}

OptionsWindow* GUIManager::CreateOptionsWindow ()
{
  optionswindow = new OptionsWindow (this);
  optionswindow->CreateGUIWindow();
  return optionswindow;
}

StatusWindow* GUIManager::CreateStatusWindow ()
{
  statuswindow = new StatusWindow (this);
  statuswindow->CreateGUIWindow();
  return statuswindow;
}

BuddyWindow* GUIManager::CreateBuddyWindow ()
{
  buddywindow = new BuddyWindow (this);
  buddywindow->CreateGUIWindow();
  return buddywindow;
}

WhisperWindow* GUIManager::CreateWhisperWindow ()
{
  whisperwindow = new WhisperWindow (this);
  whisperwindow->CreateGUIWindow();
  return whisperwindow;
}

NpcDialogWindow* GUIManager::CreateNpcDialogWindow ()
{
  npcdialogwindow = new NpcDialogWindow (this);
  npcdialogwindow->CreateGUIWindow();
  return npcdialogwindow;
}

TradeWindow* GUIManager::CreateTradeWindow ()
{
  tradewindow = new TradeWindow (this);
  tradewindow->CreateGUIWindow();
  return tradewindow;
}

ConfirmDialogWindow* GUIManager::CreateConfirmWindow ()
{
  if(!confirmwindow)
    confirmwindow = new ConfirmDialogWindow (this);
  confirmwindow->CreateGUIWindow();
  return confirmwindow;
}

OkDialogWindow* GUIManager::CreateOkWindow ()
{
  if(!okwindow)
    okwindow = new OkDialogWindow (this);
  okwindow->CreateGUIWindow();
  return okwindow;
}

BuyWindow* GUIManager::CreateBuyWindow ()
{
  buywindow = new BuyWindow (this);
  buywindow->CreateGUIWindow();
  return buywindow;
}

SellWindow* GUIManager::CreateSellWindow ()
{
  sellwindow = new SellWindow (this);
  sellwindow->CreateGUIWindow();
  return sellwindow;
}
