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

#include <cssysdef.h>
#include <csutil/util.h>
#include <iutil/objreg.h>
#include "stdcomponentimp.h"

ComponentCommon::ComponentCommon (iObjectRegistry* object_reg) : scfImplementationType (this)
{
  ComponentCommon::object_reg = object_reg;
  tag = 0;
  name = 0;
}

ComponentCommon::~ComponentCommon ()
{
  delete[] tag;
  delete[] name;
}

void ComponentCommon::SetTag (const char* tagname)
{
  delete[] tag;
  tag = csStrNew (tagname);
}

void ComponentCommon::SetName (const char* cname)
{
  delete[] name;
  name = csStrNew (cname);
}
