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

/*=================//
//    ChatWindow   //
//=================*/
ChatWindow::ChatWindow (GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ChatWindow::~ChatWindow ()
{
}

void ChatWindow::CreateGUIWindow ()
{
  GUIWindow::CreateGUIWindow ("chat.xml");
  GUIWindow::CreateGUIWindow ("chatlog.xml");
  winMgr = cegui->GetWindowManagerPtr ();

  CreateDropList();

  // Get the root window
  rootwindow = winMgr->getWindow("Chatlog/Frame");

  CEGUI::Slider* slider = static_cast<CEGUI::Slider*>(winMgr->getWindow("Chatlog/Slider"));
  // set up slider config
  slider->setCurrentValue(1.0f);
  slider->setClickStep(0.1f);
  slider->subscribeEvent(CEGUI::Slider::EventValueChanged, CEGUI::Event::Subscriber(&ChatWindow::Onslider, this));

  // set up submitting message on enter
  btn = winMgr->getWindow("InputPanel/InputBox");
  btn->subscribeEvent(CEGUI::Editbox::EventTextAccepted, CEGUI::Event::Subscriber(&ChatWindow::OnSay, this));

  // input box enter button behaviour
  btn = winMgr->getWindow("Root");
  btn->subscribeEvent(CEGUI::Window::EventKeyDown, CEGUI::Event::Subscriber(&ChatWindow::OnRootKeyDown, this));

  btn = winMgr->getWindow("InputPanel/ChatDropList");
  btn->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted, CEGUI::Event::Subscriber(&ChatWindow::OnDropList, this));
}


bool ChatWindow::OnSay (const CEGUI::EventArgs& e)
{
  btn = winMgr->getWindow("InputPanel/InputBox");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  if (text.empty()) 
  {
    winMgr->getWindow("InputPanel/Frame")->setVisible(false);
    winMgr->getWindow("Chatlog/Frame")->activate();
    return true;
  }

  // IF HandleCommand returns true, the text was a command
  // and we can return.
  if ( HandleCommand(text.c_str()) ) 
  {
    btn->setText(text.erase());
    return true;
  }

  printf("Say: %s\n", text.c_str());
  SayMessage msg;
  msg.setMessage(text.c_str());
  network->send(&msg);
  btn->setText(text.erase());
  return true;
}

bool ChatWindow::HandleCommand (const char* texti)
{
  csString text = texti;

  // Check if the first character is "/"
  // then we know its a command.
  if (strncmp (texti,"/",1) == 0)
  {
    // Find the end of the first word, which is our command.
    size_t pos = text.FindFirst(" ");
    csString command;
    csArray<csString> arg;

    // The command is one word.
    if ( pos == (size_t)-1 ) 
      command = text.Slice(1, text.Length());
    // The command is several words.
    else 
    {
      command = text.Slice(1, pos-1);

      csString args = text.Slice(pos+1, text.Length());
      csString tail = args;

      // Push the arguments on an array.
      while (tail.Length() > 0)
      {
        size_t pos = tail.FindFirst(" ");
        if ( pos == (size_t)-1 ) 
        {
          arg.Push( tail.Slice(0, tail.Length()+1) );
          printf("ChatWindow: HandleCommand: Added argument: %s\n", tail.Slice(0, tail.Length()).GetData() );
          tail.Clear();
        }
        else
        {
          arg.Push( tail.Slice(0, pos) );
          printf("ChatWindow: HandleCommand: Added argument: %s\n", tail.Slice(0, pos).GetData() );
          tail = tail.Slice(pos+1, tail.Length());
        }
      }

    }

    printf("ChatWindow: HandleCommand: found %s\n", command.GetData());


    if (command.Compare("relocate"))
    {
      network->send(&RelocateMessage());
    }
    else if (command.Compare("guild"))
    {
      printf("ChatWindow: HandleCommand: handled guild!\n");
    }
    else if (command.Compare("whisper"))
    {
      printf("ChatWindow: HandleCommand: handled whisper!\n");
      if (!arg.GetSize() || arg.GetSize() < 2) return true;
      csString nick = arg.Get(0);
      csString msg;

      for(uint i = 1; i < arg.GetSize(); i++) 
      {
        msg += arg.Get(i);
        msg += " ";
      }
      // Get your own nickname.
      csString ownnick = PointerLibrary::getInstance()->getEntityManager()->GetOwnName();
      // Add your own text to the whisper.
      guimanager->GetWhisperWindow()->AddWhisper(nick.GetData(), msg.GetData(), ownnick.GetData());
      // Send the whisper to the network.
      WhisperToMessage nmsg;
      nmsg.setListenerName(ptString(nick.GetData(), nick.Length())); //<-- name of who you want to talk to...
      nmsg.setMessage(msg.GetData());
      network->send(&nmsg);
    }
    else
      printf("ChatWindow: HandleCommand: Unknown command!\n"); 
    
    return true;
  }

  return false;
}

