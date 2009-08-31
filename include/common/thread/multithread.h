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
 * @file
 *
 * @basic
 */

#ifndef PT_THREAD_MULTITHREAD_H
#define PT_THREAD_MULTITHREAD_H

#include <boost/shared_ptr.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/thread/thread.hpp>

#include "common/util/exception.h"

#include "common/thread/threadfunctionwrapper.h"

namespace PT
{
  namespace Thread
  {
    PT_DEFINE_ERRORINFO(ThreadId, boost::thread::id);

    class MultiThread
    {
    public:
      typedef ThreadFunctionWrapper::FunctionType FunctionType;

      MultiThread(const FunctionType& f);
      void Start(size_t number = 1);
      void Stop();
      void Stop(size_t number);
      const boost::thread& GetThread(const boost::thread::id& id) const;
      boost::thread& GetThread(const boost::thread::id& id);

    private:
      typedef boost::shared_ptr<boost::thread> ThreadPtr;
      typedef boost::multi_index::multi_index_container<ThreadPtr,
        boost::multi_index::indexed_by<boost::multi_index::ordered_unique<
          boost::multi_index::const_mem_fun<
            boost::thread, boost::thread::id, &boost::thread::get_id>
      > > > ThreadGroup;

      void Stop(const ThreadGroup::iterator& begin,
        const ThreadGroup::iterator& end);

      ThreadFunctionWrapper function;
      ThreadGroup threads;
    };

  } // Thread namespace
} // PT namespace

#endif
