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

#ifndef STDCOMPONENT_H
#define STDCOMPONENT_H

#include <cstypes.h>
#include <iutil/comp.h>
#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include "include/client/component/component.h"


struct iObjectRegistry;

/**
 * A macro to help register an eventhandler. 
 * @param Class The class which is registering the eventhandler.
 * @param funct The function which will handle the event.
 * @param ev The event which to subscribe to.
 * @param specific Wether or not to make this event entity specific.
 */
#define REGISTER_LISTENER(Class, funct, ev, specific)       \
REGISTER_LISTENER2(Class, funct, ev, specific, FUNC(funct)) \

// Some macro magic to generate unique variable names.
#define FUNC2(x,y) cb##x##y
#define FUNC1(x,y) FUNC2(x,y)
#ifdef WIN32
  #define FUNC(x) FUNC1(x,__COUNTER__)
#else
  #define FUNC(x) FUNC1(x,__LINE__)
#endif
//-------------------------------------------------------------------------------------
#define REGISTER_LISTENER2(Class, funct, ev, specific, var)                           \
using namespace PT::Events;                                                           \
csRef<EventHandlerCallback> var;                                                      \
var.AttachNew(new EventHandler</**/Class>(&/**/Class::/**/funct, this));              \
if (specific)                                                                         \
evmgr->AddListener(EntityHelper::MakeEntitySpecific(ev, entity->GetId()), var);       \
else                                                                                  \
  evmgr->AddListener(ev, var);                                                        \
eventHandlers.Push(var);                                                              \
//-------------------------------------------------------------------------------------

class ComponentCommon : public scfImplementation1<ComponentCommon, ComponentInterface>
{
private:
  char* tag;
  const char* name;
  iObjectRegistry* object_reg;

public:
  ComponentCommon (iObjectRegistry* object_reg);
  virtual ~ComponentCommon ();

  virtual void SetTag (const char* tagname);
  virtual const char* GetTag () const { return tag; }

  virtual void SetName (const char* pcname);
  virtual const char* GetName () const { return name; }

};

#endif // STDCOMPONENT_H

