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

#define SERVERSETUPWINDOW "CharSelect/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class ServerSetupWindow : public GUIWindow
      {
      private:
        /// Returns to the character menu.
        bool DoneButton(const CEGUI::EventArgs& e);
        /// Uploads the server data to the server.
        bool UploadButton(const CEGUI::EventArgs& e);
        /// Downloads the server data from the server.
        bool DownloadButton(const CEGUI::EventArgs& e);

      public:
        ServerSetupWindow(GUIManager* guimanager);
        virtual ~ServerSetupWindow();
        bool Create();
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // SERVERSETUP_GUI_H
