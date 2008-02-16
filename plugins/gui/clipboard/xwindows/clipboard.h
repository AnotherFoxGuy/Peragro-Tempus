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

#ifndef CSCLIPBOARD_XWINDOWS_HEADER
#define CSCLIPBOARD_XWINDOWS_HEADER

#include "csutil/scf_implementation.h"
#include <iutil/comp.h>
#include "include/clipboard.h"

#include <ivaria/xwindow.h>
#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>
#include <iutil/eventnames.h>
#include <csutil/event.h>
#include <csutil/weakref.h>
#include <csutil/eventnames.h>

#include <X11/Xlib.h>
#include <X11/Xmd.h>          //For Card32 typedef
#include <X11/Xatom.h>
//#include <X11/Xmu/Atoms.h>
#include <csutil/csstring.h>

struct iObjectRegistry;
struct iClipboard;
struct iXWindow;
struct iEventHandler;

class csClipboard : public scfImplementation3<csClipboard, iClipboard, iEventHandler, iComponent>
{
private:
    iObjectRegistry* object_reg;
    csRef<iEventHandler> weakEventHandler;
    csRef<iXWindow> xwin;
    Display* display;
    Window window;
  
    csString string_storage;
    Time managing_since;
    csEventID csevSelectionNotify;
    csEventID csevSelectionRequest;
    csEventID csevSelectionClear;


public:

    Atom _XA_WM_SX;
    Atom _XA_MANAGER;
    Atom _XA_ATOM_PAIR;
    Atom _XA_WM_COLORMAP_NOTIFY;
    Atom XA_CLIPBOARD;
#   define _XA_TARGETS   conversion_targets[0]
#   define _XA_MULTIPLE  conversion_targets[1]
#   define _XA_TIMESTAMP conversion_targets[2]
#   define _XA_VERSION   conversion_targets[3]
#   define MAX_TARGETS                      4

    Atom conversion_targets[MAX_TARGETS];

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

    //The SetData function places data on the clipboard in a specified clipboard format.  The clipboard must have been opened previously.
    virtual bool SetData(csString text, int clipboardType);

   virtual void DoPaste(int requestorWindow, int property, int deleted, csString &text);
   virtual void DoCopy(XSelectionRequestEvent* rq);
   virtual void SetDisplay(Display* display);
   virtual void SetWindow(Window window);
   virtual void GetOS(csString &ostype);

   Bool convertProperty(Display *display, Window w, Atom target, Atom property);

   virtual bool HandleEvent(iEvent &e);
   CS_EVENTHANDLER_NAMES("crystalspace.xwindow.clipboard")
   CS_EVENTHANDLER_NIL_CONSTRAINTS
};

#endif
