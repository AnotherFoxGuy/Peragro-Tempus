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

/*
  the pointer plugin is a pseudo-plugin (meaning, it registers itself
  from the main app, rather than being loaded as a plugin), which lets
  other plugins interface with the PointerLibrary, by being able to 
  request it from the object registry.
*/

#ifndef POINTERPLUG_H
#define POINTERPLUG_H

#include <iutil/comp.h>
#include "include/ipointerplug.h"

class PointerLibrary;

class PointerPlug : public scfImplementation2<PointerPlug, iPointerPlug, iComponent>
{
private:
  iObjectRegistry* object_reg;

public:
  PointerPlug(iBase* parent);
  virtual ~PointerPlug();

  // from iComponent.
  virtual bool Initialize (iObjectRegistry*);

  // from iPointerPlug.
  virtual PointerLibrary* getPointerLibrary () const;

};

#endif // POINTERPLUG_H
