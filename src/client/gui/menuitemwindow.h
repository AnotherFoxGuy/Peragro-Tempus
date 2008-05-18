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

#ifndef PT_GUI_MENUITEMWINDOW_H
#define PT_GUI_MENUITEMWINDOW_H

#include <cssysdef.h>

#include <string>
#include <CEGUI.h>


namespace PT
{
  namespace GUI
  {
    class MenuItemWindow
    {
    protected:

      bool isOpened;

    public:

      MenuItemWindow() { isOpened = false; };
      virtual ~MenuItemWindow() { };

      /* CreateWindow
      *
      * Creates the window.
      *
      * @param    position    Where to place the window.
      * \return void.
      */
      virtual void Create(CEGUI::UVector2 position) = 0;

      /* GetWindow
      *
      * Returns the window where the layout is created.
      *
      * \return    A window.
      */
      virtual CEGUI::Window * GetWindow() = 0;

      /* SetOpened
      *
      * Set if this window is opened or not.
      *
      * @param    state        The state.
      * \return void.
      */
      void SetOpened(bool state) { isOpened = state; }

      /* GetOpened
      *
      * Get if this window is opened or not.
      *
      * \return    The state.
      */
      bool GetOpened() { return isOpened; }
    };
  }
}



#endif // PT_GUI_MENUITEMWINDOW_H
