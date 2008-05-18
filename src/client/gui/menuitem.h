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

#ifndef PT_GUI_MENUITEM_H
#define PT_GUI_MENUITEM_H

#include <cssysdef.h>

#include <string>
#include <CEGUI.h>


namespace PT
{
  namespace GUI
  {
    class MenuManager;

    class MenuItem
    {
    protected:

      CEGUI::DragContainer * dragContainer;
      CEGUI::PushButton * iconButton;
      CEGUI::PushButton * textButton;
      std::string name;
      std::string base;
      std::string imageBase;

      MenuManager * menuManager;

    public:

      MenuItem(CEGUI::Window * dragcontainer, MenuManager * menumanager);
      virtual ~MenuItem();

      /* GetDragContainer
      *
      * Get the drag container.
      *
      * \return    A pointer to the Drag container.
      */
      CEGUI::DragContainer * GetDragContainer() const;

      /* GetIconButton
      *
      * Get the icon button.
      *
      * \return    A pointer to the button.
      */
      CEGUI::PushButton * GetIconButton() const;

      /* GetTextButton
      *
      * Get the text button.
      *
      * \return    A pointer to the text button.
      */
      CEGUI::PushButton * GetTextButton() const;

      /* GetName
      *
      * Get the label of this item.
      *
      * \return    A string with the name.
      */
      const std::string GetName() const;

      /* GetBase
      *
      * Get the base string.
      *
      * \return    The base string.
      */
      const std::string GetBase() const;


    protected:

      /* Initialize
      *
      * Initializes this menu item.
      *
      * \return void.
      */
      void Initialize();

      bool OnMouseEnters(const CEGUI::EventArgs &e);
      bool OnMouseLeaves(const CEGUI::EventArgs &e);
      bool OnButtonDown(const CEGUI::EventArgs &e);
      bool OnButtonUp(const CEGUI::EventArgs &e);
    };

  }
}



#endif // PT_GUI_MENUITEM_H
