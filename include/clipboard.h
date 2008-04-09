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
