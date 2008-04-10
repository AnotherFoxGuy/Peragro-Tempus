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

