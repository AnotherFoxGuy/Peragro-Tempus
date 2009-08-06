/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include "usermanager.h"

#include <csutil/cmdline.h>

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/gui/guimanager.h"
#include "client/gui/login-gui.h"
#include "client/gui/servers-gui.h"
#include "client/gui/charsel-gui.h"
#include "client/gui/confirmdialog-gui.h"

#include "client/entity/entitymanager.h"
#include "client/network/network.h"

#include "common/event/eventmanager.h"
#include "common/event/event.h"

#include "client/state/statemanager.h"

namespace PT
{
  namespace User
  {
    UserManager::UserManager() : char_id(NO_CHARACTER_SELECTED_0)
    {
    }

    UserManager::~UserManager()
    {
    }

    bool UserManager::Initialize()
    {
      PT_SETUP_HANDLER
      PT_REGISTER_LISTENER(UserManager, Connected, "state.connected")
      PT_REGISTER_LISTENER(UserManager, LoginResponse, "user.login")
      PT_REGISTER_LISTENER(UserManager, LogoutResponse, "user.logout")
      PT_REGISTER_LISTENER(UserManager, RegisterResponse, "user.register")

      PT_REGISTER_LISTENER(UserManager, CharacterList, "user.character.list")
      PT_REGISTER_LISTENER(UserManager, CharacterCreateResponse, "user.character.create")

      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      cmdline = csQueryRegistry<iCommandLineParser> (object_reg);

      return true;
    } // end Initialize()

    void UserManager::SelectCharacter(unsigned int char_id)
    {
      this->char_id = char_id;
      CharSelectRequestMessage answer_msg;
      answer_msg.setCharId(char_id);
      PointerLibrary::getInstance()->getNetwork()->send(&answer_msg);
    } // end SelectCharacter()

    void UserManager::Login(const std::string& user, const std::string& pass)
    {
      this->user = user;
      this->pass = pass;

      LoginRequestMessage answer_msg;
      answer_msg.setUsername(ptString(user.c_str(), strlen(user.c_str())));
      answer_msg.setPassword(pass.c_str());
      PointerLibrary::getInstance()->getNetwork()->send(&answer_msg);
    } // end login()

    bool UserManager::Connected (iEvent& ev)
    {
      using namespace PT::Events;

      StateManager* stateManager = PointerLibrary::getInstance()->getStateManager();
      if (!stateManager) return false;

      if (stateManager->GetState() == STATE_RECONNECTED)
      {
        Login(user, pass);
      }
      else
      {
        if (cmdline && stateManager->GetState() == STATE_INTRO)
        {
          const char* user = cmdline->GetOption("user", 0);
          const char* pass = cmdline->GetOption("pass", 0);

          if (user && pass)
          {
            if (cmdline->GetBoolOption("register", false))
            {
              RegisterRequestMessage reg_msg;
              reg_msg.setUsername(ptString(user, strlen(user)));
              reg_msg.setPassword(pass);
              PointerLibrary::getInstance()->getNetwork()->send(&reg_msg);
            }
            Login(user, pass);
          }
        }

        stateManager->SetState(STATE_CONNECTED);
      }

      Report(PT::Notify, "Connected!");
      return true;
    }

    bool UserManager::LoginResponse(iEvent& ev)
    {
      using namespace Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return false;

      StateManager* stateManager = PointerLibrary::getInstance()->getStateManager();
      if (!stateManager) return false;

      LoginWindow* loginWindow = guimanager->GetWindow<LoginWindow>(LOGINWINDOW);
      ServerWindow* serverWindow = guimanager->GetWindow<ServerWindow>(SERVERWINDOW);
      SelectCharWindow* selectCharWindow = guimanager->GetWindow<SelectCharWindow>(SELECTCHARWINDOW);

      if (Helper::HasError(&ev))
      {
        Report(PT::Error, "Login Failed due to: %s.", Helper::GetError(&ev).c_str());

        loginWindow->EnableWindow();
        serverWindow->EnableWindow();
        //network->stop();
        //stateManager->SetState(STATE_INTRO);
        OkDialogWindow* dialog = new OkDialogWindow(guimanager);
        dialog->SetText(Helper::GetError(&ev).c_str());
        return true;
      }
      else
        Report(PT::Notify, "Login succeeded!");


      if (stateManager->GetState() == STATE_RECONNECTED && char_id != NO_CHARACTER_SELECTED_0)
      {
        SelectCharacter(char_id);
      }
      else if (stateManager->GetState() == STATE_CONNECTED)
      {
        loginWindow->HideWindow();
        serverWindow->HideWindow();
        selectCharWindow->ShowWindow();

        bool isAdmin = false;
        ev.Retrieve("isAdmin", isAdmin);
        if (isAdmin){selectCharWindow->ShowAdminButton();}

        stateManager->SetState(STATE_LOGGED_IN);

        if (cmdline)
        {
          const char* character = cmdline->GetOption("char");
          if (character)
          {
            SelectCharacter(atoi(character));
          }
        }
      }

      return true;
    } // end LoginResponse()

