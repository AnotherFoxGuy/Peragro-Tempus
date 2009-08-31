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

#ifndef PT_THREAD_LOCKWRAPPER_H
#define PT_THREAD_LOCKWRAPPER_H

#include <boost/thread/locks.hpp>

template<typename T>
class LockWrapper
{
public:
  typedef boost::detail::thread_move_t<LockWrapper<T> > MoveType;

  LockWrapper(boost::mutex& m, T& obj);
  LockWrapper(MoveType other);
  MoveType move();
  operator MoveType();
  const T& get() const;
  T& get();

private:
  boost::unique_lock<boost::mutex> lock;
  T& object;
};

template<typename T>
LockWrapper<T>::LockWrapper(boost::mutex& m, T& obj)
  : lock(m), object(obj) {}

template<typename T>
LockWrapper<T>::LockWrapper(MoveType other)
  : lock(other->lock.move()), object(other->object) {}

template<typename T>
typename LockWrapper<T>::MoveType LockWrapper<T>::move()
{ return MoveType(*this); }

template<typename T>
LockWrapper<T>::operator MoveType()
{ return move(); }

template<typename T>
const T& LockWrapper<T>::get() const
{ return object; }

template<typename T>
T& LockWrapper<T>::get()
{ return object; }

#endif
