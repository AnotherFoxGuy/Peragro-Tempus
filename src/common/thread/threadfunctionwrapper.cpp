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

#include "common/thread/threadfunctionwrapper.h"

#include <boost/thread/thread.hpp>

#include <iostream>
#include <boost/exception/diagnostic_information.hpp>

namespace PT
{
  namespace Thread
  {
    ThreadFunctionWrapper::ThreadFunctionWrapper(const FunctionType& func)
      : function(func)
    {
      BOOST_ASSERT(!function.empty());
    }

    void ThreadFunctionWrapper::operator()() const
    {
      try
      {
        while (true)
        {
          function();
          boost::this_thread::interruption_point();
        }
      }
      catch (boost::thread_interrupted& ex)
      {
        // This exception is thrown after interrupt() is called on the thread.
        throw;
      }
      catch (...)
      {
        std::cerr << "Unhandled exception in thread "
          << boost::this_thread::get_id() << std::endl
          << boost::current_exception_diagnostic_information();
        throw;
      }
    }

  } // Thread namespace
} // PT namespace
