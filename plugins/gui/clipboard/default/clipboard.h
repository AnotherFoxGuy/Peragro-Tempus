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

#ifndef CLIPBOARD_DEFAULT_HEADER
#define CLIPBOARD_DEFAULT_HEADER

#include "csutil/scf_implementation.h"
#include <iutil/comp.h>
#include <csutil/csstring.h>
#include "include/clipboard.h"

struct iObjectRegistry;
struct iClipboard;
class csString;

class csClipboard : public scfImplementation2<csClipboard,iClipboard,iComponent>
{
private:
  iObjectRegistry* object_reg;

public:
    csClipboard (iBase* parent);
    virtual ~csClipboard();

    // From iComponent.
    virtual bool Initialize (iObjectRegistry*);

    virtual bool Open();
    virtual bool Close();
    virtual bool Empty();
    virtual void GetData(csString &text, int clipboardType);
    virtual bool SetData(csString text, int clipboardType);
    virtual void GetOS(csString &ostype);
};

#endif
