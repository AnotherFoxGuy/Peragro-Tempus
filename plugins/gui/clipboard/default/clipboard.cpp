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
#include <iutil/objreg.h>
#include "clipboard.h"

SCF_IMPLEMENT_FACTORY (csClipboard)

csClipboard::csClipboard(iBase* parent)
  : scfImplementationType (this, parent), object_reg(0)
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