    bool UserManager::LogoutResponse(iEvent& ev)
    {
      Logout();
      return true;
    }

    bool UserManager::RegisterResponse(iEvent& ev)
    {
      using namespace Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return false;

      OkDialogWindow* dialog = new OkDialogWindow(guimanager);

      if (Helper::HasError(&ev))
      {
        Report(PT::Warning, "Registration Failed due to: %s", Helper::GetError(&ev).c_str());
        dialog->SetText(Helper::GetError(&ev).c_str());
      }
      else
      {
        dialog->SetText("Registration succeeded");
        Report(PT::Notify, "Registration succeeded!");
      }

      return true;
    } // end RegisterResponse()

    bool UserManager::CharacterList(iEvent& ev)
    {
      using namespace PT::Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return false;

      SelectCharWindow* selectCharWindow = guimanager->GetWindow<SelectCharWindow>(SELECTCHARWINDOW);
      selectCharWindow->EmptyCharList();

      csRef<iEvent> list;
      if (ev.Retrieve("characterList", list) == csEventErrNone)
      {
        csRef<iEventAttributeIterator> characters = list->GetAttributeIterator();
        while (characters->HasNext())
        {
          csRef<iEvent> character; list->Retrieve(characters->Next(), character);
          unsigned int charId = Helper::GetUInt(character, "charId");
          std::string name = Helper::GetString(character, "name");
          unsigned char skinColour = Helper::GetUChar(character, "skinColour");
          unsigned char hairColour = Helper::GetUChar(character, "hairColour");
          unsigned char decalColour = Helper::GetUChar(character, "decalColour");

          selectCharWindow->AddCharacter(charId, name.c_str(), skinColour, hairColour, decalColour);

        } // end while
      }
      else
        Report(PT::Error, "UserManager failed to get character list!");

      return true;
    } // end CharacterList()

    bool UserManager::CharacterCreateResponse(iEvent& ev)
    {
      using namespace Events;
      using namespace PT::GUI;
      using namespace PT::GUI::Windows;

      GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      if (!guimanager) return false;

      if (!Helper::HasError(&ev))
      {
        SelectCharWindow* selectCharWindow = guimanager->GetWindow<SelectCharWindow>(SELECTCHARWINDOW);
        unsigned int charId = Helper::GetUInt(&ev, "charId");
        std::string name = Helper::GetString(&ev, "name");
        unsigned char skinColour = Helper::GetUChar(&ev, "skinColour");
        unsigned char hairColour = Helper::GetUChar(&ev, "hairColour");
        unsigned char decalColour = Helper::GetUChar(&ev, "decalColour");

        selectCharWindow->AddCharacter(charId, name.c_str(), skinColour, hairColour, decalColour);
      }
      else
      {
        Report(PT::Warning, "Character creation failed due to: %s", Helper::GetError(&ev).c_str());
        OkDialogWindow* dialog = new OkDialogWindow(guimanager);
        dialog->SetText(Helper::GetError(&ev).c_str());
      }

      return true;
    } // end CharacterCreateResponse()

    void UserManager::Logout()
    {
      PointerLibrary::getInstance()->getStateManager()->SetState(STATE_INTRO);
      GUI::GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
      guimanager->GetWindow<GUI::GUIWindow>("Background")->ShowWindow();
      guimanager->GetWindow<GUI::GUIWindow>("Chatlog/Frame")->HideWindow();
      guimanager->GetWindow<GUI::GUIWindow>("PlayerHUD/Frame")->HideWindow();
      guimanager->GetWindow<GUI::GUIWindow>("LoginUI/Frame")->ShowWindow();
      guimanager->GetWindow<GUI::GUIWindow>("LoginUI/Frame")->EnableWindow();
      guimanager->GetWindow<GUI::GUIWindow>("ServerWindow")->ShowWindow();
      guimanager->GetWindow<GUI::GUIWindow>("ServerWindow")->EnableWindow();
      guimanager->GetWindow<GUI::Windows::SelectCharWindow>("CharSelect/Frame")->HideAdminButton();

      PointerLibrary::getInstance()->getNetwork()->stop();
      PointerLibrary::getInstance()->getEntityManager()->Reset();
    } // end Logout()

  } // User namespace
} // PT namespace
