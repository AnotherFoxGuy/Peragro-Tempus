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
 * @brief Handles npc dialogs and such.
 */

#ifndef QUESTMANAGER_H_
#define QUESTMANAGER_H_

#include <cssysdef.h>

#include "common/event/event.h"
#include "common/event/eventmanager.h"

namespace PT
{
  namespace Quest
  {
    /**
     * Handles npc dialogs and such.
     */
    class QuestManager
    {
    private:
      CALLBACK_HANDLER_LISTENERS
      /// Handler .
      bool NPCDialog(iEvent& ev);
      bool EndNPCDialog(iEvent& ev);
      
    public:
      /// Constructor.
      QuestManager();
      /// Destructor.
      ~QuestManager();

      /**
       * Initialize the questmanager.
       * @return True, indicating success.
       */
      bool Initialize();

    };
  } // Quest namespace
} // PT namespace

#endif // QUESTMANAGER_H_
