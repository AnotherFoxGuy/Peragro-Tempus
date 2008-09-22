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

#include "skinmanager.h"
#include "gui.h"
#include "guimanager.h"
#include "common/event/eventmanager.h"
#include "common/reporter/reporter.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <iutil/stringarray.h>
#include <iutil/cfgmgr.h>

#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include <ivaria/icegui.h>

#include "client/gui/options/skin-gui.h"

namespace PT
{
  namespace GUI
  {
    SkinManager::SkinManager(GUIManager* guiManager)
      : guiManager(guiManager)
    {
      iObjectRegistry* obj_reg =
        PointerLibrary::getInstance()->getObjectRegistry();
      vfs = csQueryRegistry<iVFS>(obj_reg);
      cegui = guiManager->GetCEGUI ();
      app_cfg = csQueryRegistry<iConfigManager>(obj_reg);
      std::string skinpath = "/peragro/art/skins/";
      skinpath += app_cfg->GetStr("Peragro.Skin");
      skinpath += "/";

      defaultSkin.name = "Peragro";
      defaultSkin.path = skinpath;
    } // end SkinManager()

    SkinManager::~SkinManager()
    {
    } // end ~SkinManager()

    bool SkinManager::LoadPressed(iEvent &ev)
    {
      using namespace CEGUI;

      Windows::SkinOptionsWindow* skinWindow =
        guiManager->GetWindow<Windows::SkinOptionsWindow>("SkinOptionsWindow");
      if (!skinWindow) return false;

      Skin* skin = skinWindow->GetSelectedSkin();
      if (!skin) return false;

      if (skin->name == currentSkin.name && skin->path == currentSkin.path)
      {
        Report(PT::Notify, "User attempted to load skin already in use.");
        return false;
      }
      else
      {
        ChangeSkin(skin->GetName());
      }

      return true;
    } // end LoadPressed()

    Skin SkinManager::FindSkin(const char* name)
    {
      for (size_t i = 0; i < skinList.GetSize();i++)
      {
        Skin skin = skinList.Get(i);
        if (strcmp(skin.GetName(), name) == 0)
          return skin;
      } // for

      Report(PT::Warning, "Couldn't find skin: '%s'", name);
      return defaultSkin;
    } // end FindSkin()

    void SkinManager::SearchForSkins(const char* path)
    {
      Report(PT::Notify,"Looking for skins:");

      // Clear the skinlist and add the default.
      skinList.DeleteAll();
      skinList.Push(defaultSkin);
      Report(PT::Notify, "Found '%s' at '%s'", defaultSkin.GetName(),
        defaultSkin.GetPath());

      // Search for directories in the path.
      vfs->ChDir (path);
      csRef<iStringArray> files = vfs->FindFiles(path);
      for (size_t i = 0; i < files->GetSize();i++)
      {
        std::string file(files->Get(i));
        std::string spath(path);
        std::string name(file.substr(spath.length(), file.length()));
        name = name.substr(0, name.length()-1);
        // Check if the name doesn't start with '.', a hidden file.
        if (name.substr(0,1).compare(".") == 0) continue;
        // Check if the name isn't default.
        if (name.compare("default") == 0) continue;

        // Add 'Peragro_' to the name.
        std::string skinName = "Peragro_";
        skinName += name;
        Report(PT::Notify, "Found '%s' at '%s'", skinName.c_str(),
          file.c_str());
        Skin skin;
        skin.name = skinName;
        skin.path = file;
        skinList.Push(skin);
      } // for

      Report(PT::Debug, "");
    } // end SearchForSkins()

