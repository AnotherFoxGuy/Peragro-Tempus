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

#ifndef CLIPBOARD_DEFAULT_HEADER
#define CLIPBOARD_DEFAULT_HEADER

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csgeom/vector3.h>
#include <csutil/csstring.h>
#include <csutil/refarr.h>
#include "include/client/component/entity/move/viewbob.h"
#include "include/client/component/componenttmpl.h"

#include "client/component/stdcomponentimp.h"

struct iObjectRegistry;
struct iEvent;

namespace PT
{
  namespace Events
  {
    struct EventHandlerCallback;
  }
  namespace Entity
  {
    class Entity;
  }
} //PT namespace

DECLARE_COMPONENTFACTORY (ViewBob)

class ComponentViewBob : public scfImplementationExt1<ComponentViewBob, ComponentCommon, iViewBob>
{
private:
  iObjectRegistry* object_reg;
  PointerLibrary* pointerlib;

  PT::Entity::Entity* entity;
  csRefArray<PT::Events::EventHandlerCallback> eventHandlers;

public:
    ComponentViewBob (iObjectRegistry*);
    virtual ~ComponentViewBob();

    virtual bool Initialize (PointerLibrary*, PT::Entity::Entity*);
};

#endif
