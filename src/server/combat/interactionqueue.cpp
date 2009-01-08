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
}

InteractionQueue::~InteractionQueue()
{
}

Interaction* InteractionQueue::GetInteraction()
{
  // TODO
  QueueItem* temp = NULL;
  Interaction* interaction = NULL;

  if (!head) {
    return NULL;
  }

  interaction = head->interaction;

  if (head->next) {
    head->next->prev = NULL;
  }
  temp = head;
  head = head->next;
  free(temp);

  return NULL;
}

void InteractionQueue::SetInteraction(Interaction* interaction)
{

}

InteractionQueue::QueueItem::QueueItem()
{
  prev = next = NULL;
}

InteractionQueue::QueueItem::~QueueItem()
{
}
