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


#include "wx/wxprec.h"

#ifndef WX_PRECOMP
        #include "wx/wx.h"
#endif

#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/artprov.h>
#include <wx/confbase.h>
#include <wx/dcscreen.h>

#include "cssysdef.h"
#include "mainframe.h"
#include "cssvfsfiledlg.h"
#include "engine.h"
#include "iutil/stringarray.h"
#include "iutil/eventq.h"
#include "iutil/objreg.h"
#include "iutil/virtclk.h"
#include "iengine/engine.h"
#include "imap/loader.h"
#include "ivideo/graph2d.h"
#include "ivideo/wxwin.h"
#include "ivaria/view.h"

BEGIN_EVENT_TABLE (anvMainFrame, wxFrame)
  EVT_CLOSE (anvMainFrame::OnWindowClose)
  EVT_SIZE (anvMainFrame::OnSize)

  //File Menu Events
  EVT_MENU ( ANV_FILE_QUIT,  anvMainFrame::OnFileQuit)
  EVT_MENU ( ANV_FILE_NEW,  anvMainFrame::OnFileNew)
  EVT_MENU ( ANV_FILE_OPEN,  anvMainFrame::OnFileOpen)
  EVT_MENU ( ANV_FILE_SAVE,  anvMainFrame::OnFileSave)
  EVT_MENU ( ANV_FILE_SAVEAS,anvMainFrame::OnFileSaveAs)
  EVT_MENU ( ANV_EDIT_UNDO,  anvMainFrame::OnEditUndo)
  EVT_MENU ( ANV_EDIT_REDO,  anvMainFrame::OnEditRedo)

  // Engine Menu Events
  EVT_MENU ( ANV_ENGINE_RELIGHT_ALL, anvMainFrame::OnRelightAll )
  EVT_MENU ( ANV_ENGINE_IMPORT_LIBRARY, anvMainFrame::OnImportLibrary )
END_EVENT_TABLE()

#include "images/anvil.xpm"



anvMainFrame::anvMainFrame( const wxChar *title,  anvEngine *engine, wxPoint pos, wxSize sz) :
                wxFrame
                ( (wxFrame *) NULL,
                   -1,
                   title,
                   pos,
                   sz,
                   wxDEFAULT_FRAME_STYLE
                )

{
  this->engine = engine;
  fileMenu = NULL;
  menuBar = NULL;
  statusBar = NULL;

#if wxUSE_XPM
  wxImage::AddHandler( new wxXPMHandler );
#endif

  wxIcon icon (wxICON (anvil));
  SetIcon (icon);

  wxBitmap bmFileOpen (wxArtProvider::GetBitmap (wxART_FILE_OPEN,
    wxART_TOOLBAR));

  wxMenuItem* item;
  // Create the menu
  // file menu
  fileMenu = new wxMenu();
  fileMenu->Append(ANV_FILE_NEW, "&New\tCtrl-N" );
  item = new wxMenuItem (fileMenu, ANV_FILE_OPEN, "&Open...\tCtrl-O");
  item->SetBitmap (bmFileOpen);
  fileMenu->Append (item);
  fileMenu->AppendSeparator();
  fileMenu->Append(ANV_FILE_SAVE, "&Save...\tCtrl-S" );
  fileMenu->Append(ANV_FILE_SAVEAS, "Save &As..." );
  fileMenu->AppendSeparator();
  fileMenu->Append(ANV_FILE_QUIT, "&Quit\tCtrl+Q");

  // edit menu
  wxMenu *eMenu = new wxMenu();
  eMenu->Append(ANV_EDIT_UNDO, "&Undo\tCtrl+Z");
  eMenu->Append(ANV_EDIT_REDO, "&Redo\tCtrl+Y");  

  // engine menu
  engineMenu = new wxMenu();
  engineMenu->Append(ANV_ENGINE_RELIGHT_ALL, "Relight All");
  engineMenu->Append(ANV_ENGINE_IMPORT_LIBRARY, "Import Library");

  menuBar = new wxMenuBar;
  menuBar->Append(fileMenu, "&File");
  menuBar->Append(eMenu, "&Edit");
  menuBar->Append(engineMenu, "&Engine");

  SetMenuBar(menuBar);
  // Done creating menu

  toolBar = CreateToolBar (wxTB_FLAT | wxTB_HORIZONTAL);
  toolBar->AddTool (ANV_FILE_OPEN, _("Open File"), bmFileOpen);
  toolBar->Realize ();

  CreateStatusBar(2);
  statusBar = GetStatusBar();
  statusBar->SetStatusText( "Anvil" );

  static const int widths[SFIELD_MAX] = { -1, 150, -4 };

  statusBar->SetFieldsCount(SFIELD_MAX);
  statusBar->SetStatusWidths(SFIELD_MAX, widths);

  wxRect feildRect;
  statusBar->GetFieldRect(SFIELD_GAUGE, feildRect);

  gauge = new wxGauge(statusBar, -1, 100, feildRect.GetPosition(), feildRect.GetSize(), wxGA_SMOOTH);

  panel3d = new wxPanel(this, -1, wxPoint(0, 0), wxSize(400, 400));

  ActivateAcceleratorTable();
}

