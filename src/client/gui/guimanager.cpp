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

GUIManager::GUIManager (Client* client)
: client(client)
{
}

GUIManager::~GUIManager ()
{
}

bool GUIManager::Initialize (iObjectRegistry* obj_reg)
{
    cegui = CS_QUERY_REGISTRY(obj_reg, iCEGUI);
    if (!cegui) return client->ReportError("Failed to locate CEGUI plugin");

    // Initialize CEGUI wrapper
    cegui->Initialize ();

    // Set the logging level
    cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

    csRef<iVFS> vfs = client->getVFS ();

    vfs->ChDir ("/client/skin/");

    // Load the ice skin (which uses Falagard skinning system)
    cegui->GetSchemeManagerPtr ()->loadScheme("TaharezLookSkin.scheme");

    cegui->GetSystemPtr ()->setDefaultMouseCursor("TaharezLook", "MouseArrow");
    cegui->GetFontManagerPtr ()->createFont("CommonWealth", "/client/skin/Commonv2c.ttf", 10, 
        CEGUI::Default);
    cegui->GetSystemPtr ()->setTooltip("TaharezLook/Tooltip");

    CEGUI::Window* root = cegui->GetWindowManagerPtr ()->createWindow("DefaultWindow","Root");
    root->setMetricsMode(CEGUI::MetricsMode::Relative);
    root->setAreaRect(CEGUI::Rect(0.0f, 0.0f, 1.0f, 1.0f));

    //root->setAreaRect(CEGUI::Rect(0.0f, 0.0f, 1.0f, 1.0f));
    //root->setMaximumSize(CEGUI::Size(1.0f, 1.0f));
    cegui->GetSystemPtr ()->setGUISheet(root);

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