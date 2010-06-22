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

#include "client/gui/options-gui.h"

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>

#include "client/network/network.h"

#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      OptionsWindow::OptionsWindow(GUIManager* guimanager)
        : GUIWindow (guimanager)
      {
        windowName = OPTIONSWINDOW;

        currentOption.name = "";
        currentOption.window = "";
      } // end OptionsWindow()

      OptionsWindow::~OptionsWindow()
      {
      } // end ~OptionsWindow()

      OptionsWindow::Option OptionsWindow::FindOption(const char* name)
      {
        for (size_t i = 0; i < optionList.GetSize();i++)
        {
          Option option = optionList.Get(i);
          if (strcmp(option.GetName(), name) == 0)
          {
            return option;
          }
        }// for

        Report(PT::Warning, "Couldn't find option: '%s'", name);
        return currentOption;
      } // end FindOption()

      bool OptionsWindow::ChangeOption(OptionsWindow::Option option)
      {
        CEGUI::Window* btn = guimanager->GetCeguiWindow("Options/Info");
        if (!btn) return false;

        // Remove current child window from the view window.
        GUIWindow* curWindow =
          guimanager->GetWindow<GUIWindow>(currentOption.GetWindow());
        if (curWindow && curWindow->GetCeguiWindow())
          btn->removeChildWindow(curWindow->GetCeguiWindow());

        // And add the new window.
        GUIWindow* window =
          guimanager->GetWindow<GUIWindow>(option.GetWindow());
        if (window && window->GetCeguiWindow())
          btn->addChildWindow(window->GetCeguiWindow());

        return true;
      } // end ChangeOption()

      bool OptionsWindow::OptionPressed(const CEGUI::EventArgs& e)
      {
        using namespace CEGUI;

        const WindowEventArgs& ddea = static_cast<const WindowEventArgs&>(e);

        // Get the listbox.
        CEGUI::Listbox* listbox = static_cast<CEGUI::Listbox*>(ddea.window);
        if (!listbox)
        {
          Report(PT::Error,
            "OptionsWindow: the 'Options/List' window couldn't be found or not a Listbox!");
          return true;
        }

        // Get the item.
        CEGUI::ListboxItem* item = listbox->getFirstSelectedItem();
        if (!item)
        {
          Report(PT::Error, "OptionsWindow: Invalid item!");
          return true;
        }

        // And switch.
        Report(PT::Debug, "Switching to option: '%s'",
          item->getText().c_str());
        Option option = FindOption(item->getText().c_str());
        ChangeOption(option);

        // Make the option current.
        currentOption = option;

        return true;
      } // end OptionPressed()

      bool OptionsWindow::Create()
      {
        ReloadWindow();
        SetupToggleListener("Options");
        return true;
      } // end Create()

      bool OptionsWindow::OptionButtonPressed(const CEGUI::EventArgs& e)
      {
        // Hide the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        btn->setVisible(false);

        // Show the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(true);
        btn->activate();
        return true;
      } // end OptionButtonPressed()

      bool OptionsWindow::OnCloseButton(const CEGUI::EventArgs& e)
      {
        // Show the Options button.
        btn = winMgr->getWindow("Options/Options_Button");
        btn->setVisible(true);

        // Hide the Option menu.
        btn = winMgr->getWindow("Options/Frame");
        btn->setVisible(false);
        return true;
      } // end OnCloseButton()

      bool OptionsWindow::ReloadWindow()
      {
        window = GUIWindow::LoadLayout ("client/options.layout");
        GUIWindow::AddToRoot(window);

        // Hide the Option menu.
        btn = guimanager->GetCeguiWindow("Options/Frame");
        btn->setVisible(false);
        if (btn) btn->subscribeEvent(CEGUI::FrameWindow::EventCloseClicked,
          CEGUI::Event::Subscriber(&OptionsWindow::OnCloseButton, this));

        // Register the selection changed event.
        CEGUI::Window* btn = guimanager->GetCeguiWindow("Options/List");
        if (btn) btn->subscribeEvent(CEGUI::Listbox::EventSelectionChanged,
          CEGUI::SubscriberSlot(&OptionsWindow::OptionPressed, this));

        // Readd the options.
        for (size_t i = 0; i < optionList.GetSize();i++)
        {
          Option option = optionList.Get(i);
          CreateOptionItem(option.GetName());
        }// for

        // Set up the Options button.
        btn = guimanager->GetCeguiWindow("Options/Options_Button");
        guimanager->GetCeguiWindow("Root")->addChildWindow(btn);
        btn->subscribeEvent(CEGUI::PushButton::EventClicked,
          CEGUI::Event::Subscriber(&OptionsWindow::OptionButtonPressed, this));

        return true;
      } // end ReloadWindow()

      bool OptionsWindow::CreateOptionItem(const char* optionName)
      {
        // Get the window.
        CEGUI::Window* btn = guimanager->GetCeguiWindow("Options/List");
        if (!btn) return false;

        // Create and add the item to the list window. CEGUI deletes this.
        CEGUI::ListboxItem* optionItem = new CEGUI::ListboxTextItem(optionName);
        optionItem->setSelectionBrushImage("Peragro", "TextSelectionBrush");
        ((CEGUI::Listbox*)btn)->addItem(optionItem);

        return true;
      } // end CreateOptionItem()

      bool OptionsWindow::AddOption(const char* optionName,
                                    const char* windowName)
      {
        CreateOptionItem(optionName);

        // Add the option to the array.
        Option option;
        option.name = optionName;
        option.window = windowName;
        optionList.Push(option);

        return true;
      } // end AddOption()

    } // Windows namespace
  } // GUI namespace
} // PT namespace

