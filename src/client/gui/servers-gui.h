/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef SERVERS_GUI_H
#define SERVERS_GUI_H

#include "base-gui.h"

class ServerWindow : public GUIWindow
{
private:
  void AddServer(const char* name, const char* host);
  bool OnSelection(const CEGUI::EventArgs& e);

public:
  ServerWindow(GUIManager* guimanager);
  virtual ~ServerWindow();
  void CreateGUIWindow();

  void ShowWindow();
};

#endif // SERVERS_GUI_H
