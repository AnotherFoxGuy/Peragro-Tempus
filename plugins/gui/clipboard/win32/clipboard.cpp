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
#include "clipboard.h"
#include <iutil/objreg.h>
#include <iutil/plugin.h>

#include <winuser.h>

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

//Opens the clipboard for examination and prevents other applications from modifying the clipboard content.
// Using NULL as the window handle makes Windows use active task by default.
bool csClipboard::Open() 
{
    return (::OpenClipboard((HWND) NULL) == TRUE ? true : false);
}

//Closes the clipboard. 
bool csClipboard::Close()
{
    return (::CloseClipboard()== TRUE ? true : false);
}

//Empties the clipboard and frees handles to data in the clipboard. The function then assigns ownership of the clipboard to the window that currently has the clipboard open. 
bool csClipboard::Empty()
{
    return (::EmptyClipboard() == TRUE ? true : false);
}

//Retrieves data from the clipboard in a specified format.
void csClipboard::GetData(csString &text, int clipboardType)
{
    HGLOBAL dataHandle;
    LPWSTR dataPtr;
    text = "";

    if (!IsFormatAvailable(CF_UNICODETEXT)) 
       return;
    if (!Open())
       return; 
    
	if ((dataHandle = ::GetClipboardData(CF_UNICODETEXT)))
    {
        dataPtr = (LPWSTR)GlobalLock(dataHandle);
        GlobalUnlock(dataHandle);
    }

    Close();

    if (dataPtr)
        text = csString(dataPtr);
}

//Sets the clipboard object using the specified format.
bool csClipboard::SetData(csString text, int clipboardType)
{
    HGLOBAL dataHandle;
    LPWSTR dataPtr;

    if(!Open()) 
        return false;

    Empty();

    WCHAR *source = csStrNewW(text.GetData());
    if (source == NULL || *source == '\0') {
       Close();
       return false;
    }

    size_t cch = wcslen (source);
    dataHandle = GlobalAlloc(GMEM_MOVEABLE, (cch + 1) * sizeof(WCHAR)); 

    // Lock the handle
    if (dataHandle && (dataPtr = (LPWSTR) GlobalLock(dataHandle)))
    {
        //Copy the text to the buffer
        memcpy(dataPtr, source, cch * sizeof(WCHAR));
        //Release the handle
        GlobalUnlock(dataHandle);
        // Place the handle on the clipboard. 
        ::SetClipboardData(CF_UNICODETEXT, dataHandle);
    }
    
    Close(); 
    delete source;

    return true;
}

//Retrieves from the clipboard the name of the specified registered format. The function copies the name to the specified buffer. 
int csClipboard::GetFormatName(UINT format, LPSTR lpszFormatName, int cchMaxCount)
{
    return ::GetClipboardFormatName(format, lpszFormatName, cchMaxCount);
}

//Retrieves the window handle of the current owner of the clipboard. 
HWND csClipboard::GetOwner()
{
    return ::GetClipboardOwner();
}

//Retrieves the handle to the window that currently has the clipboard open. 
HWND csClipboard::GetOpenWindow()
{
    return ::GetOpenClipboardWindow();
}

//Retrieves the first available clipboard format in the specified list. 
int csClipboard::GetPriorityFormat(UINT *paFormatPriorityList, int cFormats)
{
    return ::GetPriorityClipboardFormat(paFormatPriorityList, cFormats);
}

//Determines whether the clipboard contains data in the specified format. 
bool csClipboard::IsFormatAvailable(UINT format)
{
    return (::IsClipboardFormatAvailable(format) == TRUE ? true : false);
}

//Registers a new clipboard format. This format can then be used as a valid clipboard format. 
UINT csClipboard::RegisterFormat(LPCSTR lpszFormat)
{
    return ::RegisterClipboardFormat(lpszFormat);
}




