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

#ifndef THREAD_H
#define THREAD_H

#include <boost/thread.hpp>
#include <boost/lambda/bind.hpp>

#include "common/util/exception.h"

#ifdef Yield
  #undef Yield
#endif

PT_DEFINE_EXCEPTION(ThreadMultipleStart);

/**
 * A OO thread which calls Run() once.
 */
class ThreadBase
{
protected:
  boost::shared_ptr<boost::thread> m_thread_ptr;
public:
  ThreadBase() : m_thread_ptr() { }
  virtual ~ThreadBase() { }

  virtual void Run() = 0;

  virtual void Begin()
  {
    if (!m_thread_ptr)
    {
      m_thread_ptr.reset(
        new boost::thread(
        boost::lambda::bind(&ThreadBase::Run, this)));
    }
    else
    {
      throw PT_EX(ThreadMultipleStart("multiple start"));
    }
  }

  virtual void Kill()
  {
    if (m_thread_ptr)
    {
      m_thread_ptr->join();
    }
  }

  virtual void Yield()
  {
    if (m_thread_ptr)
    {
      m_thread_ptr->yield();
    }
  }
};

/**
 * A OO thread which repeatly calls Run()
 */
class Thread : public ThreadBase
{
protected:
  bool isRunning;

  void running()
  {
    while (isRunning) this->Run();
  }

public:
  Thread() : ThreadBase(), isRunning(false) { }
  virtual ~Thread() { }

  virtual void Begin()
  {
    if (!m_thread_ptr)
    {
      isRunning = true;
      m_thread_ptr.reset(
        new boost::thread(
        boost::lambda::bind(&Thread::running, this)));
    }
    else
    {
      throw PT_EX(ThreadMultipleStart("multiple start"));
    }
  }

  virtual void Kill()
  {
    isRunning = false;
    if (m_thread_ptr)
    {
      m_thread_ptr->join();
    }
  }

  void End()
  {
    isRunning = false;
  }

  bool IsRunning()
  {
    return isRunning;
  }
};

#endif // THREAD_H
