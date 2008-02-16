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
#ifndef ICLIPBOARD_HEADER
#define ICLIPBOARD_HEADER

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

class csString;

struct iClipboard : public virtual iBase
{
    SCF_INTERFACE(iClipboard, 1,0,0);

    virtual bool Open() = 0;

    virtual bool Close() = 0;

    virtual bool Empty() = 0;

    virtual void GetData(csString &text, int clipboardType) = 0;

    virtual bool SetData(csString text, int clipboardType) = 0;

    virtual void GetOS(csString &ostype) = 0;

};

enum ClipboardType { ClipboardClipboard = 0,PrimaryClipboard = 1,SecondaryClipboard = 2 };

#endif
