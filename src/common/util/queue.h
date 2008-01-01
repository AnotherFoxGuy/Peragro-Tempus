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

#ifndef QUEUE_H
#define QUEUE_H

#include "mutex.h"

class iLinkable
{
  friend class Queue;
private:
  iLinkable* next;
public:
  iLinkable() : next(0) {}
};

class Queue
{
private:

  iLinkable* first;
  iLinkable* last;

  int count;

  Mutex mutex;

public:
  Queue() : first(0), last(0), count(0) {}
  ~Queue() {}

  void push(iLinkable* element)
  {
    mutex.lock();
    if (!count) first = element;
    else last->next = element;
    count++;
    last = element;
    mutex.unlock();
  }

  iLinkable* pop()
  {
    mutex.lock();
    iLinkable* tmp = first;
    if (count)
    {
      first = first->next;
      count--;
    }
    mutex.unlock();
    return tmp;
  }

  int getCount()
  {
    return count;
  }
};

#endif // QUEUE_H
