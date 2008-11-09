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

#include <queue>

template<class T>
class SynchronizedQueue
{
private:

  std::queue<T> queue;

  Mutex mutex;

public:
  SynchronizedQueue() {}
  ~SynchronizedQueue() {}

  void push(T element)
  {
    mutex.lock();
    queue.push(element);
    mutex.unlock();
  }

  T pop()
  {
    mutex.lock();
    T tmp = 0;
    if (queue.empty() == false)
    {
      tmp = queue.front();
      queue.pop();
    }
    mutex.unlock();
    return tmp;
  }

  int getCount()
  {
    mutex.lock();
    size_t count = queue.size();
    mutex.unlock();
    return count;
  }
};

#endif // QUEUE_H
