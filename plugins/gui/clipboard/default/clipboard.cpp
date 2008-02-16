/*
    Copyright (C) 2001 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cssysdef.h>
#include <iutil/objreg.h>
#include "clipboard.h"

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (csClipboard)

csClipboard::csClipboard(iBase* parent) :
	scfImplementationType (this, parent),
	object_reg(0)
{
}

csClipboard::~csClipboard()
{
}

bool csClipboard::Initialize (iObjectRegistry* r)
{
  object_reg = r;
  return true;
}

bool csClipboard::Open() 
{
    return false;
}

bool csClipboard::Close()
{
    return true;
}

bool csClipboard::Empty()
{
    return true;
}

void csClipboard::GetData(csString &text, int clipboardType)
{
    text = "";
}

bool csClipboard::SetData(csString text, int clipboardType)
{
    return true;
}

void csClipboard::GetOS(csString &ostype)
{
    ostype = "Default";
}

