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

#include "common/thread/multithread.h"

#include <boost/make_shared.hpp>

namespace PT
{
  namespace Thread
  {
    using boost::thread;

    MultiThread::MultiThread(const FunctionType& f)
      : function(f)
    {
    }

    MultiThread::~MultiThread()
    {
      Stop();
    }

    void MultiThread::Start(size_t number)
    {
      for (size_t i = 0; i < number; ++i)
      {
        threads.insert(boost::make_shared<thread>(boost::cref(function)));
      }
    }

    void MultiThread::Stop(const ThreadGroup::iterator& begin,
      const ThreadGroup::iterator& end)
    {
      for (ThreadGroup::iterator itr(begin); itr != end; ++itr)
      {
        (*itr)->interrupt();
      }
      for (ThreadGroup::iterator itr(begin); itr != end; ++itr)
      {
        (*itr)->join();
      }
    }

    void MultiThread::Stop()
    {
      if (threads.empty()) return;
      Stop(threads.begin(), threads.end());
      threads.clear();
    }

    void MultiThread::Stop(size_t number)
    {
      const size_t size = threads.size();
      if (number >= size)
      {
        Stop();
        return;
      }

      ThreadGroup::reverse_iterator first(threads.rbegin());
      for (size_t i = 0; i < number ; ++i)
      {
        ++first;
      }
      const ThreadGroup::iterator begin(first.base());
      const ThreadGroup::iterator end(threads.end());
      Stop(begin, end);
      threads.erase(begin, end);
    }

    const thread& MultiThread::GetThread(const thread::id& id) const
    {
      ThreadGroup::const_iterator itr(threads.find(id));
      if (itr == threads.end())
      {
        throw PT_EX(Exception("Thread not found")) << ThreadIdInfo(id);
      }
      return **itr;
    }

    thread& MultiThread::GetThread(const thread::id& id)
    {
      ThreadGroup::iterator itr(threads.find(id));
      if (itr == threads.end())
      {
        throw PT_EX(Exception("Thread not found")) << ThreadIdInfo(id);
      }
      return **itr;
    }

  } // Thread namespace
} // PT namespace
