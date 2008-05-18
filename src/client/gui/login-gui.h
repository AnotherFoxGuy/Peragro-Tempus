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

#ifndef LOGIN_GUI_H
#define LOGIN_GUI_H

#include "base-gui.h"

#include "csutil/ref.h"

#include "iutil/cfgmgr.h"
#include "iutil/cfgfile.h"

#define LOGINWINDOW "LoginUI/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
    class LoginWindow : public GUIWindow
    {
    private:

      csRef<iConfigManager> app_cfg;

      bool LoginButtonPressed(const CEGUI::EventArgs& e);     // Executed when the Connect button is pressed.
      bool RegisterButtonPressed(const CEGUI::EventArgs& e);  // Executed when the Register button is pressed.
      bool LoginTextAccepted(const CEGUI::EventArgs &e);      // Executed when the Login's text has been accepted.
      bool PasswordTextAccepted(const CEGUI::EventArgs &e);   // Executed when the Registers text has been accepted.
      CEGUI::String GetLogin();                               // Return the login input.
      CEGUI::String GetPassword();                            // Return the password input.
      bool OnCheckBox(const CEGUI::EventArgs& e);             // Return "Remember login" checkbox.
      void CreateCheckBox();
      void SaveConfig();

    public:
      void UpdateLogin();
      LoginWindow(GUIManager* guimanager);
      virtual ~LoginWindow();
      bool Create();
      bool ReloadWindow();

      void ShowWindow();
    };
    }
  }
}
#endif // LOGIN_GUI_H
