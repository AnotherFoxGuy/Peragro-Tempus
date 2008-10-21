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
/**
 * @file questmanager.h
 *
 * @brief Handles connect/disconnect(login) logic aswell as the character selection.
 */

#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <cssysdef.h>

#include "common/event/event.h"
#include "common/event/eventmanager.h"

#define NO_CHARACTER_SELECTED_0 0

struct iCommandLineParser;

namespace PT
{
  namespace User
  {
    /**
     * Handles connect/disconnect(login) logic aswell as the character selection.
     */
    class UserManager
    {
    private:
      csRef<iCommandLineParser> cmdline;

    private:
      PT_CALLBACK_HANDLER_LISTENERS
      /// Handlers.
      bool Connected(iEvent& ev);
      bool LoginResponse(iEvent& ev);
      bool RegisterResponse(iEvent& ev);
      bool CharacterList(iEvent& ev);
      bool CharacterCreateResponse(iEvent& ev);
      //bool CharacterSelectResponse(iEvent& ev);

    private:
      // needed for relogin on disconnect
      std::string user;
      std::string pass;
      unsigned int char_id;

    public:
      /// Constructor.
      UserManager();
      /// Destructor.
      ~UserManager();

      /**
       * Initialize the usermanager.
       * @return True, indicating success.
       */
      bool Initialize();

      void Login(const std::string& user, const std::string& pass);

      void SelectCharacter(unsigned int char_id);

    };
  } // User namespace
} // PT namespace

#endif // USERMANAGER_H_
