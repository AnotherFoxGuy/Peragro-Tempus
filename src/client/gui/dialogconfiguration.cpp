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

#include "client/gui/dialogconfiguration.h"

#include <iutil/vfs.h>
#include <iutil/objreg.h>
#include <iutil/string.h>
#include <csutil/scfstr.h>
#include <csutil/xmltiny.h>

#include <CEGUI.h>
#include <ivaria/icegui.h>

#include <algorithm>
#include <iostream>

#include "common/reporter/reporter.h"

namespace PT
{
  namespace GUI
  {
    DialogConfiguration::DialogConfiguration(iObjectRegistry* obj_reg)
      : objReg(obj_reg)
    {
      cegui = csQueryRegistry<iCEGUI>(objReg);
      vfs = csQueryRegistry<iVFS> (obj_reg);
    } // end DialogConfiguration()

    DialogConfiguration::~DialogConfiguration()
    {
    } // end ~DialogConfiguration()

    bool DialogConfiguration::LoadConfiguration(const std::string& file)
    {
      csRef<iFile> buf = vfs->Open(file.c_str(), VFS_FILE_READ);
      csRef<iDocumentSystem> docsys = csQueryRegistry<iDocumentSystem>(objReg);
      CEGUI::WindowManager * winMgr = cegui->GetWindowManagerPtr();

      if (!buf)
      {
        return false;
      }

      csRef<iDocument> doc;
      csRef<iDocumentNode> node;

      if (!docsys)
      {
        docsys = csPtr<iDocumentSystem>(new csTinyDocumentSystem());
      }
      doc = docsys->CreateDocument();
      const char* error = doc->Parse(buf, true);
      if (error != 0)
      {
        return false;
      }

      if (doc)
      {
        node = doc->GetRoot()->GetNode("dialogs");
        if (!node)
        {
          return false;
        }
      }

      // Get all the nodes from the node 'dialogs'.
      csRef<iDocumentNodeIterator> it = node->GetNodes();

      while (it->HasNext())
      {
        // For each dialog
        csRef<iDocumentNode> child = it->Next();

        DialogConfig dialogConfig;
        std::string windowName = child->GetAttributeValue("name");
        CEGUI::Window * window = 0;
        try
        {
          window = winMgr->getWindow(windowName);
        }
        catch (CEGUI::Exception)
        {
          continue;
        }
        dialogConfig.defaultPosition = window->getPosition();
        dialogConfig.defaultSize = window->getSize();
        dialogConfig.windowName = windowName;

        csRef<iDocumentNode> position = child->GetNode("position");
        csRef<iDocumentNode> size = child->GetNode("size");
        csRef<iDocumentNode> visible = child->GetNode("visible");

        if (!position || !size || !visible)
        {
          continue;
        }
        std::string positionVal = position->GetContentsValue();
        float xScale, xOffset, yScale, yOffset;
        xScale = xOffset = yScale = yOffset = 0.0f;

        std::vector<std::string> parts = split(positionVal, ",");

        if (parts.size() != 4)
        {
          continue;
        }
        if (sscanf(parts.at(0).c_str(), "%f", &xScale) == 0 ||
            sscanf(parts.at(1).c_str(), "%f", &xOffset) == 0)
        {
          continue;
        }
        if (sscanf(parts.at(2).c_str(), "%f", &yScale) == 0 ||
            sscanf(parts.at(3).c_str(), "%f", &yOffset) == 0)
        {
          continue;
        }

        dialogConfig.userPosition = CEGUI::UVector2(CEGUI::UDim(xScale, xOffset),
          CEGUI::UDim(yScale, yOffset));

        std::string sizeVal = size->GetContentsValue();
        xScale = xOffset = yScale = yOffset = 0.0f;

        parts = split(sizeVal, ",");

        if (parts.size() != 4)
        {
          continue;
        }
        if (sscanf(parts.at(0).c_str(), "%f", &xScale) == 0 ||
            sscanf(parts.at(1).c_str(), "%f", &xOffset) == 0)
        {
          continue;
        }
        if (sscanf(parts.at(2).c_str(), "%f", &yScale) == 0 ||
            sscanf(parts.at(3).c_str(), "%f", &yOffset) == 0)
        {
          continue;
        }

        dialogConfig.userSize = CEGUI::UVector2(CEGUI::UDim(xScale, xOffset),
          CEGUI::UDim(yScale, yOffset));

        std::string visibleVal = visible->GetContentsValue();
        if (!visibleVal.empty())
        {
          if (visibleVal == "true" || visibleVal == "True")
          {
            dialogConfig.visible = true;
          }
          else
          {
            dialogConfig.visible = false;
          }
        }

        dialogConfigurations[window] = dialogConfig;
      }

      if (!ApplyDialogConfiguration()) return false;
      HideAll();

      return true;
    } // end LoadConfiguration()

