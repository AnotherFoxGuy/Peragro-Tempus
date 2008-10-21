/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
 * @file inputmanager.h
 *
 * @basic Handles keyboard and mouse input.
 */

#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <cssysdef.h>
#include <csutil/csbaseeventh.h>
#include <csutil/event.h>
#include <csutil/cfgmgr.h>
#include <csutil/cfgfile.h>
#include <csutil/cfgacc.h>

#include <string>
#include <map>
#include <boost/scoped_ptr.hpp>

#include "controlcombo.h"

#include "common/event/event.h"
#include "common/event/eventmanager.h"

struct iClipboard;

namespace PT
{
  class Client;

  namespace Input
  {
    /**
     * Handles keyboard and mouse input.
     */
    class InputManager : public csBaseEventHandler
    {
    private:
      /// Clipboard access provider.
      csRef<iClipboard> csTheClipboard;
      typedef std::map<const ControlCombo, std::string> ControlMap;
      /// Map of controls to actions.
      ControlMap controls;

      struct ControlChangeData
      {
        ControlChangeData(const ControlMap::iterator& itr,
          const std::string& action)
          : itr(itr), action(action) {}

        const ControlMap::iterator itr;
        const std::string action;
      };
      boost::scoped_ptr<ControlChangeData> changeControl;
        /*
      /// Whether a control is being changed, overriding normal input.
      bool changingControl;
      /// Iterator pointing to the control to be changed.
      ControlMap::iterator changeControl;
      /// Action to be mapped, needed when binding a new control.
      std::string changeAction;
*/

      PT_CALLBACK_HANDLER_LISTENERS
      /// Handler for the clipboard copy event. Calls DoCopy(false).
      bool ClipboardCopy(iEvent& ev);
      /// Handler for the clipboard cut event. Calls DoCopy(true).
      bool ClipboardCut(iEvent& ev);
      /// Handler for the clipboard paste event.
      bool ClipboardPaste(iEvent& ev);
      /**
       * Handler for cut and paste events.
       * @param cuttext Whether this is a cut or paste event.
       * @return True.
       */
      bool DoCopy(bool cuttext);

      /// Handler for the control change request event.
      bool ChangeControl(iEvent& ev);

      /**
       * Parse the configuration file for shortcuts.
       * @return True for success.
       */
      bool LoadControls();

      /**
       * PT handler for all input events.
       * @param ev The event.
       * @param keyboard Whether it was a keyboard or mouse event.
       * @return True if the event was handled.
       */
      bool HandleControlEvents(const iEvent &ev, const bool keyboard);

    public:
      /// Constructor.
      InputManager();

      /**
       * Initializes the InputManager. Amongst other things, configuration file
       * is read.
       * @return True if initialization succeeded.
       */
      bool Initialize();

      /**
       * Invoked by the InputManager when a keyboard event is received.
       * This method calls the private HandleControlEvents() member.
       * @return True if the event was handled.
       * @return true if a new action was triggered, false otherwise.
       */
      bool OnKeyboard(iEvent& ev);

      /**
       * Invoked by the InputManager when a mouse down event is received.
       * This method calls the private HandleControlEvents() member.
       * @return True if the event was handled.
       * @return true if a new action was triggered, false otherwise.
       */
      bool OnMouseDown(iEvent& ev);

      /**
       * Invoked by the InputManager when a mouse up event is received.
       * This method calls the private HandleControlEvents() member.
       * @return True if the event was handled.
       */
      bool OnMouseUp(iEvent& ev);

    };

  } // Input namespace
} // PT namespace

#endif // INPUTMANAGER_H
