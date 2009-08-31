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

#include <queue>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

template<class T>
class SynchronizedQueue
{
public:
  SynchronizedQueue() {}
  ~SynchronizedQueue() {}

  void push(T element)
  {
    LockType lock(mutex);
    queue.push(element);
  }

  T pop()
  {
    LockType lock(mutex);
    T tmp = T();
    if (queue.empty() == false)
    {
      tmp = queue.front();
      queue.pop();
    }
    return tmp;
  }

  int getCount()
  {
    LockType lock(mutex);
    size_t count = queue.size();
    return count;
  }

private:
  std::queue<T> queue;

  typedef boost::lock_guard<boost::mutex> LockType;
  boost::mutex mutex;
};

#endif // QUEUE_H
