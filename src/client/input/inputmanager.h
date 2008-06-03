/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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
#include "shortcutcombo.h"

#include "common/event/event.h"

struct iClipboard;

namespace PT
{
  class Client;

  class InputManager : public csBaseEventHandler
  {
  private:
    /// Clipboard access provider
    csRef<iClipboard> csTheClipboard;
    bool ClipboardCopy(iEvent& ev);
    bool ClipboardCut(iEvent& ev);
    bool ClipboardPaste(iEvent& ev);
    bool DoCopy(bool cuttext);

    std::map<ShortcutCombo,std::string> functions;

    /**
     * Invoked by the InputManager when a mouse event is received.
     * @param ev Description of a mouse event.
     * @return True if a new action was triggered, false otherwise.
     */
    bool OnMouse(iEvent& ev);

  public:
    InputManager();

    /**
     * Initializes the InputManager. Amongst other things, configuration file is read.
     * @return True if operation succeeded, false otherwise.
     */
    bool Initialize();

    /**
     * Invoked by the InputManager when a keyboard event is received.
     * @param ev Description of a keyboard event.
     * @return True if a new action was triggered, false otherwise.
     */
    bool OnKeyboard(iEvent& ev);
    /**
     * Invoked by the InputManager when a mouse down event is received.
     * This method actually just wraps around the private OnMouse() member.
     * @param ev Description of a mouse event.
     * @return True if a new action was triggered, false otherwise.
     */
    bool OnMouseDown(iEvent& ev);
    /**
     * Invoked by the InputManager when a mouse up event is received.
     * This method actually just wraps around the private OnMouse() member.
     * @param ev Description of a mouse event.
     * @return True if a new action was triggered, false otherwise.
     */
    bool OnMouseUp(iEvent& ev);
  };

}// PT namespace

#endif // INPUTMANAGER_H
