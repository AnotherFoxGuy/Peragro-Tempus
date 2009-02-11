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

#ifndef IPOINTERPLUG_H
#define IPOINTERPLUG_H

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include <iutil/plugin.h>
#include <iutil/objreg.h>

#define PT_POINTERLIBRARY_PLUGNAME "peragro.pointerlibrary"

class PointerLibrary;

struct iPointerPlug : public virtual iBase
{
  SCF_INTERFACE(iPointerPlug, 1,0,0);

  /** Returns a reference to the client's PointerLibrary instance */
  virtual PointerLibrary* getPointerLibrary() const = 0;

};

namespace PT
{

  inline PointerLibrary* getPointerLibrary (iObjectRegistry* objreg)
  {
    csRef<iPointerPlug> ptrplug = csLoadPluginCheck<iPointerPlug> (objreg, PT_POINTERLIBRARY_PLUGNAME);

    if (!ptrplug) return NULL;
    return ptrplug->getPointerLibrary();
  }

}; // PT namespace

#endif // IPOINTERPLUG_H
