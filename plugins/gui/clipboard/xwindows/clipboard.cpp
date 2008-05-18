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
#include "clipboard.h"
#include <iutil/objreg.h>
#include <iutil/plugin.h>

#define PIXMAP_W 200
#define PIXMAP_H 200

#include <X11/Xutil.h>
#include <iutil/plugin.h>
#include <X11/Xlib.h>


CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (csClipboard)

csClipboard::csClipboard(iBase* parent)
  : scfImplementationType (this, parent), object_reg(0)
{
}

csClipboard::~csClipboard ()
{
}

bool csClipboard::Initialize (iObjectRegistry* r)
{
  object_reg = r;

  //Query XWindow plugin
  csRef<iPluginManager> plugin_mgr =
    csQueryRegistry<iPluginManager> (object_reg);
  xwin = csQueryPluginClass<iXWindow> (plugin_mgr, "crystalspace.window.x");
  if (xwin)
  {
    SetDisplay(xwin->GetDisplay());
    SetWindow(xwin->GetWindow());
  }
  else
  {
    return false;
  }

  // Register the event handler and the events
  csRef<iEventNameRegistry> name_reg = csEventNameRegistry::GetRegistry (object_reg);
  csRef<iEventQueue> q = csQueryRegistry<iEventQueue> (object_reg);
  if (!q) return false;

  csevSelectionNotify = csEventNameRegistry::GetID(name_reg, "crystalspace.xwindow.clipboard.selection.notify");
  csevSelectionRequest = csEventNameRegistry::GetID(name_reg, "crystalspace.xwindow.clipboard.selection.request");
  csevSelectionClear = csEventNameRegistry::GetID(name_reg, "crystalspace.xwindow.clipboard.selection.clear");

  csEventID events[4] = { csevSelectionNotify,
                          csevSelectionRequest,
                          csevSelectionClear,
                          CS_EVENTLIST_END };

  CS::RegisterWeakListener (q, this, events, weakEventHandler);
  return true;
}

// Explanation for standard copy/paste in Xwindows:
//   1. Request for data is made from the requesting application
//   2. XConvertSelection throws SelectionRequest XEvent to X Server for the copy request
//   3. X Server throws SelectionRequest XEvent to the application that "owns" the data to be copied.
//   4. The application converts the data with XChangeProperty to the requested format and fills the SelectionNotify XEvent.
//   5. The "owning" application throws the SelectionNotify XEvent to the X Server to notify the "requesting" application the data is ready.
//   6. The X Server throws SelectionNotify XEvent to back to the requesting application.
//   7. The requesting application catches the SelectionNotify XEvent and extracts the data from the XEvent.
//   Note: During copy/paste to the SAME window, it will execute copy (for ctrl-c) then copy and paste (for ctrl-v).
//   However, in the implementation displayed here it only executes copy and uses the data stored as the return value for paste text. This allows SelectionNotify to be caught and processed within one function instead of requiring a separate function to return the data.

bool csClipboard::HandleEvent(iEvent& event)
{
  XEvent e = xwin->GetStoredEvent();

  if (event.Name == csevSelectionRequest)
  {
    DoCopy(&e.xselectionrequest);
  }
  else if (event.Name == csevSelectionClear)
  {
    Empty();
  }

  return true;
}

//Opens the clipboard for examination and prevents other applications from modifying the clipboard content.
bool csClipboard::Open()
{
  return true;
}

//Closes the clipboard.
bool csClipboard::Close()
{
  return true;
}

//Empties the clipboard and frees handles to data in the clipboard.
bool csClipboard::Empty()
{
  //XSelectInput(dpy, ev.requestor, NoEventMask);
  //XFlush(dpy);
  return true;
}

//Retrieves data from the clipboard in a specified format. (aka Paste)
//During a paste, SelectionNotify is sent from the local window to the remote window asking for text.
void csClipboard::GetData(csString &text, int clipboardType)
{
  XEvent event;
  Atom theClipboard;

  switch (clipboardType)
  {
  case ClipboardClipboard:
    theClipboard = XInternAtom(display, "CLIPBOARD", 0);
    break;
  case PrimaryClipboard:
    theClipboard = XA_PRIMARY;
    break;
  case SecondaryClipboard:
    theClipboard = XA_SECONDARY;
    break;
  default:
    theClipboard = XInternAtom(display, "CLIPBOARD", 0);
    break;
  }

  //Get an atom with the given name
  //Atom property = XInternAtom (display, "VT_SELECTION", False);

  //Request that the selection be converted to the specified target type.
  //XConvertSelection (display, theClipboard, XA_STRING, property, window, CurrentTime);
  XConvertSelection (display, theClipboard, XA_STRING, XA_PRIMARY, window, CurrentTime);

  //XCheckMaskEvent(display, SelectionNotify, &event);
  XNextEvent (display, &event);
  if (event.type == SelectionRequest)
  {
    // This XEvent is called first during paste only if you are copying
    // and pasting to the same window.
    // Since the data was copied to the storage location during copy, we will skip
    // the back and forth messages with the X Server and just set the text value.
    text = string_storage;
  }
  else if (event.type == SelectionNotify)
  {
    DoPaste (event.xselection.requestor, event.xselection.property, True, text);
  }
}

