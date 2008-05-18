/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

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

#include "menuitem.h"
#include "menumanager.h"
#include <vector>
#include <cctype>
#include <algorithm>

namespace PT
{
  namespace GUI
  {

    MenuItem::MenuItem (CEGUI::Window * dragcontainer, MenuManager * menumanager)
    {
      menuManager = menumanager;
      dragContainer = (CEGUI::DragContainer*)dragcontainer;
      dragcontainer->subscribeEvent(CEGUI::Window::EventMouseEnters, CEGUI::SubscriberSlot(&MenuItem::OnMouseEnters, this));
      dragcontainer->subscribeEvent(CEGUI::Window::EventMouseLeaves, CEGUI::SubscriberSlot(&MenuItem::OnMouseLeaves, this));
      dragcontainer->subscribeEvent(CEGUI::Window::EventMouseButtonDown, CEGUI::SubscriberSlot(&MenuItem::OnButtonDown, this));
      dragcontainer->subscribeEvent(CEGUI::Window::EventMouseButtonUp, CEGUI::SubscriberSlot(&MenuItem::OnButtonUp, this));
      Initialize();
    }

    MenuItem::~MenuItem ()
    {
    }

    void MenuItem::Initialize()
    {
      CEGUI::WindowManager * winMgr = CEGUI::WindowManager::getSingletonPtr();
      base = dragContainer->getName().c_str();
      CEGUI::String baseTemp = base.substr(0, base.length() - 4);

      std::string tempBase = baseTemp.substr(12, base.length() - 1).c_str();
      std::string firstChar = tempBase.substr(0, 1);
      std::string rest = tempBase.substr(1, tempBase.length());
      std::transform(firstChar.begin(), firstChar.end(), firstChar.begin(), static_cast<int(*)(int)>(::toupper));
      imageBase = firstChar + rest;
      imageBase = imageBase.erase(imageBase.length() - 1, 1);

      iconButton = (CEGUI::PushButton*)winMgr->getWindow(baseTemp + "icon");
      textButton = (CEGUI::PushButton*)winMgr->getWindow(baseTemp + "text");

      name = textButton->getText().c_str();
    }

    CEGUI::DragContainer * MenuItem::GetDragContainer () const
    {
      return dragContainer;
    }

    CEGUI::PushButton * MenuItem::GetIconButton () const
    {
      return iconButton;
    }

    CEGUI::PushButton * MenuItem::GetTextButton () const
    {
      return textButton;
    }

    const std::string MenuItem::GetName() const
    {
      return name;
    }

    const std::string MenuItem::GetBase() const
    {
      return base;
    }

    bool MenuItem::OnMouseEnters(const CEGUI::EventArgs &e)
    {
      textButton->setProperty("DisabledTextColour", "FFBBBBFF");
      std::string value = "set:Buttons image:Button_" + imageBase + "_Over";
      iconButton->setProperty("NormalImage", value);
      return true;
    }

    bool MenuItem::OnMouseLeaves(const CEGUI::EventArgs &e)
    {
      textButton->setProperty("DisabledTextColour", "FFFFFFFF");
      std::string value = "set:Buttons image:Button_" + imageBase + "_Normal";
      iconButton->setProperty("NormalImage", value);
      return true;
    }

    bool MenuItem::OnButtonDown(const CEGUI::EventArgs &e)
    {
      using namespace CEGUI;

      const CEGUI::MouseEventArgs& args = static_cast<const CEGUI::MouseEventArgs&>(e);

      if (args.button == LeftButton)
      {
        textButton->setProperty("DisabledTextColour", "FFFFFFFF");
        std::string value = "set:Buttons image:Button_" + imageBase + "_Pushed";
        iconButton->setProperty("NormalImage", value);
      }

      return true;
    }

    bool MenuItem::OnButtonUp(const CEGUI::EventArgs &e)
    {
      using namespace CEGUI;

      const MouseEventArgs& args = static_cast<const MouseEventArgs&>(e);

      if (args.button == LeftButton)
      {
        textButton->setProperty("DisabledTextColour", "FFBBBBFF");
        std::string value = "set:Buttons image:Button_" + imageBase + "_Over";
        iconButton->setProperty("NormalImage", value);

        if (!menuManager->IsDragging())
        {
          menuManager->OnItemClicked(this);
        }
      }
      return true;
    }



  }
}