    bool SkinManager::LoadDefaultSkin()
    {
      vfs->ChDir(defaultSkin.GetPath());

      // Load the default skin.
      cegui->GetSchemeManagerPtr()->loadScheme("widgets/peragro.scheme");
      cegui->GetSchemeManagerPtr()->loadScheme("widgets/alias.scheme");

      // Set the default mouse cursor.
      cegui->GetSystemPtr()->
        setDefaultMouseCursor(defaultSkin.GetName(), "MouseArrow");

      // Load the default font.
      CEGUI::Font* font = cegui->GetFontManagerPtr()->
        createFont("FreeType", "Vera", "/peragro/art/skins/default/font/vera.ttf");
      font->setProperty("PointSize", "10");
      font->load();

      CEGUI::Font* font2 = cegui->GetFontManagerPtr ()->
        createFont("FreeType", "Commonwealth-8", "/peragro/art/skins/default/font/commonv2c.ttf");
      font2->setProperty("PointSize", "8");
      font2->load();

      CEGUI::Font* font3 = cegui->GetFontManagerPtr ()->
        createFont("FreeType", "Commonwealth-10", "/peragro/art/skins/default/font/commonv2c.ttf");
      font3->setProperty("PointSize", "10");
      font3->load();

      CEGUI::Font* font4 = cegui->GetFontManagerPtr ()->
        createFont("FreeType", "CommonWealth", "/peragro/art/skins/default/font/commonv2c.ttf");
      font4->setProperty("PointSize", "10");
      font4->load();

      CEGUI::Font* font5 = cegui->GetFontManagerPtr ()->
      createFont("FreeType", "Tahoma-12", "/peragro/art/skins/default/font/commonv2c.ttf");
      font5->setProperty("PointSize", "12");
      font5->load();

      currentSkin = defaultSkin;
      Report(PT::Notify, "Current skin is: '%s' at '%s'",
        currentSkin.GetName(), currentSkin.GetPath());

      return true;
    } // end LoadDefaultSkin()

    bool SkinManager::Initialize()
    {
      // Get some pointers.
      schMgr = cegui->GetSchemeManagerPtr();
      winMgr = cegui->GetWindowManagerPtr();
      system = cegui->GetSystemPtr();

      if (!LoadDefaultSkin())
        return false;

      SearchForSkins("/peragro/art/skins/");

      // Register listener for interface.
      SETUP_HANDLER
      REGISTER_LISTENER(SkinManager, LoadPressed, "interface.skinwindow.buttons.load")

      return true;
    } // end Initialize()

    CEGUI::Window* SkinManager::loadLayout(const char* layoutFile)
    {
      std::string layoutPath ("/peragro/art/skins/default/layouts/");
      vfs->ChDir (layoutPath.c_str());

      CEGUI::Window* win = 0;
      try
      {
        win = winMgr->loadWindowLayout(layoutFile);
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed setting layout: %s!",
            e.getMessage().c_str());
      }
      return win;
    } // end loadLayout()

    bool SkinManager::ChangeSkin(const char* skinname)
    {
      // Switch to the new skin.
      Skin newSkin = FindSkin (skinname);

      Report(PT::Notify, "Switching to new skin: '%s' at '%s'",
        newSkin.GetName(), newSkin.GetPath());

      // Load new scheme.
      try
      {
        vfs->ChDir (newSkin.GetPath());

        if (!schMgr->isSchemePresent(newSkin.GetName()))
          schMgr->loadScheme(/*newSkin.GetPrefix(),*/"widgets/peragro.scheme");
        schMgr->loadScheme(/*newSkin.GetPrefix(),*/"widgets/alias.scheme");
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed switching skin: %s", e.getMessage().c_str());
        return false;
      }

      // Set mousepointer.
      try
      {
        system->setDefaultMouseCursor(/*newSkin.GetPrefix()+*/newSkin.name, "MouseArrow");
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed switching Mouse cursor: %s",
          e.getMessage().c_str());
        system->setDefaultMouseCursor(currentSkin.name, "MouseArrow");
        return false;
      }

