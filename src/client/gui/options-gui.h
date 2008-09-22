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

#ifndef OPTIONS_GUI_H
#define OPTIONS_GUI_H

#include "base-gui.h"

#include "csutil/ref.h"

#include "iutil/cfgmgr.h"
#include "iutil/cfgfile.h"

#define OPTIONSWINDOW "Options/Options_Button"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class OptionsWindow : public GUIWindow
      {
      private:
        struct Option
        {
          std::string name;
          std::string window;

          const char* GetName () { return name.c_str();}
          const char* GetWindow () { return window.c_str(); }
        };

        csArray<Option> optionList;
        Option currentOption;
        Option FindOption (const char* name);
        bool ChangeOption (Option option);
        bool CreateOptionItem (const char* optionName);

      private:
        bool OptionPressed (const CEGUI::EventArgs& e);
        bool OptionButtonPressed(const CEGUI::EventArgs& e);
        bool OnCloseButton(const CEGUI::EventArgs& e);

      public:
        OptionsWindow(GUIManager* guimanager);
        virtual ~OptionsWindow();
        bool Create();

        /// Add an option item.
        bool AddOption (const char* optionName, const char* windowName);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // OPTIONS_GUI_H
