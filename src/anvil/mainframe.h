/*
    Copyright (C) 2006 by Seth Yastrov
    Copyright (C) 2003 by Philipp Aumayr

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


#ifndef __MAINFRAME_H__
#define __MAINFRAME_H__

#include "csutil/weakref.h"
#include "csutil/ref.h"
#include <wx/frame.h>


class wxGauge;
class wxMenu;
class wxTextCtrl;
class wxButton;
class anvEngine;


class anvMainFrame : public wxFrame
{
public:
  anvMainFrame( const wxChar *title, anvEngine *engine, wxPoint pos, wxSize sz);
  ~anvMainFrame();

  //Menu Items
  wxStatusBar *statusBar;
  wxGauge *gauge;
  wxMenu *fileMenu;
  wxMenu *viewMenu;
  wxMenu *emMenu;
  wxMenu *engineMenu;
  wxMenuBar  *menuBar;
  wxToolBar* toolBar;

  wxPanel* panel3d;

  anvEngine* engine;

  wxPanel* GetPanel3d ()
  {
    return panel3d;
  }

  void PushFrame ();

  void OnWindowClose (wxCloseEvent& event);

  // File Event Methods
  void OnFileQuit (wxCommandEvent & event);
  void OnFileNew (wxCommandEvent & event);
  void OnFileOpen (wxCommandEvent & event);
  void OnFileSave (wxCommandEvent & event);
  void OnFileSaveAs (wxCommandEvent & event);
  
  // Edit Event Methods
  void OnEditUndo(wxCommandEvent & event);
  void OnEditRedo(wxCommandEvent & event);  

  // Engine Event Methods
  void OnRelightAll (wxCommandEvent &event);
  void OnImportLibrary (wxCommandEvent &event);

  void OnSize (wxSizeEvent& event);
	
  void ActivateAcceleratorTable();
  
  DECLARE_EVENT_TABLE()

};

enum
{ 
  // Menu commands

  ANV_FILE_START = 0,
  ANV_FILE_QUIT  =   1,
  ANV_FILE_NEW,
  ANV_FILE_OPEN,
  ANV_FILE_SAVE,
  ANV_FILE_SAVEAS,
  //EDIT  start: 10
  ANV_EDIT_START = 10,
  ANV_EDIT_UNDO,
  ANV_EDIT_REDO,

  // ENGINE start
  ANV_ENGINE = 100,
  ANV_ENGINE_RELIGHT_ALL,
  ANV_ENGINE_IMPORT_LIBRARY,

  // WINDOW start
  ANV_WINDOW_START = 200,
 
};

enum
{
  //StatusBar Items
  SFIELD_NAME,
  SFIELD_BLANK,
  SFIELD_GAUGE,
  SFIELD_MAX
};

#endif
