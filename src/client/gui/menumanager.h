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

#ifndef PT_GUI_MENUMANAGER_H
#define PT_GUI_MENUMANAGER_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/array.h>
#include "menuitem.h"
#include "menuitemwindow.h"

#include <CEGUI.h>

struct iObjectRegistry;
struct iVFS;
struct iCEGUI;

namespace PT
{
  namespace GUI
  {
    class MenuManager
    {
    private:
      bool isDragging;

      typedef std::map<MenuItem*, MenuItemWindow*> MenuEntrys;
      MenuEntrys menuItems;

    public:

      MenuManager();
      virtual ~MenuManager();

      /* AddMenuItem
      *
      * Adds a newly created item to the manager.
      *
      * @param		menuItem		The menuItem to add.
      * @param		menuWindow	The registrant.
      * \return True if successful, false if not.
      */
      bool AddMenuItem(MenuItem * menuItem, MenuItemWindow * menuWindow);

      /* RemoveMenuItem
      *
      * Removes a menu item from the manager.
      *
      * @param	menuItem	The menuItem to remove.
      * \return True if successful, false if not.
      */
      bool RemoveMenuItem(MenuItem * menuItem);

      /* GetMenuEntry
      *
      * Get a menu entry.
      *
      * @param		dragcontainer		The dragcontainer associated to this menu entry.
      * \return A reference to a std::pair, containing pointers to MenuItem and MenuWindow.
      */
      std::pair<MenuItem*, MenuItemWindow*> GetMenuEntry(CEGUI::DragContainer * dragcontainer);

      /* GetMenuEntry
      *
      * Get a menu entry.
      *
      * @param		name	The name of the menu item.
      * \return A reference to a std::pair, containing pointers to MenuItem and MenuWindow.
      */
      std::pair<MenuItem*, MenuItemWindow*> GetMenuEntry(const std::string& name);

      /* GetMenuWindow
      *
      * Get a menu (item) Window.
      *
      * @param		menuItem	The menu item associated to this menu window.
      * \return	A pointer to the menu (item) window.
      */
      MenuItemWindow * GetMenuWindow(MenuItem * menuItem);

      /* IsDragging
      *
      * Return if something is currently dragged.
      *
      * \return	True if so, false if not.
      */
      bool IsDragging() const;

      /* Reload
      *
      * Reloads the Menu manager by renewing all pointers to CEGUI windows.
      *
      * \return void.
      */
      void Reload();

    protected:

      /* OnItemClicked
      *
      * Called when a item has been clicked, can only be called by a MenuItem
      *  instance.
      *
      * \return	void.
      */
      void OnItemClicked(MenuItem * item);
      friend class MenuItem;

      /* Initialize
      *
      * Adds the current items defined by the xml to the manager.
      *
      * \return void.
      */
      void Initialize();

      bool OnDragItemDropped(const CEGUI::EventArgs &e);
      bool OnDragStarted(const CEGUI::EventArgs &e);
      bool OnDragEnded(const CEGUI::EventArgs &e);


    };


  }
}



#endif // PT_GUI_MENUMANAGER_H
