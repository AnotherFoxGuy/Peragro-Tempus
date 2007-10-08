/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#include "playerentity.h"

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"

#include "client/event/eventmanager.h"
#include "client/event/interfaceevent.h"
#include "client/event/entityevent.h"

namespace PT
{

  namespace Entity
  {

    PlayerEntity::PlayerEntity(const Events::EntityAddEvent& ev) : PcEntity(ev)
    {
      Create();
    }

    PlayerEntity* PlayerEntity::Instance(const Events::EntityAddEvent* ev)
    {
    static PlayerEntity* instance=0;

    if (instance) return instance;
    if (!ev) return 0;

    instance = new PlayerEntity(*ev);
    return instance;
    }

    void PlayerEntity::Create()
    {
      csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

      //At this time PlayerEntity's Create() method takes care of the appropriate
      //stuff, so we only need to do this little thing.
      pl->CreatePropertyClass(celentity, "pccamera.old");
    }
  }

}
