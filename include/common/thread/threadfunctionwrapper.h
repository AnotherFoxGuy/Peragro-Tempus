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
 * @file threadfunctionwrapper.h
 *
 * @basic
 */

#ifndef PT_THREAD_THREADFUNCTIONWRAPPER_H
#define PT_THREAD_THREADFUNCTIONWRAPPER_H

#include <boost/function.hpp>
#include <boost/exception/exception.hpp>

namespace PT
{
  namespace Thread
  {
    /**
     * Thread function wrapper which runs the contained target function in an
     *   infintite loop, with a thread interruption point after each iteration.
     */
    class ThreadFunctionWrapper
    {
    public:
      typedef boost::function<void()> FunctionType;

      /// Constructor.
      ThreadFunctionWrapper(const FunctionType& func);
      /// Function call operator.
      void operator()() const;

    private:
      /// Function to call.
      const FunctionType function;
    };

  } // Thread namespace
} // PT namespace

#endif
