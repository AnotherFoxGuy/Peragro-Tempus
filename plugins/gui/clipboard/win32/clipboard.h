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

#ifndef CSCLIPBOARD_WIN32_HEADER
#define CSCLIPBOARD_WIN32_HEADER

#include "csutil/scf_implementation.h"
#include "include/clipboard.h"
#include <iutil/comp.h>
#include <csutil/csstring.h>

struct iObjectRegistry;
struct iClipboard;
class csString;

//Generic types in windows.h
typedef unsigned int    UINT;
typedef void *          CS_HWND;
typedef void *          CS_HANDLE;
typedef void *          CS_PVOID;

class csClipboard : public scfImplementation2<csClipboard,iClipboard,iComponent>
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

    //The Empty function empties the clipboard and frees handles to data in the clipboard. The function then assigns ownership of the clipboard to the window that currently has the clipboard open. 
    virtual bool Empty();

    //The GetData function retrieves data from the clipboard in a specified format. The clipboard must have been opened previously. 
    virtual void GetData(csString &text, int clipboardType);

    //The SetData function places data on the clipboard in a specified clipboard format. The window must be the current clipboard owner, and the application must have called the OpenClipboard function. (When responding to the WM_RENDERFORMAT and WM_RENDERALLFORMATS messages, the clipboard owner must not call OpenClipboard before calling SetClipboardData.) 
    virtual bool SetData(csString text, int clipboardType);

    virtual void GetOS(csString &ostype) = 0;

    //The GetFormatName function retrieves from the clipboard the name of the specified registered format. The function copies the name to the specified buffer. 
    int GetFormatName(UINT format, LPSTR lpszFormatName, int cchMaxCount);

    //The GetOwner function retrieves the window handle of the current owner of the clipboard. 
    HWND GetOwner();

    //The GetOpenWindow function retrieves the handle to the window that currently has the clipboard open. 
    HWND GetOpenWindow();

    //The GetPriorityFormat function retrieves the first available clipboard format in the specified list. 
    int GetPriorityFormat(UINT *paFormatPriorityList, int cFormats);
    
    //The IsFormatAvailable function determines whether the clipboard contains data in the specified format. 
    bool IsFormatAvailable(UINT format);

    //The RegisterFormat function registers a new clipboard format. This format can then be used as a valid clipboard format. 
    UINT RegisterFormat(LPCSTR lpszFormat);

    //The DataFormat enumeration lists all the available clipboard formats.
    //The list is available in winuser.h as CF_*.
};

#endif
