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

#ifndef CSCLIPBOARD_MACOSX_HEADER
#define CSCLIPBOARD_MACOSX_HEADER

#include "csutil/scf_implementation.h"
#include <iutil/comp.h>
#include "include/clipboard.h"
#include <string.h>

#include <Carbon/Carbon.h>

struct iObjectRegistry;
struct iClipboard;

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (csClipboard)

class csClipboard : scfImplementation2<csClipboard,iClipboard,iComponent>
{
private:
  iObjectRegistry* object_reg;

public:

  csClipboard (iBase* parent);
  virtual ~csClipboard();

  // From iComponent.
  virtual bool Initialize (iObjectRegistry*);

  //The Open function opens the clipboard for examination and prevents other applications from modifying the clipboard content.
  virtual bool Open();

  //The Close function closes the clipboard.
  virtual bool Close();

  //The Empty function empties the clipboard and frees handles to data in the clipboard.
  virtual bool Empty();

  //The GetData function retrieves data from the clipboard in a specified format.
  virtual void GetData(csString &text, int clipboardType);

  //The SetData function places data on the clipboard in a specified clipboard format.
  virtual bool SetData(csString text, int clipboardType);

  virtual void GetOS(csString &ostype);

private:
  PasteboardRef clipboardRef;
};

#endif