Bool csClipboard::convertProperty(Display *display, Window w, Atom target, Atom property)
{
  if (target == _XA_TARGETS)
  {
    XChangeProperty(
    display, w, property, XA_ATOM, 32, PropModeReplace,
    (unsigned char *)conversion_targets, MAX_TARGETS);
  }
  else if (target == _XA_TIMESTAMP)
  {
    long local_managing_since;

    local_managing_since = managing_since;
    XChangeProperty(
    display, w, property, XA_INTEGER, 32, PropModeReplace,
    (unsigned char *)&local_managing_since, 1);
  }
  else if (target == _XA_VERSION)
  {
    const char * version = "2.0";
    XChangeProperty(
    display, w, property, XA_INTEGER, 32, PropModeReplace, (const unsigned char*) version,2);
  }
  else
  {
    return false;
  }

  XFlush(display);
  return True;
}

//Sets the clipboard object using the specified format. (aka Copy)
//During a copy, the local window is set as the primary window and tells everyone it has text available for pasting.
bool csClipboard::SetData(csString text, int clipboardType)
{
  Atom theClipboard;

  switch (clipboardType)
  {
  case ClipboardClipboard:
    theClipboard = XInternAtom(display, "CLIPBOARD", 0);
    break;
  case PrimaryClipboard:
    theClipboard = XA_PRIMARY;
    break;
  case SecondaryClipboard:
    theClipboard = XA_SECONDARY;
    break;
  default:
    theClipboard = XInternAtom(display, "CLIPBOARD", 0);
    break;
  }

  //XSetSelectionOwner(display,theClipboard,window,CurrentTime);

  //Override standard behavior and copy to BOTH clipboards.
  //This allows middle-click to access the text in the clipboard in legacy X applications.
  XSetSelectionOwner(display, XInternAtom(display, "CLIPBOARD", 0), window, CurrentTime);
  XSetSelectionOwner(display, XA_PRIMARY, window, CurrentTime);

  //Verify XSetSelectionOwner by getting Selection owner and comparing to current window.
  Window owner = XGetSelectionOwner (display,theClipboard);
  if (owner != window)
  {
    printf("XSetSelectionOwner failed.\n");
  }

  //Set clipboard object variable
  string_storage = text;

  return false;
}

void csClipboard::DoCopy(XSelectionRequestEvent* rq)
{
  XSelectionEvent notify_event;

  int nitems;
  nitems = 1;
  XTextProperty  xtpText = {0};
  int iReturn;
  char *pszTextList[2] = {NULL};
  XICCEncodingStyle    xiccesStyle;
  pszTextList[0] = (char *)string_storage.GetData();
  pszTextList[1] = NULL;
  xiccesStyle = XStringStyle;
  iReturn = Xutf8TextListToTextProperty (display, pszTextList, 1,xiccesStyle, &xtpText);
  switch (iReturn)
  {
  case XLocaleNotSupported:
    csPrintf("Copy failed - Locale Not Supported!\n");
    break;
  case XNoMemory:
    csPrintf("Copy failed - X server ran out of memory!\n");
    break;
  }

  iReturn = XChangeProperty (display, rq->requestor, rq->property, XA_STRING, 8, PropModeReplace,
    xtpText.value, xtpText.nitems);
  notify_event.display = rq->display;
  //XSelectInput(dpy, ev.requestor, NoEventMask);
  //XFlush(dpy);
  notify_event.type = SelectionNotify;
  notify_event.requestor = rq->requestor;
  notify_event.selection = rq->selection;
  notify_event.target = rq->target;
  notify_event.time = rq->time;
  notify_event.property = rq->property;
  XSendEvent (display, rq->requestor, False, 0L, (XEvent*) &notify_event);
}

//Internal method only. Used to return text from selection
void csClipboard::DoPaste(int requestorWindow, int property, int deleted, csString &text)
{
  Atom actual_type;
  int actual_format;
  unsigned long long_offset, long_length, nitems, bytes_after;
  unsigned char *data;

  //Setup initial values
  if (property == None)
  {
    requestorWindow = DefaultRootWindow (display);
    //Set built-in property name
    property = XA_CUT_BUFFER0;
    deleted = False;
  }

  //Specifies the offset in a specified property (in 32-bit quantities)
  //where the data is to be retrieved.
  long_offset = 0;

  //Specifies the length in 32-bit multiples of the data to be retrieved.
  long_length = 1024;

  //Get the actual type of the property, the actual format of the property, the number of 8-bit, 16-bit or
  //32-bit items transferred; the number of bytes to be remaining to be read in the property; and a pointer
  //to the data actually returned; Make sure the data retrieved from the clipboard conforms to our needs.
  if ((XGetWindowProperty (display, requestorWindow, property, long_offset/4, long_length, deleted, AnyPropertyType,
      &actual_type, &actual_format, &nitems, &bytes_after, &data) != Success) ||
      (actual_type != XA_STRING))
  {
    XFree(data);
    return;
  }

  //TODO: May need to cast data above to extract wchar_t properly.
  //Copy data from clipboard into return text buffer
  char* txt = NULL;

  //Set data to intermediate variable so it can be copied.
  txt = (char *) data;
  text = txt;

  //Free allocated memory
  XFree (data);
}

void csClipboard::SetDisplay(Display *dpy)
{
  display = dpy;
}

void csClipboard::SetWindow(Window win)
{
  window = win;
}

void csClipboard::GetOS(csString &ostype)
{
  ostype = "XWindow";
}

