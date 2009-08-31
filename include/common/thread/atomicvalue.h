/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
 * @file atomicvalue.h
 *
 * @basic Simple wrapper around a value and a mutex.
 */

#ifndef PT_THREAD_ATOMICVALUE_H
#define PT_THREAD_ATOMICVALUE_H

#include <boost/thread/locks.hpp>

namespace PT
{
  namespace Thread
  {
    /**
     * Simple wrapper around a value and a mutex, to provide syncronization
     *   for simple types where pass by value is acceptable.
     */
    template<typename T>
    class AtomicValue
    {
    public:
      typedef T ValueType;

      /// Constructor, must initialize value.
      AtomicValue(T val);
      /// Copy constructor.
      AtomicValue(const AtomicValue& other);
      /// Assignment operator.
      AtomicValue& operator=(const AtomicValue& other);
      /// Set the value, after locking the mutex.
      AtomicValue& operator=(T val);
      /// Get the value, after locking the mutex.
      operator T() const;
      /// Set the value, after locking the mutex.
      void Set(T val);
      /// Get the value, after locking the mutex.
      T Get() const;

    private:
      typedef boost::mutex MutexType;
      typedef boost::lock_guard<MutexType> LockType;

      /// Mutex.
      mutable MutexType mutex;
      /// Value.
      T value;
    };

    template<typename T>
    AtomicValue<T>::AtomicValue(T val)
      : mutex(), value(val) {}

    template<typename T>
    AtomicValue<T>::AtomicValue(const AtomicValue& other)
      : mutex(), value(other.Get()) {}

    template<typename T>
    AtomicValue<T>& AtomicValue<T>::operator=(const AtomicValue& other)
    { Set(other.Get()); }

    template<typename T>
    AtomicValue<T>& AtomicValue<T>::operator=(T val)
    { Set(val); }

    template<typename T>
    AtomicValue<T>::operator T() const
    { return Get(); }

    template<typename T>
    inline void AtomicValue<T>::Set(T val)
    { LockType lock(mutex); value = val; }

    template<typename T>
    inline T AtomicValue<T>::Get() const
    { LockType lock(mutex); return value; }

  } // Thread namespace
} // PT namespace

#endif
