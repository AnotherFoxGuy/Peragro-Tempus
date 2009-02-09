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

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <iutil/plugin.h>

#include "client/pointer/pointer.h"

#include "pointerplug.h"

SCF_REGISTER_STATIC_CLASS(PointerPlug, PT_POINTERLIBRARY_PLUGNAME, "Pointer Library interface", NULL)

PointerPlug::PointerPlug(iBase* parent) :
  scfImplementationType(this, parent), object_reg(0)
{
}

PointerPlug::~PointerPlug()
{ }

bool PointerPlug::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}

PointerLibrary* PointerPlug::getPointerLibrary () const
{
  return PointerLibrary::getInstance();
}
