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

#ifndef PT_GUI_DIALOGCONFIGURATION_H
#define PT_GUI_DIALOGCONFIGURATION_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/array.h>

#include <CEGUI.h>

#ifdef WIN32
#pragma warning( once : 4482 )
#endif

struct iObjectRegistry;
struct iVFS;
struct iCEGUI;

namespace PT
{
  namespace GUI
  {
    class DialogConfiguration
    {
    private:
      csRef<iObjectRegistry> objReg;
      csRef<iCEGUI> cegui;
      csRef<iVFS> vfs;


      struct DialogConfig
      {
        CEGUI::UVector2 defaultPosition;
        CEGUI::UVector2 defaultSize;
        CEGUI::UVector2 userPosition;
        CEGUI::UVector2 userSize;
        bool visible;
        std::string windowName;
      };

      typedef std::map<CEGUI::Window*, DialogConfig> WindowMap;
      WindowMap dialogConfigurations;

    public:
      DialogConfiguration(iObjectRegistry* objReg);
      ~DialogConfiguration();

      /* LoadConfiguration
      *
      * Loads the configuration.
      *
      * @param   file  The XML file to load the configuration from.
      * \return  True if successful, false if not.
      */
      bool LoadConfiguration(const std::string& file = "/this/dialogconfiguration.xml");

      /* SaveConfiguration
      *
      * Saves the current configuration to a specified XML file.
      *
      * @param   fileName  The XML to write the configuration to.
      * \return  True if successful, false if not.
      */
      bool SaveConfiguration(const std::string& fileName = "/this/dialogconfiguration.xml");

      /* ApplyDialogConfiguration
      *
      * Manually update the dialog configuration. Also used internally. You don't have to
      *  call this.
      *
      * \return  True if successful, false if not.
      */
      bool ApplyDialogConfiguration();

      /* ApplyDialogConfiguration
      *
      * Manually update a dialog's configuration. Used internally, you don't have to call
      *  this.
      *
      * @param   window  The window to apply it's configuration.
      * \return True if successful, false if not.
      */
      bool ApplyDialogConfiguration(CEGUI::Window * window);

      /* ApplyDialogConfiguration
      *
      * Manually update a dialog's configuration. Used internally, you don't have to call
      *  this.
      *
      * @param   windowName  The window to apply it's configuration.
      * \return True if successful, false if not.
      */
      bool ApplyDialogConfiguration(const std::string& windowName);

      /* AddDialog
      *
      * Adds a dialog to the system, which position and size are to
      *  be saved.
      *
      * @param   window  The window to add.
      * \return  True if adding was successful, false if not.
      */
      bool AddDialog(CEGUI::Window * window);

      /* AddDialog
      *
      * Adds a dialog to the system, which position and size are to
      *  be saved.
      *
      * @param   windowName  The window's name to add.
      * \return  True if successful, false if not.
      */
      bool AddDialog(const std::string& windowName);

      /* RemoveDialog
      *
      * Removes a dialog from the system, therefore making sure this window's
      *  attributes are not saved.
      *
      * @param   window  The window to remove.
      * \return  True if successful, false if not.
      */
      bool RemoveDialog(CEGUI::Window * window);

      /* RemoveDialog
      *
      * Removes a dialog from the system, therefore making sure this window's
      *  attributes are not saved.
      *
      * @param   windowName  The window's to remove.
      * \return  True if successful, false if not.
      */
      bool RemoveDialog(const std::string& windowName);

      /* SetDialogToDefault
      *
      * Sets the dialog back to it's default settings.
      *
      * @param   window    The window to set back.
      * @param   position  True if the position needs to be set to default.
      * @param   size      True if the size needs to be set to default.
      * @param   visible   True if the visibility needs to be set to default.
      * \return  True if successful, false if not.
      */
      bool SetDialogToDefault(CEGUI::Window * window, bool position = true, bool size = true, bool visible = true);

      /* SetDialogToDefault
      *
      * Sets the dialog back to it's default settings.
      *
      * @param     windowName    The name of the window.
      * @param   position        True if the position needs to be set to default.
      * @param   size                True if the size needs to be set to default.
      * @param   visible        True if the visibility needs to be set to default.
      * \return  True if successful, false if not.
      */
      bool SetDialogToDefault(const std::string& windowName, bool position = true, bool size = true, bool visible = true);

      /* Reload
      *
      * Reloads the system, creating new pointers to the dialogs.
      *
      * \return void.
      */
      void Reload();

      /* SavePositions
      *
      * Updates all the elements with new information.
      *
      * \return void.
      */
      void SavePositions();

      /* IsPresent
      *
      * Checks whether given window is present within the system or not.
      *
      * @param   window  The window to check.
      * \return  True if so, false if not.
      */
      bool IsPresent(CEGUI::Window * window);

      /* IsPresent
      *
      * Checks whether given window is present within the system or not.
      *
      * @param    windowName  The window's name to check.
      * \return True if so, false if not.
      */
      bool IsPresent(const std::string& windowName);

    private:

      std::vector<std::string> split(std::string str, std::string delimiter);


    };

  } // GUI namespace
} // PT namespace

#endif // PT_GUI_DIALOGCONFIGURATION_H
