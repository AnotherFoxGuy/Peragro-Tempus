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

/*==================//
//     GUIBase      //
//==================*/

GUIWindow::GUIWindow(GUIManager* guimanager)
: guimanager(guimanager)
{
    cegui = guimanager->GetCEGUI ();
    vfs = guimanager->GetClient ()->getVFS ();
    network = guimanager->GetClient ()->getNetwork ();
}
GUIWindow::~GUIWindow()
{
}

void GUIWindow::CreateGUIWindow(const char* layoutFile)
{
    cegui->GetSystemPtr ()->setGUISheet(LoadLayout (layoutFile));
}

CEGUI::Window* GUIWindow::LoadLayout(const char* layoutFile)
{
    winMgr = cegui->GetWindowManagerPtr ();

    // Load layout and set as root
    vfs->ChDir ("/client/gui/");
    return winMgr->loadWindowLayout(layoutFile);
}

void GUIWindow::HideWindow()  
{
    rootwindow->setVisible(false);
}

void GUIWindow::ShowWindow() 
{
    rootwindow->setVisible(true);
}

void GUIWindow::DisableWindow() 
{
    rootwindow->setEnabled(false);
}

void GUIWindow::EnableWindow() 
{
    rootwindow->setEnabled(true);
}

/*====================//
// GUIConnectWindow   //
//====================*/
ConnectWindow::ConnectWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

ConnectWindow::~ConnectWindow()
{
}

bool ConnectWindow::ConnectButtonPressed(const CEGUI::EventArgs& e) 
{
    // Get the connect window and disable it
    GUIWindow::DisableWindow();

    ConnectRequestMessage msg;
    SocketAddress addr = Socket::getSocketAddress(GetServer(), 12345);
    network->setServerAddress(addr);
    network->send(&msg);

    return true;
}

csString ConnectWindow::GetServer()  
{
    btn = winMgr->getWindow("Server");
    CEGUI::String str = btn->getText();
    csString serverip = str.c_str();
    return serverip;
}

void ConnectWindow::CreateGUIWindow()
{
    GUIWindow::CreateGUIWindow ("connect.xml");

    winMgr = cegui->GetWindowManagerPtr ();

    // Get the root window
    rootwindow = winMgr->getWindow("Connect");

    // Register the button events.
    btn = winMgr->getWindow("Connect_Button");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ConnectWindow::ConnectButtonPressed, this));

}

/*====================//
//   GUILoginWindow   //
//====================*/
LoginWindow::LoginWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

LoginWindow::~LoginWindow()
{
}

bool LoginWindow::LoginButtonPressed(const CEGUI::EventArgs& e) 
{
    // Get the login window and disable it
    GUIWindow::DisableWindow();

    LoginRequestMessage answer_msg;
    CEGUI::String login = GetLogin();
    CEGUI::String password = GetPassword();
    answer_msg.setName(login.c_str());
    answer_msg.setPwHash(password.c_str());
    network->send(&answer_msg);

    return true;
}

bool LoginWindow::RegisterButtonPressed(const CEGUI::EventArgs& e)
{
    RegisterRequestMessage answer_msg;
    CEGUI::String login = GetLogin();
    CEGUI::String password = GetPassword();
    answer_msg.setName(login.c_str());
    answer_msg.setPwHash(password.c_str());
    network->send(&answer_msg);

    return true; 
}

CEGUI::String LoginWindow::GetLogin() 
{
    return winMgr->getWindow("Name")->getText();
}

CEGUI::String LoginWindow::GetPassword() 
{
    return winMgr->getWindow("Password")->getText();
}

bool LoginWindow::RememberLogin()  
{
    return true;
}

void LoginWindow::CreateGUIWindow()
{
    GUIWindow::CreateGUIWindow ("login.xml");

    winMgr = cegui->GetWindowManagerPtr ();

    // Get the root window
    rootwindow = winMgr->getWindow("LoginUI");

    // Register the button events.
    btn = winMgr->getWindow("Login_Button");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoginWindow::LoginButtonPressed, this));

    btn = winMgr->getWindow("Register_Button");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&LoginWindow::RegisterButtonPressed, this));

}

/*====================//
// SelectCharWindow   //
//====================*/
SelectCharWindow::SelectCharWindow(GUIManager* guimanager)
: GUIWindow (guimanager)
{
}

SelectCharWindow::~SelectCharWindow()
{
}

