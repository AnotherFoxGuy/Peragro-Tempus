
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
 * @file interaction.h
 *
 * @basic Server interaction queue for holding interactions.
 */

#ifndef INTERACTIONQUEUE
#define INTERACTIONQUEUE

#include "common/util/thread.h"
#include "interaction.h"

#include "src/server/entity/pcentity.h"

/**
 * Server interaction queue
 */
class InteractionQueue
{
private:
  // TODO might have to lock this queue
  /**
   * Server queue item, class for holding next/prev interaction pointers.
   */
  class QueueItem {
  public:
    /// Constructor
    QueueItem();
    /// Destructor.
    ~QueueItem();
    /// Pointer to the previous entry in the queue.
    QueueItem* prev;
    /// Pointer to the next entry in the queue.
    QueueItem* next;
    /// Pointer to the interaction.
    Interaction* interaction;
  };
public:
  /// Constructor.
  InteractionQueue();
  /// Destructor.
  ~InteractionQueue();
  /**
   * Used to check if there is any interaction request that has to be served.
   * @return Interaction if there is one to handle, else NULL.
   */
  Interaction* GetInteraction();
  /**
   * Used to insert a new interaction into the queue.
   * @param interaction The interaction to insert into the queue.
   * @return None.
   */
  void SetInteraction(Interaction* interaction);
  /**
   * Used to remove all interactions for a character.
   * @param lockedCharacter The character for whom to remove interactiosn.
   * @return None.
   */
  void RemoveAllInteractions(Character* lockedCharacter);

  /**
   * Removes an entry from the queue.
   * @param queue The entry to remove.
   * @return None.
   */
  void RemoveInteractionEntry(QueueItem* queue);

private:

  QueueItem* head;
};

#endif //INTERACTIONQUEUE
