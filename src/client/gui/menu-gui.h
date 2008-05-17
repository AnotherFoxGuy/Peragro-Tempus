/*
  Copyright (C) 2005 - 2007 Development Team of Peragro Tempus

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General public License for more details.

  You should have received a copy of the GNU General public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PT_GUI_MENUWINDOW_H
#define PT_GUI_MENUWINDOW_H

#include "base-gui.h"

#define MENUWINDOW "MenuWindow"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {

    class MenuWindow : public GUIWindow
    {
    private:
      bool MenuButtonPressed (const CEGUI::EventArgs& e);
      bool MouseLeaves (const CEGUI::EventArgs& e);

    public:
      MenuWindow (GUIManager* guiManager);
      virtual ~MenuWindow ();
      bool Create ();
      bool ReloadWindow ();
    };
    }
  }
} // PT namespace

#endif // PT_GUI_MENUWINDOW_H
