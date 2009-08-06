/*
  Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "client/gui/options/chatoptions-gui.h"

#include <iutil/objreg.h>
#include <iutil/cfgmgr.h>

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "common/event/eventmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      ChatOptionsWindow::ChatOptionsWindow(GUIManager* guiManager)
        : GUIWindow (guiManager)
      {
        windowName = CHATOPTIONSWINDOW;
      } // end ChatOptionsWindow()

      ChatOptionsWindow::~ChatOptionsWindow()
      {
      } // end ~ChatOptionsWindow()

      void ChatOptionsWindow::SaveConfig()
      {
        app_cfg->Save();
      } // end SaveConfig()

      bool ChatOptionsWindow::Create()
      {
        app_cfg = csQueryRegistry<iConfigManager>
          (PointerLibrary::getInstance()->getObjectRegistry());
        if (!app_cfg)
        {
          Report(PT::Error, "Can't find the config manager!");
          return false;
        }

        ReloadWindow();

        return true;
      } // end Create()

      void ChatOptionsWindow::CreateHideAfterSendCheckBox()
      {
        btn = winMgr->getWindow("Options/Chat/HideAfterSend");

        bool fs = app_cfg->GetBool("Peragro.Chat.HideAfterSending");

        ((CEGUI::Checkbox*)btn)->setSelected(fs);
      }

      bool ChatOptionsWindow::OnHideAfterSendCheckBox(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("Options/Chat/HideAfterSend");
        bool ya = ((CEGUI::Checkbox*)btn)->isSelected();

        app_cfg->SetBool("Peragro.Chat.HideAfterSending", ya);
        PT::Events::EventManager* evmgr =
          PointerLibrary::getInstance()->getEventManager();
        evmgr->AddEvent(evmgr->CreateEvent("interface.options.chat"));
        SaveConfig();
        return true;
      }

      bool ChatOptionsWindow::ReloadWindow()
      {
        // Load the layout.
        window = GUIWindow::LoadLayout ("client/options/chat.layout");

        // Set up the "hide input field after sending" checkbox.
        CreateHideAfterSendCheckBox();
        btn = winMgr->getWindow("Options/Chat/HideAfterSend");
        btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged,
          CEGUI::Event::Subscriber(&ChatOptionsWindow::OnHideAfterSendCheckBox, this));

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