bool ChatWindow::OnShout (const CEGUI::EventArgs& e)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String text = btn->getText();
  if (text.empty()) return true;
  printf("Shout: %s\n", text.c_str());
  SayMessage msg;
  msg.setMessage(text.c_str());
  network->send(&msg);
  btn->setText(text.erase());
  return true;
}

bool ChatWindow::OnWhisper (const CEGUI::EventArgs& e)
{
  /*
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn;
  btn = winMgr->getWindow("Input");
  if (!btn)
  {
    printf("Inputbox of Chat not found!\n");
    return false;
  }
  CEGUI::String input = btn->getText();
  if (input.empty()) return true;
  printf("Whisper: %s\n", input.c_str());
  WhisperMessage msg;
  size_t split = input.find(": "); //you type 'nick: message'
  CEGUI::String name = input.substr(0, split);
  CEGUI::String text = input.substr(split, input.length() - split);
  msg.setOther(ptString(name.c_str(), name.length())); //<-- name of who you want to talk to...
  msg.setMessage(text.c_str());
  network->send(&msg);
  btn->setText(text.erase());
  */
  return true;
}

bool ChatWindow::OnDropList(const CEGUI::EventArgs& e) 
{
  printf("success \n");
  return true;
}
bool ChatWindow::Onslider(const CEGUI::EventArgs& e)
{
    using namespace CEGUI;

    // we know it's a slider.
    Slider* s = static_cast<Slider*>(static_cast<const WindowEventArgs&>(e).window);

    // get value from slider and set it as the current alpha
    float val = s->getCurrentValue();
    winMgr->getWindow("Chatlog/GlobalButton")->setAlpha(val);
    winMgr->getWindow("Chatlog/PartyButton")->setAlpha(val);
    winMgr->getWindow("Chatlog/GuildButton")->setAlpha(val);
    winMgr->getWindow("Chatlog/CombatButton")->setAlpha(val);
    winMgr->getWindow("Chatlog/ChatlogWidget")->setAlpha(val);

    // indicate the event was handled here.
    return true;
}
bool ChatWindow::OnRootKeyDown(const CEGUI::EventArgs& e)
{
    using namespace CEGUI;

    CEGUI::Window* inventorywin = winMgr->getWindow("Inventory/Frame");
    CEGUI::Window* statuswin = winMgr->getWindow("Status/Frame");

    const KeyEventArgs& keyArgs = static_cast<const KeyEventArgs&>(e);

    //printf("OnRootKeyDown: key %d pressed \n", keyArgs.scancode);

    switch (keyArgs.scancode)
    {
    case Key::Return:
        winMgr->getWindow("InputPanel/Frame")->setVisible(true);
        winMgr->getWindow("InputPanel/InputBox")->activate();
        break;

    case Key::F11:
      statuswin->isVisible(true) ? statuswin->hide() : statuswin->show();
      break;

    case Key::F12:
      inventorywin->isVisible(true) ? inventorywin->hide() : inventorywin->show();
      break;

    default: return false;
    }

    return true;
}
void ChatWindow::CreateDropList()
{

  btn = winMgr->getWindow("InputPanel/ChatDropList");
  CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Say /s", 0);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);
  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Shout /h", 1);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);
  charIdItem = new CEGUI::ListboxTextItem((CEGUI::utf8*)"Whisper /w", 1);
  ((CEGUI::Combobox*)btn)->addItem(charIdItem);
  ((CEGUI::Combobox*)btn)->setReadOnly(true);

  ((CEGUI::Combobox*)btn)->setItemSelectState(charIdItem, true);

}
void ChatWindow::AddChatMessage (const char* nick, const char* msg)
{
  CEGUI::MultiLineEditbox* chatlog = static_cast<CEGUI::MultiLineEditbox*>(winMgr->getWindow("Chatlog/Chatlog"));
  CEGUI::String nickstr = (CEGUI::String)(nick);
  CEGUI::String new_text = (CEGUI::String)(msg);
  if (!new_text.empty())
  {
    nickstr = "<"+nickstr+">";
    // append newline to this entry
    new_text += '\n';
    // append new text to history output
    chatlog->setText(chatlog->getText() + nickstr + new_text);
    // scroll to bottom of history output
    chatlog->setCaratIndex((size_t)-1);
  }
}

void ChatWindow::AddMessage (const char* msg)
{
  CEGUI::MultiLineEditbox* chatlog = static_cast<CEGUI::MultiLineEditbox*>(winMgr->getWindow("Chatlog/Chatlog"));
  CEGUI::String new_text = (CEGUI::String)(msg);
  if (!new_text.empty())
  {
    // append newline to this entry
    new_text += '\n';
    // append new text to history output
    chatlog->setText(chatlog->getText() + new_text);
    // scroll to bottom of history output
    chatlog->setCaratIndex((size_t)-1);
  }
}
