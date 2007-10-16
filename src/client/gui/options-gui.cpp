/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "client/gui/gui.h"

#include "CEGUI.h"
#include "CEGUIWindowManager.h" 
#include "CEGUILogger.h"

#include "client/network/network.h"
#include "client/gui/guimanager.h"

#include "client/reporter/reporter.h"

OptionsWindow::OptionsWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

OptionsWindow::~OptionsWindow ()
{
}

void OptionsWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("options.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  // Get the root window
  rootwindow = winMgr->getWindow("Options/Options_Button");

  // Hide the Option menu.
  btn = winMgr->getWindow("Options/Frame");
  btn->setVisible(false);

  app_cfg =  csQueryRegistry<iConfigManager> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
  if (!app_cfg) 
  {
    Report(PT::Error, "Can't find the config manager!"); 
    return;
  }

  vfs =  csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getClient()->GetObjectRegistry());
  if (!vfs) 
  {
    Report(PT::Error, "Can't find the vfs!"); 
    return;
  }

  // Set up the button behaviour for Reflections.
  CreateDropListReflections();
  btn = winMgr->getWindow("Options/Reflections/DropList");
  btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::OnDropListReflections, this));

  // Set up the button behaviour for Texture quality.
  CreateDropListTexture();
  btn = winMgr->getWindow("Options/TextureQuality/DropList");
  btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&OptionsWindow::OnDropListTexture, this));

  // Set up the OK button.
  btn = winMgr->getWindow("Options/OK_Button");
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsWindow::OkButtonPressed, this));

  // Set up the Options button.
  btn = winMgr->getWindow("Options/Options_Button");
  winMgr->getWindow("Root")->addChildWindow(btn);
  btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&OptionsWindow::OptionButtonPressed, this));

  // Set up the Fullscreen checkbox.
  CreateCheckBox();
  btn = winMgr->getWindow("Options/Fullscreen");
  btn->subscribeEvent(CEGUI::Checkbox::EventCheckStateChanged, CEGUI::Event::Subscriber(&OptionsWindow::OnCheckBox, this));
}

void OptionsWindow::SaveConfig()
{
  app_cfg->Save();
}

void OptionsWindow::HideWindow()
{
  // Hide the Options button.
  btn = winMgr->getWindow("Options/Options_Button");
  btn->setVisible(false);

  // Hide the Option menu.
  btn = winMgr->getWindow("Options/Frame");
  btn->setVisible(false);
}

bool OptionsWindow::OptionButtonPressed(const CEGUI::EventArgs& e) 
{
  // Hide the Options button.
  btn = winMgr->getWindow("Options/Options_Button");
  btn->setVisible(false);

  // Show the Option menu.
  btn = winMgr->getWindow("Options/Frame");
  btn->setVisible(true);
  return true;
}

bool OptionsWindow::OkButtonPressed(const CEGUI::EventArgs& e) 
{
  // Hide the Options button.
  btn = winMgr->getWindow("Options/Options_Button");
  btn->setVisible(true);

  // Show the Option menu.
  btn = winMgr->getWindow("Options/Frame");
  btn->setVisible(false);
  return true;
}

bool OptionsWindow::OnDropListReflections(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("Options/Reflections/DropList");
  uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

  bool ref = false;

  switch(id)
  {
  case 0: // Off
    ref = false;
    break;
  case 1: // On
    ref = true;
    break;

  default: Report(PT::Error, "OnDropListReflections: failed %d", id);
  }

  app_cfg->SetBool("Client.waterreflections", ref);
  SaveConfig();
  return true;
}

void OptionsWindow::CreateDropListReflections()
{

  btn = winMgr->getWindow("Options/Reflections/DropList");

  bool ref = app_cfg->GetBool("Client.waterreflections");

  if (ref)
    ((CEGUI::Combobox*)btn)->setText("On"); 
  else 
    ((CEGUI::Combobox*)btn)->setText("Off");


  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Off", 0);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"On", 1);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  ((CEGUI::Combobox*)btn)->setReadOnly(true);

}

bool OptionsWindow::OnDropListTexture(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("Options/TextureQuality/DropList");
  uint id = ((CEGUI::Combobox*)btn)->getSelectedItem()->getID();

  int quality = 0;

  switch(id)
  {
  case 0: // High
    quality = 0;
    break;
  case 1: // Normal
    quality = 1;
    break;
  case 2: // Low
    quality = 2;
    break;
  case 3: // Very low
    quality = 3;
    break;

  default: Report(PT::Error, "OnDropListTexture: failed %d", id);
  }

  app_cfg->SetInt("Video.OpenGL.TextureDownsample", quality);
  SaveConfig();
  return true;
}

void OptionsWindow::CreateDropListTexture()
{

  btn = winMgr->getWindow("Options/TextureQuality/DropList");

  int quality = app_cfg->GetInt("Video.OpenGL.TextureDownsample");

  if (quality == 0)
    ((CEGUI::Combobox*)btn)->setText("High"); 
  else if (quality == 1)
    ((CEGUI::Combobox*)btn)->setText("Normal");
  else if (quality == 2)
    ((CEGUI::Combobox*)btn)->setText("Low");
  else if (quality == 3)
    ((CEGUI::Combobox*)btn)->setText("Very Low");
  else
    ((CEGUI::Combobox*)btn)->setText("Custom");


  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"High", 0);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Normal", 1);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Low", 2);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Very Low", 3);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);

  ((CEGUI::Combobox*)btn)->setReadOnly(true);

}

bool OptionsWindow::OnCheckBox(const CEGUI::EventArgs& e) 
{
  btn = winMgr->getWindow("Options/Fullscreen");
  bool fs = ((CEGUI::Checkbox*)btn)->isSelected();

  app_cfg->SetBool("Video.FullScreen", fs);
  SaveConfig();
  return true;
}

void OptionsWindow::CreateCheckBox()
{
  btn = winMgr->getWindow("Options/Fullscreen");

  bool fs = app_cfg->GetBool("Video.FullScreen");

  ((CEGUI::Checkbox*)btn)->setSelected(fs);
}
