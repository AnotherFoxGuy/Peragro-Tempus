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

#include "client/gui/serversetup-gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h"
#include "CEGUILogger.h"

#include "client/pointer/pointer.h"

#include "client/serversetup/serversetupmanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      ServerSetupWindow::ServerSetupWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
         windowName = SERVERSETUPWINDOW;
      } // end ServerSetupWindow()

      ServerSetupWindow::~ServerSetupWindow()
      {
      } // end ~ServerSetupWindow()

      bool ServerSetupWindow::DoneButton(const CEGUI::EventArgs& e)
      {
        btn = winMgr->getWindow("CharSelect/Frame");
        btn->setVisible(true);
        btn = winMgr->getWindow("ServerSetup/Frame");
        btn->setVisible(false);
        return true;
      } // end DoneButton()

      bool ServerSetupWindow::UploadButton(const CEGUI::EventArgs& e)
      {
        PointerLibrary::getInstance()->
          getServerSetupManager()->UploadServerData();
        return true;
      } // end UploadButton()

      bool ServerSetupWindow::DownloadButton(const CEGUI::EventArgs& e)
      {
        PointerLibrary::getInstance()->
          getServerSetupManager()->DownloadServerData();
        return true;
      } // end DownloadButton()

      bool ServerSetupWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool ServerSetupWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/serversetup.layout");
        GUIWindow::AddToRoot(window);

        winMgr = cegui->GetWindowManagerPtr ();

        // Register the button events.
        btn = winMgr->getWindow("ServerSetup/Done");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&ServerSetupWindow::DoneButton, this));
        btn = winMgr->getWindow("ServerSetup/Upload");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&ServerSetupWindow::UploadButton, this));
        btn = winMgr->getWindow("ServerSetup/Download");
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&ServerSetupWindow::DownloadButton, this));

        btn = winMgr->getWindow("ServerSetup/Frame");
        btn->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&ServerSetupWindow::DoneButton, this));

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

