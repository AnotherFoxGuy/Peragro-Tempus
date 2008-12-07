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

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      BuddyWindow::BuddyWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = BUDDYLISTWINDOW;
      } // end BuddyWindow()

      BuddyWindow::~BuddyWindow()
      {
      } // end ~BuddyWindow()

      bool BuddyWindow::handleCloseButton(const CEGUI::EventArgs& args)
      {
        winMgr->getWindow("BuddyList/Frame")->setVisible(false);
        return true;
      } // end handleCloseButton()

      void BuddyWindow::AddPlayer(const char* name)
      {
        btn = winMgr->getWindow("BuddyList/SkillTab");
        CEGUI::ListboxItem* nameItem = new CEGUI::ListboxTextItem(name);

        unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
        ((CEGUI::MultiColumnList*)btn)->setItem(nameItem, 0, row);
      } // end AddPlayer()

      void BuddyWindow::RemovePlayer(const char* name)
      {
        btn = winMgr->getWindow("BuddyList/SkillTab");

        CEGUI::ListboxItem* nameItem =
          ((CEGUI::MultiColumnList*)btn)->findListItemWithText(name,0);
        if (!nameItem)
        {
          Report(PT::Error,
            "BuddyWindow: ERROR Couldn't find player %s in buddylist!", name);
          return;
        }
        nameItem->setAutoDeleted(true);
        unsigned int row =
          ((CEGUI::MultiColumnList*)btn)->getItemRowIndex(nameItem);
        ((CEGUI::MultiColumnList*)btn)->removeRow(row);
      } // end RemovePlayer()

      bool BuddyWindow::ProcessEvents(iEvent& ev)
      {
        using namespace PT::Events;
        EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

        csStringID id = ev.GetName();

        if (evmgr->IsKindOf(id, "entity.add"))
        {
          if (EntityHelper::GetEntityType(&ev, evmgr) == PT::Common::Entity::PCEntityType)
          {
            const char * nick = 0;
            ev.Retrieve("entityName", nick);
            AddPlayer(nick);
          }
        }
        else if (evmgr->IsKindOf(id, "entity.remove"))
        {
          unsigned int entid = EntityHelper::GetEntityID(&ev);

          PT::Entity::Entity* ent = PointerLibrary::getInstance()->
            getEntityManager()->findPtEntById(entid);
          if (!ent) return false;

          if (ent->GetType() == PT::Common::Entity::PCEntityType)
          {
            RemovePlayer(ent->GetName().c_str());
          }
        }

        return true;
      } // end ProcessEvents()

      bool BuddyWindow::Create()
      {
        ReloadWindow();
        SetupToggleListener("BuddyList");
        return true;
      }  // end Create()

      bool BuddyWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/buddylist.xml");
        GUIWindow::AddToRoot(window);

        // Create the UserTab
        CEGUI::MultiColumnList* usertab = static_cast<CEGUI::MultiColumnList*>
          (winMgr->createWindow("Peragro/MultiColumnList","BuddyList/SkillTab"));
        usertab->
          setPosition(CEGUI::UVector2(CEGUI::UDim(0,0), CEGUI::UDim(0,0)));
        usertab->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
        CEGUI::String name("Name");
        usertab->addColumn(name,0,CEGUI::UDim(0.9f,0));
        usertab->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
        usertab->setUserColumnSizingEnabled(false);
        usertab->setProperty("Font", "Commonwealth-8");
        usertab->setText(" Players");

        // Get the tabcontrol.
        CEGUI::TabControl* tabcontrol = static_cast<CEGUI::TabControl*>
          (winMgr->getWindow("BuddyList/TabControl"));
        tabcontrol->setTabHeight(CEGUI::UDim(0.10f,0));
        tabcontrol->addTab(usertab);
        //EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();

        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("BuddyList/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&BuddyWindow::handleCloseButton, this));

        PT_SETUP_HANDLER
        PT_REGISTER_LISTENER(BuddyWindow, ProcessEvents, "entity.add")
        PT_REGISTER_LISTENER(BuddyWindow, ProcessEvents, "entity.remove")

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

