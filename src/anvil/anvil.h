/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#ifndef __ANVIL_H__
#define __ANVIL_H__

#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "mainframe.h"

struct iObjectRegistry;

// Define a new application type
class AnvilApp: public wxApp
{
public:
  iObjectRegistry* object_reg;
  anvMainFrame* frame;

  virtual bool OnInit(void);
  virtual int OnExit(void);

#ifdef USE_IDLE
  virtual void OnIdle();
  DECLARE_EVENT_TABLE();
#endif
};


#endif // __ANVIL_H__
