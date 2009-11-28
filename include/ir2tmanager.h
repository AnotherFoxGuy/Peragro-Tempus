/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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


#ifndef _H_IR2TMANAGER
#define _H_IR2TMANAGER

#include <cssysdef.h>
#include <csutil/scf.h>

struct iMeshWrapper;

namespace CEGUI
{
  class Window;
}

struct iR2TWidgetManager : public virtual iBase
{
  SCF_INTERFACE(iR2TWidgetManager,1,0,0);

  virtual bool Register (CEGUI::Window*, iMeshWrapper*) = 0;

  virtual bool UnRegister (CEGUI::Window*) = 0;
};

#endif
// _H_IR2TMANAGER