    bool DialogConfiguration::SaveConfiguration(const std::string& fileName)
    {
      csRef<iDocumentSystem> docsys = csPtr<iDocumentSystem>(
        new csTinyDocumentSystem());
      if (!docsys)
      {
        return Report(PT::Warning,
          "SaveConfiguration unable to load csTinyDocumentSystem!");
      }

      csRef<iDocument> doc = docsys->CreateDocument();
      csRef<iDocumentNode> root = doc->CreateRoot();
      root->SetValue("configuration");

      csRef<iDocumentNode> dialogs = root->CreateNodeBefore(CS_NODE_ELEMENT);
      dialogs->SetValue("dialogs");

      WindowMap::iterator ppkNode = dialogConfigurations.begin();
      WindowMap::iterator ppkEnd = dialogConfigurations.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        CEGUI::Window * wnd = (*ppkNode).first;
        DialogConfig dlgConfig = (*ppkNode).second;

        if (!wnd->testClassName("FrameWindow") && !wnd->testClassName("Listbox"))
        {
          continue;
        }

        csRef<iDocumentNode> dialogNode =
          dialogs->CreateNodeBefore(CS_NODE_ELEMENT);
        dialogNode->SetValue("dialog");
        dialogNode->SetAttribute("name", wnd->getName().c_str());

        csRef<iDocumentNode> position =
          dialogNode->CreateNodeBefore(CS_NODE_ELEMENT);
        position->SetValue("position");

        csRef<iDocumentNode> positionText =
          position->CreateNodeBefore(CS_NODE_TEXT);

        std::stringstream ss;
        CEGUI::UVector2 pos = wnd->getPosition();
        ss << pos.d_x.d_scale << "," << pos.d_x.d_offset << ","
          << pos.d_y.d_scale << "," << pos.d_y.d_offset;
        positionText->SetValue(ss.str().c_str());

        csRef<iDocumentNode> size =
          dialogNode->CreateNodeBefore(CS_NODE_ELEMENT);
        size->SetValue("size");

        csRef<iDocumentNode> sizeText =
          size->CreateNodeBefore(CS_NODE_TEXT);

        CEGUI::UVector2 sz = wnd->getSize();
        ss.str("");
        ss << sz.d_x.d_scale << "," << sz.d_x.d_offset << ","
          << sz.d_y.d_scale << "," << sz.d_y.d_offset;
        sizeText->SetValue(ss.str().c_str());

        csRef<iDocumentNode> visible =
          dialogNode->CreateNodeBefore(CS_NODE_ELEMENT);
        visible->SetValue("visible");

        csRef<iDocumentNode> visibleText =
          visible->CreateNodeBefore(CS_NODE_TEXT);
        if (wnd->isVisible())
        {
          visibleText->SetValue("true");
        }
        else
        {
          visibleText->SetValue("false");
        }

      }  // end for

      csRef<iString> buf;
      buf.AttachNew(new scfString());
      const char *err = doc->Write(buf);
      if (err)
      {
        return Report(PT::Warning, "Error writing to buffer: %s", err);
      }

