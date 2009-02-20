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

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

      StatusWindow::StatusWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
         windowName = STATUSWINDOW;
      } // end StatusWindow()

      StatusWindow::~StatusWindow()
      {
      } // end ~StatusWindow()

      bool StatusWindow::handleCloseButton(const CEGUI::EventArgs& args)
      {
        winMgr->getWindow("Status/Frame")->setVisible(false);
        //winMgr->getWindow("Chatlog/Frame")->activate();
        return true;
      } // end handleCloseButton()

      CEGUI::Window* StatusWindow::createDragDropSlot(CEGUI::Window* parent,
                                                      const CEGUI::UVector2& position)
      {
        // Create the slot
        CEGUI::Window* slot = winMgr->createWindow("Peragro/StaticImage");
        parent->addChildWindow(slot);
        slot->setPosition(position);
        slot->setSize(CEGUI::UVector2(CEGUI::UDim(0,24.0f),
          CEGUI::UDim(0,24.0f)));

        return slot;
      } // end createDragDropSlot()

      CEGUI::Window* StatusWindow::createItemIcon(CEGUI::String itemname,
                                                  int itemtype, bool stackable)
      {
        using namespace CEGUI;

        char uniquename[1024];
        counter += 1;
        snprintf(uniquename, 1024, "%d_%d_icon", itemtype, counter);

        // create a drag/drop item
        CEGUI::DragContainer* item = static_cast<CEGUI::DragContainer*>
          (winMgr->createWindow("DragContainer", uniquename));
        item->setPosition(CEGUI::UVector2(cegui_reldim(0.05f),
          cegui_reldim(0.05f)));
        item->setSize(CEGUI::UVector2(cegui_reldim(0.9f),
          cegui_reldim(0.9f)));
        item->setTooltipText(itemname);
        // Set the itemID.
        char itemtypestr[1024];
        snprintf(itemtypestr, 1024, "%d", itemtype);
        item->setUserString("itemtype" , itemtypestr);
        // Set wether or not the item is stackable
        if (stackable)
          item->setUserString("stackable" , "true");
        else item->setUserString("stackable" , "false");

        // set a static image as drag container's contents
        CEGUI::Window* itemIcon = winMgr->createWindow("Peragro/StaticImage");
        item->addChildWindow(itemIcon);
        itemIcon->setPosition(CEGUI::UVector2(cegui_reldim(0),
          cegui_reldim(0)));
        itemIcon->setSize(CEGUI::UVector2(cegui_reldim(1),
          cegui_reldim(1)));
        //itemIcon->setProperty("Image", "set:Peragro image:CloseButtonNormal");
        char inventoryimage[1024];
        snprintf(inventoryimage, 1024, "set:Inventory image:%d", itemtype);
        itemIcon->setProperty("Image", inventoryimage);
        // disable to allow inputs to pass through.
        itemIcon->disable();

        return item;
      } // end createItemIcon()

      void StatusWindow::AddSkill(const char* skillname, unsigned int skillvalue)
      {
        btn = winMgr->getWindow("Status/SkillTab");
        char charskillvalue[10];
        snprintf(charskillvalue, 10, "%d", skillvalue);
        CEGUI::ListboxItem* skillvalueItem =
          new CEGUI::ListboxTextItem(charskillvalue);
        CEGUI::ListboxItem* skillnameItem =
          new CEGUI::ListboxTextItem(skillname);

        //skillvalueItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
        //  (CEGUI::utf8*)"TextSelectionBrush");
        //skillnameItem->setSelectionBrushImage((CEGUI::utf8*)"Peragro",
        //  (CEGUI::utf8*)"TextSelectionBrush");

        unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
        ((CEGUI::MultiColumnList*)btn)->setItem(skillnameItem, 0, row);
        ((CEGUI::MultiColumnList*)btn)->setItem(skillvalueItem, 1, row);
        //((CEGUI::MultiColumnList*)btn)->
        //  setSelectionMode(CEGUI::MultiColumnList::RowSingle);
      } // end AddSkil()

      bool StatusWindow::Create()
      {
        ReloadWindow();
        return true;
      } // end Create()

      bool StatusWindow::ReloadWindow()
      {
        using namespace CEGUI;

        window = GUIWindow::LoadLayout ("client/status.layout");
        GUIWindow::AddToRoot(window);

        winMgr = cegui->GetWindowManagerPtr ();

        // Create the SkillTab
        CEGUI::MultiColumnList* skilltab = static_cast<CEGUI::MultiColumnList*>
          (winMgr->createWindow("Peragro/MultiColumnList","Status/SkillTab"));
        skilltab->setPosition(CEGUI::UVector2(cegui_reldim(0),
          cegui_reldim(0)));
        skilltab->setSize(CEGUI::UVector2(cegui_reldim(1), cegui_reldim(1)));
        CEGUI::String str_skill("Skill");
        CEGUI::String str_value("Value");
        skilltab->addColumn(str_skill,0,CEGUI::UDim(0.65f,0));
        skilltab->addColumn(str_value,1,CEGUI::UDim(0.35f,0));
        skilltab->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
        skilltab->setUserColumnSizingEnabled(false);
        skilltab->setProperty("Font", "Commonwealth-8");
        skilltab->setText("Skills");

        // Create the SpellTab
        CEGUI::Window* spelltab = winMgr->createWindow("Peragro/StaticImage",
          "Status/SpellTab");
        spelltab->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0),
          CEGUI::UDim(0,0)));
        spelltab->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
        spelltab->setProperty("Font", "Commonwealth-8");
        spelltab->setText("Spells");
        for (int j=0; j<6; j++)
        {
          for (int i=0; i<5; i++)
          {
            createDragDropSlot(spelltab, CEGUI::UVector2(
              CEGUI::UDim(0,4.0f+(28*i)), CEGUI::UDim(0,4.0f+(28*j))));
          }
        }

        // Create the QuestTab
        CEGUI::Window* questtab =
          winMgr->createWindow("Peragro/StaticImage","Status/QuestTab");
        questtab->setPosition(CEGUI::UVector2(CEGUI::UDim(0,0),
          CEGUI::UDim(0,0)));
        questtab->setSize(CEGUI::UVector2(CEGUI::UDim(1,0), CEGUI::UDim(1,0)));
        questtab->setProperty("Font", "Commonwealth-8");
        questtab->setText("Quests");

        // Get the tabcontrol.
        CEGUI::TabControl* tabcontrol = static_cast<CEGUI::TabControl*>
          (winMgr->getWindow("Status/TabControl"));
        tabcontrol->setTabHeight(CEGUI::UDim(0.10f,0));
        tabcontrol->addTab(skilltab);
        tabcontrol->addTab(spelltab);
        tabcontrol->addTab(questtab);


        // Get the frame window
        CEGUI::FrameWindow* frame = static_cast<CEGUI::FrameWindow*>
          (winMgr->getWindow("Status/Frame"));
        frame->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&StatusWindow::handleCloseButton, this));

        return true;
      } // end ReloadWindow()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

