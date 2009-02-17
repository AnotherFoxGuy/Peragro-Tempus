/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef ICOMPONENT_H
#define ICOMPONENT_H

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

class PointerLibrary;

namespace Client
{
  namespace Entity
  {
    class Entity;
  }
} //Client namespace

struct ComponentInterface;

struct PropertyChangeCallback;


struct ComponentFactoryInterface : public virtual iBase
{
  SCF_INTERFACE (ComponentFactoryInterface, 0, 0, 1);

  virtual const char* GetName () const = 0;

  virtual csPtr<ComponentInterface> CreateComponent (const char* name) = 0;
};

struct ComponentInterface : public virtual iBase
{
  SCF_INTERFACE(ComponentInterface, 1,0,0);

  virtual const char* GetName () const = 0;

  virtual void SetName (const char* name) = 0;

  virtual const char* GetTag () const = 0;

  virtual void SetTag (const char* tagname) = 0;
/*
  virtual bool AddPropertyChangeCallback (PropertyChangeCallback* cb) = 0;

  virtual bool RemovePropertyChangeCallback (PropertyChangeCallback* cb) = 0;

  virtual bool SetProperty (const char* propertyID, const char* value) = 0;

  virtual const char* GetProperty (const char* propertyID) = 0;
  */

};


#endif // ICOMPONENT_H
