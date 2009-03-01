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
  : head(0)
{
}

InteractionQueue::~InteractionQueue()
{
  ptScopedMutex lock(mutex);
  if (!head)
  {
    return;
  }

  QueueItem* temp = 0;

  while (true)
  {
    temp = head;
    head = head->next;

    if (temp)
    {
      delete temp->interaction;
      delete temp;
    }
    else
    {
      break;
    }
  }
}

Interaction* InteractionQueue::GetInteraction()
{
  ptScopedMutex lock(mutex);
  if (!head)
  {
    return 0;
  }

  Interaction* interaction = head->interaction;
  // Compare time of interaction with current time
  if (interaction->time > time(0))
  {
    return 0;
  }

  if (head->next)
  {
    head->next->prev = 0;
  }

  QueueItem* temp = head;
  head = head->next;
  delete temp;

  return interaction;
}

void InteractionQueue::RemoveAllInteractions(Character *lockedCharacter)
{
  ptScopedMutex lock(mutex);

  for (QueueItem* queue = head; queue; queue = queue->next)
  {
    if (queue->interaction->character->GetId() == lockedCharacter->GetId())
    {
      RemoveInteractionEntry(queue);
    }
  }

}

void InteractionQueue::RemoveInteractionEntry(QueueItem* queue)
{
  ptScopedMutex lock(mutex);
  if (queue == head)
  {
    head = queue->next;
  }
  else
  {
    if (queue->prev)
    {
      queue->prev->next = queue->next;
    }
    if (queue->next)
    {
      queue->next->prev = queue->prev;
    }
  }
  delete queue;
}

void InteractionQueue::SetInteraction(Interaction* interaction)
{
  // TODO One character cannot have more than X entries.
  // TODO change code elsewhere to make sure old interactions are
  // removed when char dies, or when new target is selected.
  QueueItem* queueItem = new QueueItem();

  queueItem->interaction = interaction;
  queueItem->prev = queueItem->next = 0;
  interaction->time = time(0);

  ptScopedMutex lock(mutex);

  // TODO If the same player already has interactions enqueued time
  // need to include those as well.

  QueueItem* queue = 0;
  for (queue = head; queue && queue->next; queue = queue->next)
  {
    if (queue->interaction->time > interaction->time)
    {
      break;
    }
  }
  if (!queue)
  {
    head = queueItem;
    return;
  }

  QueueItem* temp = queue->prev;
  queue->prev = queueItem;
  queueItem->next = queue;
  if (!temp)
  {
    head = queueItem;
  }
  else
  {
    temp->next = queueItem;
    queueItem->prev = temp;
  }
}

InteractionQueue::QueueItem::QueueItem()
  : prev(0), next(0), interaction(0)
{
  // This queue item doesn't have any neighbours yet.
}

InteractionQueue::QueueItem::~QueueItem()
{
}
