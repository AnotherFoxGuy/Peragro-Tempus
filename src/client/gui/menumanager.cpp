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

#include "client/gui/menumanager.h"

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/array.h>
#include <CEGUI.h>

#include <iostream>
#include <algorithm>


namespace PT
{
  namespace GUI
  {

    MenuManager::MenuManager()
    {
      isDragging = false;
      Initialize();
    } // end MenuManager()

    MenuManager::~MenuManager()
    {
    } // end ~MenuManager()

    void MenuManager::Initialize()
    {
      CEGUI::GlobalEventSet * gbset = CEGUI::GlobalEventSet::getSingletonPtr();
      gbset->subscribeEvent(
        CEGUI::Window::EventNamespace + "/" +
        CEGUI::Window::EventDragDropItemDropped,
        CEGUI::SubscriberSlot(&MenuManager::OnDragItemDropped, this)
        );

      gbset->subscribeEvent(
        CEGUI::DragContainer::EventNamespace + "/" +
        CEGUI::DragContainer::EventDragStarted,
        CEGUI::SubscriberSlot(&MenuManager::OnDragStarted, this)
        );

      gbset->subscribeEvent(
        CEGUI::DragContainer::EventNamespace + "/" +
        CEGUI::DragContainer::EventDragEnded,
        CEGUI::SubscriberSlot(&MenuManager::OnDragEnded, this)
        );

    } // end Initialize()

    bool MenuManager::AddMenuItem(MenuItem * menuItem,
                                  MenuItemWindow * menuWindow)
    {
      MenuEntrys::iterator result = menuItems.find(menuItem);

      if (result != menuItems.end())
      {
        // This item already exists!
        return false;
      }

      menuItems[menuItem] = menuWindow;
      return true;
    } // end AddMenuItem()

    bool MenuManager::RemoveMenuItem(MenuItem * menuItem)
    {
      MenuEntrys::iterator result = menuItems.find(menuItem);

      if (result != menuItems.end())
      {
        menuItems.erase(result);
        return true;
      }
      return false;
    } // end RemoveMenuItem()

    std::pair<MenuItem*, MenuItemWindow*>
      MenuManager::GetMenuEntry(CEGUI::DragContainer * dragcontainer)
    {
      MenuEntrys::iterator ppkNode = menuItems.begin();
      MenuEntrys::iterator ppkEnd = menuItems.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        if ((*ppkNode).first->GetDragContainer() == dragcontainer)
        {
          return (*ppkNode);
        }
      }
      return std::pair<MenuItem*, MenuItemWindow*>(0, 0);
    } // end GetMenuEntry()

    std::pair<MenuItem*, MenuItemWindow*>
      MenuManager::GetMenuEntry(const std::string& name)
    {
      MenuEntrys::iterator ppkNode = menuItems.begin();
      MenuEntrys::iterator ppkEnd = menuItems.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        if ((*ppkNode).first->GetName() == name)
        {
          return (*ppkNode);
        }
      }
      return std::pair<MenuItem*, MenuItemWindow*>(0, 0);
    } // end GetMenuEntry()

    MenuItemWindow * MenuManager::GetMenuWindow(MenuItem * menuItem)
    {
      return menuItems[menuItem];
    } // end GetMenuWindow()

    bool MenuManager::IsDragging() const
    {
      return isDragging;
    }

    void MenuManager::Reload()
    {
      MenuEntrys tempMap;

      MenuEntrys::iterator ppkNode = menuItems.begin();
      MenuEntrys::iterator ppkEnd = menuItems.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        MenuItem * menuItem = (*ppkNode).first;
        MenuItemWindow * menuItemWindow = (*ppkNode).second;

        tempMap[
          new MenuItem(CEGUI::WindowManager::getSingletonPtr()->
            getWindow(menuItem->GetBase()),
          this)] = menuItemWindow;
      }

      menuItems.clear();
      menuItems.insert(tempMap.begin(), tempMap.end());
    } // end Reload()

    void MenuManager::OnItemClicked(MenuItem * item)
    {
      MenuItemWindow * menuItemWindow = GetMenuWindow(item);
      if (menuItemWindow->GetOpened())
      {
        return;
      }
      // I suppose that is standard position.
      menuItemWindow->Create(menuItemWindow->GetWindow()->getPosition());
    } // end OnItemClicked()

    bool MenuManager::OnDragItemDropped(const CEGUI::EventArgs &e)
    {
      // Determine what menu item has been dragged.
      const CEGUI::DragDropEventArgs& ddea =
        static_cast<const CEGUI::DragDropEventArgs&>(e);
      std::pair<MenuItem*, MenuItemWindow*> menuEntry =
        GetMenuEntry(ddea.dragDropItem);
      if (menuEntry.first != 0 || menuEntry.second != 0)
      {
        // One of our menu items got dropped somewhere.
        isDragging = false;

        if (menuEntry.second->GetOpened())
        {
          // Our window is already opened.
          return true;
        }
        // Show the window on mouse cursor's position.
        CEGUI::Point pt = CEGUI::MouseCursor::getSingletonPtr()->getPosition();
        menuEntry.second->Create(CEGUI::UVector2(CEGUI::UDim(0.0f, pt.d_x),
          CEGUI::UDim(0.0f, pt.d_y)));
      }
      return true;
    } // end OnDragItemDropped()

    bool MenuManager::OnDragStarted(const CEGUI::EventArgs &e)
    {
      // Determine if a menu item started dragging.
      const CEGUI::WindowEventArgs& ddea =
        static_cast<const CEGUI::WindowEventArgs&>(e);
      std::pair<MenuItem*, MenuItemWindow*> menuEntry =
        GetMenuEntry((CEGUI::DragContainer*)ddea.window);
      if (menuEntry.first != 0 || menuEntry.second != 0)
      {
        // We started dragging one of our menu items.
        isDragging = true;
      }
      return true;
    } // end OnDragStarted()

    bool MenuManager::OnDragEnded(const CEGUI::EventArgs &e)
    {
      // Determine if a menu item ended dragging.
      const CEGUI::WindowEventArgs& ddea =
        static_cast<const CEGUI::WindowEventArgs&>(e);
      std::pair<MenuItem*, MenuItemWindow*> menuEntry =
        GetMenuEntry((CEGUI::DragContainer*)ddea.window);
      if (menuEntry.first != 0 || menuEntry.second != 0)
      {
        isDragging = false;
      }
      return true;
    } // end OnDragEnded()

  } // GUI namespace
} // PT namespace