anvMainFrame::~anvMainFrame()
{
}

void anvMainFrame::PushFrame () {
  csRef<iEventQueue> q ( csQueryRegistry<iEventQueue> (engine->GetObjectRegistry()));
  if (!q)
    return ;
  csRef<iVirtualClock> vc ( csQueryRegistry<iVirtualClock> (engine->GetObjectRegistry()));
  if (vc)
    vc->Advance();
  q->Process();
}

void anvMainFrame::OnWindowClose (wxCloseEvent& event)
{
  Destroy();
}  

void anvMainFrame::OnSize (wxSizeEvent& event)
{
  wxFrame::OnSize(event);

  int width, height;
  GetClientSize(&width, &height); 

  if (engine->wxwin && engine->wxwin->GetWindow())
    engine->wxwin->GetWindow()->SetSize (width, height);
}

void anvMainFrame::OnFileQuit (wxCommandEvent & event)
{
  Close(TRUE);
}

void anvMainFrame::OnFileNew (wxCommandEvent & event)
{
  // Clear the engine contents
}

void anvMainFrame::OnFileOpen (wxCommandEvent & event)
{
  cssVFSFileDlg dlg(this, -1, "Open Map File", wxDefaultPosition, wxDefaultSize, 
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, engine, "/", "/", VFS_OPEN );
  if(dlg.ShowModal())
  {
    SetStatusText("Loading map ...");
    // TODO: use gauge
    if(engine->LoadWorld((char*)dlg.GetPathData(), (char*)dlg.GetFileNameData()))
      SetStatusText("Map loaded successfully.");
    else
    {
      SetStatusText("Error loading map.");
      wxMessageBox("The mapfile couldn't be loaded.", "Error loading map.", wxICON_ERROR);
    }
  }
}

void anvMainFrame::OnFileSave ( wxCommandEvent & event )
{
  //Check dirty flag to see if map is already saved or not
  //If map already exists then skip showing dialog
  //and go directly to saving

  cssVFSFileDlg dlg(this, -1, "Save", wxDefaultPosition, wxDefaultSize, 
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, engine, "/", "/", VFS_SAVE );
  if(dlg.ShowModal())
  {
    engine->SaveWorld((char*)dlg.GetPathData(), (char*)dlg.GetFileNameData());
  }
}

void anvMainFrame::OnFileSaveAs ( wxCommandEvent & event )
{
  cssVFSFileDlg dlg(this, -1, "Save As", wxDefaultPosition, wxDefaultSize, 
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, engine, "/", "/", VFS_SAVE );
  if(dlg.ShowModal())
  {
    engine->SaveWorld((char*)dlg.GetPathData(), (char*)dlg.GetFileNameData());
  }
}

void anvMainFrame::OnEditUndo(wxCommandEvent & event)
{
  engine->Undo();
}

void anvMainFrame::OnEditRedo(wxCommandEvent & event)
{
  engine->Redo();
}

void anvMainFrame::ActivateAcceleratorTable()
{
  wxAcceleratorEntry entries[6];
  entries[0].Set(wxACCEL_CTRL,  (int) 'N', ANV_FILE_NEW);
  entries[1].Set(wxACCEL_CTRL,  (int) 'O', ANV_FILE_OPEN);
  entries[2].Set(wxACCEL_CTRL,  (int) 'S', ANV_FILE_SAVE);
  entries[3].Set(wxACCEL_CTRL,  (int) 'Q', ANV_FILE_QUIT);
  entries[4].Set(wxACCEL_CTRL,  (int) 'Z', ANV_EDIT_UNDO);
  entries[5].Set(wxACCEL_CTRL,  (int) 'Y', ANV_EDIT_REDO);
  wxAcceleratorTable accel(6, entries);
  SetAcceleratorTable(accel); 
}

void anvMainFrame::OnRelightAll (wxCommandEvent &event)
{
  csRef<iEngine> e =  csQueryRegistry<iEngine> (engine->GetObjectRegistry());
  e->ForceRelight();
}

void anvMainFrame::OnImportLibrary (wxCommandEvent &event)
{
   cssVFSFileDlg dlg(this, -1, "Import Library", wxDefaultPosition, wxDefaultSize, 
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER, engine, "/", "/", VFS_OPEN );
  if(dlg.ShowModal())
  {
    csRef<iLoader> loader =  csQueryRegistry<iLoader> (engine->GetObjectRegistry());
    csString s = dlg.GetPathData();
    s.Append(dlg.GetFileNameData());
    loader->LoadLibraryFile(s.GetData());
  }
}
