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

#ifndef SERVERSETUP_GUI_H
#define SERVERSETUP_GUI_H

#include "base-gui.h"

class ServerSetupWindow : public GUIWindow
{
private:
  bool DoneButton(const CEGUI::EventArgs& e);    // returns to the character menu.
  bool UploadButton(const CEGUI::EventArgs& e);  // Uploads the server data to the server.
  bool DownloadButton(const CEGUI::EventArgs& e);  // Downloads the server data from the server.

public:
  ServerSetupWindow(GUIManager* guimanager);
  virtual ~ServerSetupWindow();
  void CreateGUIWindow();    // load the Server setup guilayout and register button events.
};

#endif // SERVERSETUP_GUI_H
