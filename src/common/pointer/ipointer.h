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

#ifndef POINTERLIB_H_
#define POINTERLIB_H_

struct iObjectRegistry;

namespace PT
{
  class Reporter;

  namespace Events
  {
    class EventManager;
  } // Events namespace

} // PT namespace

class iPointerLibrary
{
public:
  /**
   * Constructor.
   */
  iPointerLibrary() {}
  /**
   * Virtual destructor.
   */
  virtual ~iPointerLibrary() {}

  /// Set the object registry.
  virtual void setObjectRegistry(iObjectRegistry* objreg) = 0;
  /// Get the object registry.
  virtual iObjectRegistry* getObjectRegistry() = 0;

  /// Set the event manager.
  virtual void setEventManager(PT::Events::EventManager* eventManager) = 0;
  /// Get the event manager.
  virtual PT::Events::EventManager* getEventManager() = 0;

  /// Set the reporter.
  virtual void setReporter(PT::Reporter* reporter) = 0;
  /// Get the reporter.
  virtual PT::Reporter* getReporter() = 0;

};

#endif // POINTERLIB_H_
