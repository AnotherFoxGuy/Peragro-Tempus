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

class GUIManager;

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
	  
	  bool CreateOptionItem (const char* optionName);

	private:

	  csRef<iConfigManager> app_cfg;
	  csRef<iVFS> vfs;
      csArray<Option> optionList;

	  void SaveConfig();

	  bool OptionButtonPressed(const CEGUI::EventArgs& e);

	  bool OkButtonPressed(const CEGUI::EventArgs& e);

	  bool OnDropListReflections(const CEGUI::EventArgs& e);
	  void CreateDropListReflections();

	  bool OnDropListTexture(const CEGUI::EventArgs& e);
	  void CreateDropListTexture();

	  bool OnDropListMovement(const CEGUI::EventArgs& e);
	  void CreateDropListMovement();

	  void CreateFullScreenCheckBox();
	  bool OnFullScreenCheckBox(const CEGUI::EventArgs& e);

	  void CreateReverseCheckBox();
	  bool OnReverseCheckBox(const CEGUI::EventArgs& e);

	  void CreateYAxisCheckBox();
	  bool OnYAxisCheckBox(const CEGUI::EventArgs& e);

	  void CreateAdaptiveSpinners();
	  bool OnMinFPSSpinnerChanged(const CEGUI::EventArgs &e);
	  bool OnMaxFPSSpinnerChanged(const CEGUI::EventArgs &e);
	  bool OnMinDistanceSpinnerChanged(const CEGUI::EventArgs &e);

	public:
	  OptionsWindow(GUIManager* guimanager);
	  virtual ~OptionsWindow();
      bool Create();
	  bool ReloadWindow();
	  void HideWindow();

      /// Add an option item.
      bool AddOption (const char* optionName, const char* windowName);
	};
	}
  }
}


#endif // OPTIONS_GUI_H
