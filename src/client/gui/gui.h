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

#ifndef GUI_H
#define GUI_H


#include "cssysdef.h"

#include "iutil/vfs.h"

#include "CEGUI.h"
#include "ivaria/icegui.h"

#include "common/network/netmessage.h"
#include "common/util/mutex.h"

class Network;

/*================*
|| GUI base class ||
*================*/
class GuiBase
{
private:

protected:
	CEGUI::Window* rootwindow;
	CEGUI::Window* btn;
	CEGUI::WindowManager* winMgr;
	csRef<iCEGUI> cegui;
	csRef<iVFS> vfs;
	Network* network;

public:
	GuiBase();
	~GuiBase();
	void CreateGuiWindow(const char* layoutFile);
	void SetPointers(iCEGUI* cegui, iVFS* vfs, Network* network);
	void HideWindow();
	void ShowWindow();
	void EnableWindow();
	void DisableWindow();
};

/*====================//
// GUIConnectWindow   //
//====================*/
class ConnectWindow : public GuiBase
{
private:
	bool ConnectButtonPressed(const CEGUI::EventArgs& e);   // Executed when the Connect button is pressed.
	csString GetServer();                                   // Return the selected server.

public:
	ConnectWindow();
	virtual ~ConnectWindow();
	void CreateGuiWindow();    // load the Login guilayout and register button events.	  
};

/*====================//
// GUILoginWindow     //
//====================*/
class LoginWindow : public GuiBase
{
private:
	bool LoginButtonPressed(const CEGUI::EventArgs& e);     // Executed when the Connect button is pressed.
	bool RegisterButtonPressed(const CEGUI::EventArgs& e);  // Executed when the Register button is pressed.
	char* GetLogin();                                       // Return the login input.
	char* GetPassword();                                    // Return the password input.
	bool RememberLogin();                                   // Return "Remember login" checkbox value.
	char* login;
	char* password;

public:
	LoginWindow();
	virtual ~LoginWindow();
	void CreateGuiWindow();    // load the Login guilayout and register button events.
};

/*======================//
// GUISelectCharWindow  //
//======================*/
class SelectCharWindow : public GuiBase
{
private:
	bool SelectChar(const CEGUI::EventArgs& e);  // Selects the current character.
	bool NewChar(const CEGUI::EventArgs& e);     // Makes a new character.
	bool DelChar(const CEGUI::EventArgs& e);     // Deletes the selected character.
	char* GetNewCharName();                      // Returns the character name.
	void ScrollLeft();                           // Scroll left throught the characterlist.
	void ScrollRight();                          // Scroll right throught the characterlist.
	int own_char_id;
	char* character;

public:
	SelectCharWindow();
	virtual ~SelectCharWindow();
	void CreateGuiWindow();    // load the Login guilayout and register button events.
	void AddCharacter(unsigned int charId, const char* name);
	int GetOwnChar();
};


#endif // GUI_H