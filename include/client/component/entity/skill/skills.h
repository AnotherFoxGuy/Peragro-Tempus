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

#ifndef ICOMPONENT_SKILLS_H
#define ICOMPONENT_SKILLS_H

#include <csutil/scf.h>
#include <csutil/scf_implementation.h>

#include "include/client/component/component.h"

class PointerLibrary;

namespace Client
{
  namespace Entity
  {
    class Entity;
  }
} //Client namespace


struct iSkills : public virtual iBase
{
    SCF_INTERFACE(iSkills, 1,0,0);

    virtual bool Initialize(PointerLibrary*, Client::Entity::Entity*) = 0;

    virtual size_t GetSkillLevel(const std::string& name) = 0;
};


#endif // ICOMPONENT_SKILLS_H