      // Finally change the look.
      try
      {
        CEGUI::WindowManager::WindowIterator windowIt = CEGUI::WindowManager::getSingleton().getIterator();
        while (!windowIt.isAtEnd())
        {
          SetWidgetLook(windowIt.getCurrentValue(), /*newSkin.GetPrefix()+*/newSkin.name);
        }
      }
      catch ( CEGUI::Exception& e )
      {
        Report(PT::Error, "Failed switching skin: %s", e.getMessage().c_str());
        return false;
      }

      // If current skin is different from default: unload current skin.
      if (strcmp(defaultSkin.GetName(), currentSkin.GetName()) != 0)
      {
        if (schMgr->isSchemePresent(currentSkin.GetName()))
        {
          Report(PT::Notify, "Unloading scheme file: '%s'",
            currentSkin.GetName());
          schMgr->getScheme(currentSkin.GetName())->unloadResources();
          schMgr->unloadScheme(currentSkin.GetName());
        }
      }

      currentSkin = newSkin;

      return true;
    } // end ChangeSkin()

    void SkinManager::SetWidgetLook(CEGUI::Window* currentWindow, const std::string& _newLook)
    {
      CEGUI::String newLook = _newLook.c_str();

      CEGUI::WindowFactoryManager& wfMgr = CEGUI::WindowFactoryManager::getSingleton();
      const CEGUI::String separator("/");

      // Obtain current window type (in format look/widget)
      const CEGUI::String currentType = currentWindow->getType();

      // Retrieve the widget look and the widget kind
      const CEGUI::String::size_type pos = currentType.find(separator);
      CEGUI::String look(currentType, 0, pos);
      CEGUI::String widget(currentType, pos + 1);

      // Build the new desired type (in format look/widget)
      const CEGUI::String newType = newLook + separator + widget;

      // If no type change is detected, leave
      if(newType == currentType)
        return;

      if (!wfMgr.isFalagardMappedType(newType))
        return;

      // Obtain falagard mapping for type
      const CEGUI::WindowFactoryManager::FalagardWindowMapping& fwm = wfMgr.getFalagardMappingForType(newType);

      // Retrieve and store the currently assigned widget properties that are editable.
      SaveWindowProperties(currentWindow);

      // Assign the new Look'N'Feel to the widget.
      currentWindow->setFalagardType(fwm.d_lookName, fwm.d_rendererType);

      // Time to restore the original editable properties.
      RestoreWindowProperties(currentWindow);

    } // end SetWidgetLook()

    SkinManager::PropertyType SkinManager::GetPropertyType(const CEGUI::String& propertyName)
    {
      const std::string supportedProperties("Alpha,float;ClickStepSize,float;MaximumValue,float; \
                                       Visible,bool;AlwaysOnTop,bool;ClippedByParent,bool; \
                                       InheritsAlpha,bool;Selected,bool;ReadOnly,bool; \
                                       CloseButtonEnabled,bool;DragMovingEnabled,bool; \
                                       FrameEnabled,bool;SizingEnabled,bool;TitlebarEnabled,bool; \
                                       MultiSelect,bool;Sort,bool;DraggingEnabled,bool; \
                                       BackgroundEnabled,bool;InheritsTooltipText,bool; \
                                       HoverImage,text;PushedImage,text;DisabledImage,text; \
                                       NormalImage,text;Font,font;TitlebarFont,font; \
                                       VerticalAlignment,vert_align;HorizontalAlignment,horz_align; \
                                       VertFormatting,vert_text_format;HorzFormatting,horz_text_format; \
                                       Tooltip,text;Image,text;TextColours,text;");
      const std::string::size_type pos = supportedProperties.find(propertyName.c_str());
      if ( pos != std::string::npos )
      {
        const std::string::size_type scPos = supportedProperties.find(";", pos+1);
        const std::string value = supportedProperties.substr(pos+1, scPos);

        if (value == "float")
          return FLOAT_TYPE;
        else if (value == "bool")
          return BOOL_TYPE;
        else if (value == "font")
          return FONT_TYPE;
        else if (value == "text")
          return TEXT_TYPE;
        else if (value == "vert_align")
          return VERT_ALIGN_TYPE;
        else if (value == "horz_align")
          return HORZ_ALIGN_TYPE;
        else if (value == "vert_text_format")
          return VERT_TEXT_FORMAT_TYPE;
        else if (value == "horz_text_format")
          return HORZ_TEXT_FORMAT_TYPE;
      }

      // An upsupported property was given
      return UNSUPPORTED;
    } // end GetPropertyType()

    void SkinManager::SaveWindowProperties(CEGUI::Window* currentWindow)
    {
      originalProps.clear();
      CEGUI::PropertySet::Iterator oit = currentWindow->getPropertyIterator();
      for(; !oit.isAtEnd() ; ++oit)
      {
        // Obtain current property name
        const CEGUI::String propertyName = (*oit)->getName();

        // Check if this property is editable. If so, we want to store it for later reference
        if(this->GetPropertyType(propertyName) != UNSUPPORTED)
          (void)originalProps.insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));

        // We also want to keep some properties, that while not directly editable by the user,
        // are affected by the new Look'N'Feel.
        else if (propertyName == "TextColours" || propertyName == "Image" ||
        propertyName == "MouseCursorImage" || propertyName == "EWSizingCursorImage" ||
        propertyName == "NSSizingCursorImage" || propertyName == "NESWSizingCursorImage" ||
        propertyName == "NWSESizingCursorImage" || propertyName == "ImageColours" ||
        propertyName == "WantsMultiClickEvents")
          (void)originalProps.insert(std::make_pair(propertyName, currentWindow->getProperty(propertyName)));
      }
    } // end SaveWindowProperties()

    void SkinManager::RestoreWindowProperties(CEGUI::Window* currentWindow)
    {
      // Time to restore the original editable properties
      for(PropMap::iterator it = originalProps.begin(); it != originalProps.end(); it++)
      {
        // Obtain current property name
        const CEGUI::String propertyName = (*it).first;

        // Although the Look'N'Feel and WindowRender properties are not
        // currently editable, we want to REALLY make sure they are not processed here (safety-check)
        if(propertyName != "LookNFeel" && propertyName != "WindowRenderer")
        {
          // Make sure this property is still valid with the new Look'N'Feel
          if(currentWindow->isPropertyPresent(propertyName))
          {
            // Restore property
            currentWindow->setProperty(propertyName, (*it).second);

            // If the property wasn't correctly set (i.e. because the value
            // is not valid), we want to remove it to prevent garbage
            // NOTE: This is mostly for the MouseCursorImage property, whose
            // set() implementation (see CEGUIWindowProperties.cpp) prevents
            // empty entries.
            if(currentWindow->getProperty(propertyName) != (*it).second)
            {
              if (propertyName == "Image" ||
                propertyName == "MouseCursorImage" || propertyName == "EWSizingCursorImage" ||
                propertyName == "NSSizingCursorImage" || propertyName == "NESWSizingCursorImage" ||
                propertyName == "NWSESizingCursorImage")
                currentWindow->removeProperty(propertyName);
            } // if
          } // if
        } // if
      } // for

    } // end RestoreWindowProperties()

    bool SkinManager::Populate()
    {
      Windows::SkinOptionsWindow* skinWindow =
        guiManager->GetWindow<Windows::SkinOptionsWindow>("SkinOptionsWindow");
      if (!skinWindow) return false;

      // Add the skins to the GUI.
      for (size_t i = 0; i < skinList.GetSize();i++)
      {
        Skin skin = skinList.Get(i);
        skinWindow->AddSkin(skin);
      } // for

      return true;
    } // end Populate()

    Skin SkinManager::GetCurrentSkin()
    {
      return currentSkin;
    } // end GetCurrentSkin()

  } // GUI namespace
} // PT namespace

