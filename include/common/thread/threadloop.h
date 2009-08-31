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
 * @file threadloop.h
 *
 * @basic Wrapper around boost::thread for looping on a function.
 */

#ifndef PT_THREAD_THREADLOOP_H
#define PT_THREAD_THREADLOOP_H

#include <boost/assert.hpp>

#include "common/thread/singlethread.h"
#include "common/thread/storagepolicy.h"

namespace PT
{
  namespace Thread
  {
    /**
     * Wrapper around boost::thread that calls the supplied member function
     *   repeatedly until stopped.
     */
    template<typename T, template <typename> class StoragePolicy_ = OwnedStorage,
      typename ThreadPolicy_ = SingleThread>
    class ThreadLoop : public ThreadPolicy_
    {
    public:
      typedef StoragePolicy_<T> StoragePolicy;
      typedef ThreadPolicy_ ThreadPolicy;
      typedef typename StoragePolicy::ArgType ArgType;
      typedef void (T::*FunctionPtr)();

      /// Constructor; before calling Start() the target object needs to be set.
      ThreadLoop(FunctionPtr fPtr);
      /// Constructor; starts a thread running immediately.
      ThreadLoop(FunctionPtr fPtr, ArgType obj);
      /// Set the object for the function pointer.
      void Set(ArgType obj);
      /// Get the object.
      T* Get() const;

    private:
      /// Functor for the thread to call.
      struct ThreadFunction
      {
        /// Constructor.
        ThreadFunction(FunctionPtr fPtr);
        /// Constructor.
        ThreadFunction(FunctionPtr fPtr, ArgType obj);
        /// Function call operator.
        void operator()() const;

        /// Member function pointer of object (T).
        FunctionPtr functionPtr;
        /// Object pointer.
        StoragePolicy object;
      } function;
    };

    template<typename T, template <typename> class SP, typename TP>
    ThreadLoop<T,SP,TP>::ThreadLoop(FunctionPtr fPtr)
      : ThreadPolicy(boost::cref(function)), function(fPtr) {}

    template<typename T, template <typename> class SP, typename TP>
    ThreadLoop<T,SP,TP>::ThreadLoop(FunctionPtr fPtr, ArgType obj)
      : ThreadPolicy(boost::cref(function)), function(fPtr, obj)
    { ThreadPolicy::Start(); }

    template<typename T, template <typename> class SP, typename TP>
    inline void ThreadLoop<T,SP,TP>::Set(ArgType obj)
    { function.object.Set(obj); BOOST_ASSERT(function.object.Get() != 0); }

    template<typename T, template <typename> class SP, typename TP>
    inline T* ThreadLoop<T,SP,TP>::Get() const
    { return function.object.Get(); }

    template<typename T, template <typename> class SP, typename TP>
    ThreadLoop<T,SP,TP>::ThreadFunction::ThreadFunction(FunctionPtr fPtr)
      : functionPtr(fPtr)
    { BOOST_ASSERT(functionPtr != 0); }

    template<typename T, template <typename> class SP, typename TP>
    ThreadLoop<T,SP,TP>::ThreadFunction::ThreadFunction(FunctionPtr fPtr, ArgType obj)
      : functionPtr(fPtr), object(obj)
    { BOOST_ASSERT(functionPtr != 0); BOOST_ASSERT(object.Get() != 0); }

    template<typename T, template <typename> class SP, typename TP>
    void ThreadLoop<T,SP,TP>::ThreadFunction::operator()() const
    { (object.Get()->*functionPtr)(); }

  } // Thread namespace
} // PT namespace

#endif
