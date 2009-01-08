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

#include "interactionqueue.h"

InteractionQueue::InteractionQueue()
{
  head = NULL;
}

InteractionQueue::~InteractionQueue()
{
  mutex.lock();
  QueueItem* temp = NULL;

  while (1) {
    temp = head;
    head = head->next;

    if (temp) {
      free(temp->interaction);
      free(temp);
    } else {
      break;
    }
  }
  mutex.unlock();
}

Interaction* InteractionQueue::GetInteraction()
{
  QueueItem* temp = NULL;
  Interaction* interaction = NULL;

  mutex.lock();
  if (!head) {
    mutex.unlock();
    return NULL;
  }

  interaction = head->interaction;
  // Compare time of interaction with current time
  if (interaction->time > time(0)) {
    mutex.unlock();
    return NULL;
  }

  if (head->next) {
    head->next->prev = NULL;
  }

  temp = head;
  head = head->next;
  free(temp);

  mutex.unlock();
  return NULL;
}

void InteractionQueue::RemoveAllInteractions(Character *lockedCharacter)
{
  QueueItem* queue = NULL;
  mutex.lock();

  for (queue = head; queue; queue = queue->next) {
    if (queue->interaction->character->getId() == lockedCharacter->getId()) {
      RemoveInteractionEntry(queue);
    }
  }
  mutex.unlock();

}

void InteractionQueue::RemoveInteractionEntry(QueueItem* queue)
{
  mutex.lock();
  if (queue == head) {
    head = queue->next;
  } else {
    if (queue->prev) {
      queue->prev->next = queue->next;
    }
    if (queue->next) {
      queue->next->prev = queue->prev;
    }
  }
  free(queue);
  mutex.unlock();
}

void InteractionQueue::SetInteraction(Interaction* interaction)
{
  // TODO One character cannot have more than X entries.
  // TODO change code elsewhere to make sure old itneractions are
  // removed when char dies, or when new target is selected.
  QueueItem* queue = NULL;
  QueueItem* temp = NULL;
  QueueItem* queueItem = new QueueItem();

  queueItem->interaction = interaction;
  mutex.lock();
  queueItem->prev = queueItem->next = NULL;

  // TODO If the same player already has interactions enqueued time
  // need to include those as well.
  interaction->time = time(0);

  for (queue = head; queue && queue->next; queue = queue->next) {
    if (queue->interaction->time > interaction->time) {
      break;
    }
  }
  if (!queue) {
    head = queueItem;
    mutex.unlock();
    return;
  }

  temp = queue->prev; 
  queue->prev = queueItem;
  queueItem->next = queue;
  if (!temp) {
    head = queueItem;
  } else {
    temp->next = queueItem;
    queueItem->prev = temp;
  }
  mutex.unlock();
}

InteractionQueue::QueueItem::QueueItem()
{
  // This queue item doesn't have any neighbours yet.
  prev = next = NULL;
}

InteractionQueue::QueueItem::~QueueItem()
{
}