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

#ifndef NPC_DIALOG_MANAGER_H
#define NPC_DIALOG_MANAGER_H

class NPCDialog;

class NPCDialogManager
{
private:
  Array<NPCDialog*> dialogs;
  static NPCDialogManager* self;

public:
  NPCDialogManager() { self = this; }
  ~NPCDialogManager();

  static NPCDialogManager& getDialogManager() { return *self; }

  /**
   * Get a dialog from an NPCs ID and the dialog's ID (unique for each NPC)
   * \param npc_id The ID of the NPC.
   * \param dialog_id The dialog's ID.
   * \return A pointer to the corresponding NPCDialog.
   */
  NPCDialog* getDialog(unsigned int npc_id, unsigned int dialog_id);

  void load();

  /// Clear out all dialogs
  void delAll();
};

#endif // NPC_DIALOG_MANAGER_H