      if (!vfs->WriteFile(fileName.c_str(), buf->GetData(), buf->Length()))
      {
        return Report(PT::Warning, "Error writing to file '%s'",
          fileName.c_str());
      }

      return true;
    } // end SaveConfiguration()

    bool DialogConfiguration::ApplyDialogConfiguration()
    {
      if (dialogConfigurations.empty())
      {
        return false;
      }
      WindowMap::iterator ppkNode = dialogConfigurations.begin();
      WindowMap::iterator ppkEnd = dialogConfigurations.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        ApplyDialogConfiguration((*ppkNode).first);
      }
      return true;
    } // end ApplyDialogConfiguration()

    bool DialogConfiguration::ApplyDialogConfiguration(CEGUI::Window * window)
    {
      WindowMap::iterator result = dialogConfigurations.find(window);

      // Check if the window exists.
      if (result == dialogConfigurations.end())
      {
        // The window has NOT been found.
        return false;
      }

      result->first->setPosition(result->second.userPosition);
      result->first->setSize(result->second.userSize);
      result->first->setVisible(result->second.visible);
      return true;
    } // end ApplyDialogConfiguration()

    bool DialogConfiguration::ApplyDialogConfiguration(
      const std::string& windowName)
    {
      try
      {
        return ApplyDialogConfiguration(cegui->GetWindowManagerPtr()->
          getWindow(windowName));
      }
      catch (CEGUI::Exception)
      {
        return false;
      }
    } // end ApplyDialogConfiguration()

    bool DialogConfiguration::AddDialog(CEGUI::Window * window)
    {
      WindowMap::iterator result = dialogConfigurations.find(window);

      // Check if the window has already been added.
      if (result != dialogConfigurations.end())
      {
        return false;
      }

      for (unsigned int i = 0; i != window->getChildCount(); ++i)
      {
        CEGUI::Window * child = window->getChildAtIdx(i);
        // Check if its a FrameWindow or Listbox.
        if (child->testClassName("FrameWindow") ||
          child->testClassName("Listbox"))
        {
          DialogConfig cfg;
          cfg.defaultPosition = child->getPosition();
          cfg.defaultSize = child->getSize();
          cfg.windowName = child->getName().c_str();
          cfg.visible = child->isVisible();
          dialogConfigurations[child] = cfg;
        }
      }

      if (window->testClassName("FrameWindow") ||
        window->testClassName("Listbox"))
      {
        DialogConfig defaults;
        defaults.defaultPosition = window->getPosition();
        defaults.defaultSize = window->getSize();
        defaults.windowName = window->getName().c_str();
        defaults.visible = window->isVisible();
        dialogConfigurations[window] = defaults;
      }

      if (!window->testClassName("FrameWindow") &&
        !window->testClassName("Listbox"))
      {
        Report(PT::Warning,"DialogConfiguration::AddDialog: Unable to add dialog %s",
               window->getName().c_str());
      }
      return true;
    } // end AddDialog()

    bool DialogConfiguration::AddDialog(const std::string& windowName)
    {
      try
      {
        return AddDialog(cegui->GetWindowManagerPtr()->getWindow(windowName));
      }
      catch (CEGUI::Exception)
      {
        // Most probably the window could not been found.
        return false;
      }
    } // end AddDialog()

    bool DialogConfiguration::RemoveDialog(CEGUI::Window * window)
    {
      WindowMap::iterator result = dialogConfigurations.find(window);

      // Check if the window exists.
      if (result == dialogConfigurations.end())
      {
        // The window has been found: erase it.
        dialogConfigurations.erase(result);
        return true;
      }
      return false;
    } // end RemoveDialog()

    bool DialogConfiguration::RemoveDialog(const std::string& windowName)
    {
      try
      {
        return RemoveDialog(cegui->GetWindowManagerPtr()->getWindow(windowName));
      }
      catch (CEGUI::Exception)
      {
        // Most probably the window could not been found.
        return false;
      }
    } // end RemoveDialog()

    bool DialogConfiguration::SetDialogToDefault(CEGUI::Window * window,
                                                 bool position, bool size,
                                                 bool visible)
    {
      WindowMap::iterator result = dialogConfigurations.find(window);

      // Check if the window exists.
      if (result != dialogConfigurations.end())
      {
        // The window has NOT been found.
        return false;
      }
      if (position)
      {
        result->second.userPosition = result->second.defaultPosition;
      }
      if (size)
      {
        result->second.userSize = result->second.defaultSize;
      }
      return true;
    } // end SetDialogToDefault()

    bool DialogConfiguration::SetDialogToDefault(const std::string& windowName,
                                                 bool position, bool size,
                                                 bool visible)
    {
      try
      {
        return SetDialogToDefault(
          cegui->GetWindowManagerPtr()->getWindow(windowName), position, size);
      }
      catch (CEGUI::Exception)
      {
        return false;
      }
    } // end SetDialogToDefault()

    void DialogConfiguration::Reload()
    {
      WindowMap tempMap;

      CEGUI::WindowManager * winMgr = cegui->GetWindowManagerPtr();

      WindowMap::iterator ppkNode = dialogConfigurations.begin();
      WindowMap::iterator ppkEnd = dialogConfigurations.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        try
        {
          DialogConfig cfg = (*ppkNode).second;
          CEGUI::Window * wnd = winMgr->getWindow(cfg.windowName);
          tempMap[wnd] = cfg;
        }
        catch (CEGUI::Exception)
        {
          continue;
        }
      }
      dialogConfigurations.clear();
      dialogConfigurations.insert(tempMap.begin(), tempMap.end());
      //dialogConfigurations = tempMap;

      ApplyDialogConfiguration();
    } // end Reload()

    void DialogConfiguration::SavePositions()
    {
      WindowMap tempMap;

      WindowMap::iterator ppkNode = dialogConfigurations.begin();
      WindowMap::iterator ppkEnd = dialogConfigurations.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        CEGUI::Window * wnd = (*ppkNode).first;
        DialogConfig cfg = (*ppkNode).second;

        cfg.userPosition = wnd->getPosition();
        cfg.userSize = wnd->getSize();

        tempMap[wnd] = cfg;
      }
      dialogConfigurations.clear();
      dialogConfigurations.insert(tempMap.begin(), tempMap.end());
    } // end SavePositions()

    bool DialogConfiguration::IsPresent(CEGUI::Window * window)
    {
      WindowMap::iterator result = dialogConfigurations.find(window);

      // Check if the window exists.
      if (result == dialogConfigurations.end())
      {
        // The window has been found.
        return true;
      }
      return false;
    } // end IsPresent()

    bool DialogConfiguration::IsPresent(const std::string& windowName)
    {
      try
      {
        return IsPresent(cegui->GetWindowManagerPtr()->getWindow(windowName));
      }
      catch (CEGUI::Exception)
      {
        // Most probably the window could not been found.
        return false;
      }
    } // end IsPresent()

    std::vector<std::string> DialogConfiguration::split(std::string str,
                                                        std::string delimiter)
    {
      std::vector<std::string> retvector;

      if (delimiter.length() > 1)
      {
        return retvector;
      }

      size_t start = 0;
      size_t pos;

      do
      {
        pos = str.find_first_of(delimiter, start);

        if (pos == start)
        {
          start = pos + 1;
        }
        else if (pos == str.npos)
        {
          retvector.push_back(str.substr(start));
        }
        else
        {
          retvector.push_back(str.substr(start, pos - start));
          start = pos + 1;
        }
        start = str.find_first_not_of(delimiter, start);
      } while (pos != str.npos);

      return retvector;
    } // end split()

    void DialogConfiguration::HideAll()
    {

      WindowMap::iterator ppkNode = dialogConfigurations.begin();
      WindowMap::iterator ppkEnd = dialogConfigurations.end();

      for (; ppkNode != ppkEnd; ++ppkNode)
      {
        CEGUI::Window * wnd = (*ppkNode).first;
        wnd->hide();
      }
    } // end HideAll()

  } // GUI namespace
} // PT namespace

