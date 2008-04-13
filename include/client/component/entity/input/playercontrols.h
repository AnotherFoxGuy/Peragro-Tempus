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

#ifndef IPLAYERCONTROLS_H
#define IPLAYERCONTROLS_H

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include "include/client/component/component.h"

class PointerLibrary;

namespace PT
{
  namespace Entity
  {
    class Entity;
  }
} //PT namespace

struct iPlayerControls : public virtual iBase
{
    SCF_INTERFACE(iPlayerControls, 1,0,0);

    virtual bool Initialize(PointerLibrary*, PT::Entity::Entity*) = 0;

};


#endif // IPLAYERCONTROLS_H