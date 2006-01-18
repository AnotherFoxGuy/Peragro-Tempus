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

/*==================//
//     GUIBase      //
//==================*/

GuiBase::GuiBase()
{
}
GuiBase::~GuiBase()
{
}

void GuiBase::CreateGuiWindow(const char* layoutFile)
{
	winMgr = cegui->GetWindowManagerPtr ();

	// Initialize CEGUI wrapper
	cegui->Initialize ();

	// Set the logging level
	cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

	vfs->ChDir ("/client/skin/");

	// Load the ice skin (which uses Falagard skinning system)
	cegui->GetSchemeManagerPtr ()->loadScheme("TaharezLookSkin.scheme");

	cegui->GetSystemPtr ()->setDefaultMouseCursor("TaharezLook", "MouseArrow");
	cegui->GetFontManagerPtr ()->createFont("CommonWealth", "/client/skin/Commonv2c.ttf", 10, 
		CEGUI::Default);

	// Load layout and set as root
	vfs->ChDir ("/client/gui/");
	cegui->GetSystemPtr ()->setGUISheet(winMgr->loadWindowLayout(layoutFile));
}

void GuiBase::SetPointers(iCEGUI* cegui, iVFS* vfs, Network* network)
{
	this->cegui = cegui;
	this->vfs = vfs;
	this->network = network;
}
void GuiBase::HideWindow()  
{
	rootwindow->setVisible(false);
}

void GuiBase::ShowWindow() 
{
	rootwindow->setVisible(true);
}

void GuiBase::DisableWindow() 
{
	rootwindow->setEnabled(false);
}

void GuiBase::EnableWindow() 
{
	rootwindow->setEnabled(true);
}

/*====================//
// GUIConnectWindow   //
//====================*/
ConnectWindow::ConnectWindow()
{
}

ConnectWindow::~ConnectWindow()
{
}

bool ConnectWindow::ConnectButtonPressed(const CEGUI::EventArgs& e) 
{
	// Get the connect window and disable it
	GuiBase::DisableWindow();

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

void ConnectWindow::CreateGuiWindow()
{
	GuiBase::CreateGuiWindow ("connect.xml");

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
LoginWindow::LoginWindow()
{
}

LoginWindow::~LoginWindow()
{
}

bool LoginWindow::LoginButtonPressed(const CEGUI::EventArgs& e) 
{
	// Get the login window and disable it
	GuiBase::DisableWindow();

	printf("Login %s:%s!!\n", GetLogin(), GetPassword());

	LoginRequestMessage answer_msg;
	answer_msg.setName(GetLogin());
	answer_msg.setPwHash(GetPassword());
	network->send(&answer_msg);

	return true;
}

bool LoginWindow::RegisterButtonPressed(const CEGUI::EventArgs& e)
{
	printf("Register %s %s!!\n", GetLogin(), GetPassword());
	printf("Register1 %s %s!!\n", login, GetPassword());

	RegisterRequestMessage answer_msg;
	answer_msg.setName(GetLogin());
	answer_msg.setPwHash(GetPassword());
	network->send(&answer_msg);

	return true; 
}

char* LoginWindow::GetLogin() 
{
	btn = winMgr->getWindow("Name");
	CEGUI::String name = btn->getText();
	login = const_cast<char*>(name.c_str());
	printf("GetLogin %s \n", login);
	return login;
}

char* LoginWindow::GetPassword() 
{
	btn = winMgr->getWindow("Password");
	CEGUI::String pw = btn->getText();
	password = (char*)pw.c_str();
	return password;
}

bool LoginWindow::RememberLogin()  
{
	return true;
}

void LoginWindow::CreateGuiWindow()
{
	GuiBase::CreateGuiWindow ("login.xml");

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
SelectCharWindow::SelectCharWindow()
{
	own_char_id = -1;
}

SelectCharWindow::~SelectCharWindow()
{
}

bool SelectCharWindow::SelectChar(const CEGUI::EventArgs& e) 
{
	GuiBase::DisableWindow();

	btn = winMgr->getWindow("Characters");
	if (((CEGUI::MultiColumnList*)btn)->getSelectedCount() == 0)
		return true;

	CEGUI::ListboxItem* item = ((CEGUI::MultiColumnList*)btn)->getFirstSelectedItem();

	own_char_id = atoi(item->getText().c_str());

	CharacterSelectionRequestMessage answer_msg;
	answer_msg.setCharId(own_char_id);
	network->send(&answer_msg);
	return true;
}

bool SelectCharWindow::NewChar(const CEGUI::EventArgs& e) 
{
	CharacterCreationRequestMessage answer_msg;
	answer_msg.setName(GetNewCharName());
	network->send(&answer_msg);
	return true;
}

bool SelectCharWindow::DelChar(const CEGUI::EventArgs& e) 
{
	return true;
}

char* SelectCharWindow::GetNewCharName()  
{
	btn = winMgr->getWindow("CharNewName");
	CEGUI::String name = btn->getText();
	character = (char*)name.c_str();
	return character;
}


void SelectCharWindow::CreateGuiWindow()
{
	//GuiBase::CreateGuiWindow ("login.xml");

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
int SelectCharWindow::GetOwnChar()
{
	return this->own_char_id;
}