bool SelectCharWindow::SelectChar(const CEGUI::EventArgs& e) 
{
    GUIWindow::DisableWindow();

    btn = winMgr->getWindow("Characters");
    if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
        return true;

    CEGUI::ListboxItem* item = ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

    int own_char_id = atoi(item->getText().c_str());

    CharacterSelectionRequestMessage answer_msg;
    answer_msg.setCharId(own_char_id);
    network->send(&answer_msg);
    return true;
}

bool SelectCharWindow::NewChar(const CEGUI::EventArgs& e) 
{
    CEGUI::String NewCharName = GetNewCharName();
    CharacterCreationRequestMessage answer_msg;
    answer_msg.setName(NewCharName.c_str());
    network->send(&answer_msg);
    return true;
}

bool SelectCharWindow::DelChar(const CEGUI::EventArgs& e) 
{
    return true;
}

CEGUI::String SelectCharWindow::GetNewCharName()  
{
    return winMgr->getWindow("CharNewName")->getText();
}


void SelectCharWindow::CreateGUIWindow()
{
    GUIWindow::CreateGUIWindow ("charselect.xml");

    winMgr = cegui->GetWindowManagerPtr ();

    // Get the root window
    rootwindow = winMgr->getWindow("CharSelectUI");

    // Register the button events.
    btn = winMgr->getWindow("CharSelOk");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::SelectChar, this));

    btn = winMgr->getWindow("CharSelNew");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&SelectCharWindow::NewChar, this));

    btn = winMgr->getWindow("Characters");
    CEGUI::String str_id("Id");
    CEGUI::String str_name("Name");
    ((CEGUI::MultiColumnList*)btn)->addColumn(str_id,0,0.1f);
    ((CEGUI::MultiColumnList*)btn)->addColumn(str_name,1,0.5f);
    ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);

}

void SelectCharWindow::AddCharacter(unsigned int charId, const char* name)
{
    btn = winMgr->getWindow("Characters");
    char charIdstr[10];
    sprintf(charIdstr, "%d", charId);
    CEGUI::ListboxItem* charIdItem = new CEGUI::ListboxTextItem(charIdstr);
    CEGUI::ListboxItem* charNameItem = new CEGUI::ListboxTextItem(name);

    charIdItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");
    charNameItem->setSelectionBrushImage((CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"TextSelectionBrush");

    unsigned int row = ((CEGUI::MultiColumnList*)btn)->addRow();
    ((CEGUI::MultiColumnList*)btn)->setItem(charIdItem, 0, row);
    ((CEGUI::MultiColumnList*)btn)->setItem(charNameItem, 1, row);
    ((CEGUI::MultiColumnList*)btn)->setSelectionMode(CEGUI::MultiColumnList::RowSingle);
}


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

    winMgr = cegui->GetWindowManagerPtr ();

    // Get the root window
    rootwindow = winMgr->getWindow("Chat");

    btn = winMgr->getWindow("Say");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnSay, this));
    btn = winMgr->getWindow("Shout");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnShout, this));
    btn = winMgr->getWindow("Whisper");
    btn->subscribeEvent(CEGUI::PushButton::EventClicked, CEGUI::Event::Subscriber(&ChatWindow::OnWhisper, this));
}


bool ChatWindow::OnSay (const CEGUI::EventArgs& e)
{
    btn = winMgr->getWindow("Input");
    if (!btn)
    {
        printf("Inputbox of Chat not found!\n");
        return false;
    }
    CEGUI::String text = btn->getText();
    printf("Say: %s\n", text.c_str());
    ChatMessage msg;
    msg.setType(0);
    msg.setMessage(text.c_str());
    network->send(&msg);
    return true;
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
    printf("Shout: %s\n", text.c_str());
    ChatMessage msg;
    msg.setType(1);
    msg.setMessage(text.c_str());
    network->send(&msg);
    return true;
}

bool ChatWindow::OnWhisper (const CEGUI::EventArgs& e)
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
    printf("!!TDB!! Whisper: %s\n", text.c_str());
    return true;
}

void ChatWindow::AddChatMessage (csRef<iString> msg)
{
    CEGUI::Listbox* dialog = 
        static_cast<CEGUI::Listbox*> (winMgr->getWindow("MessageDialog"));

    //add text to list
    CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(msg->GetData());
    dialog->addItem ( item );
    dialog->ensureItemIsVisible(dialog->getItemCount());
}
