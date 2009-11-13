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
#include "mesh.h"

#include <iutil/objreg.h>
#include <iutil/object.h>
#include <iengine/mesh.h>

#include <physicallayer/pl.h>
#include <physicallayer/propclas.h>
#include <propclass/mesh.h>
#include <propclass/linmove.h>

#include "common/eventcs/eventmanager.h"

#include "client/entity/entity.h"

#include "include/cacheentry.h"

#include "common/reporter/reporter.h"
#include "client/pointer/pointer.h"

PT_IMPLEMENT_COMPONENTFACTORY (Mesh, "peragro.entity.mesh")

ComponentMesh::ComponentMesh(iObjectRegistry* object_reg) :
  scfImplementationType (this, object_reg), iCacheUser(object_reg), object_reg(object_reg)
{
}

ComponentMesh::~ComponentMesh()
{
}

bool ComponentMesh::Initialize (PointerLibrary* pl, Client::Entity::Entity* ent)
{
  pointerlib = pl;
  entity = ent;

  // Create a default mesh.
  entity->SetMesh(0);

  return true;
} // end Initialize()

void ComponentMesh::Load()
{
  iCacheUser::Load(entity->GetFileName());
}

void ComponentMesh::Loaded(iCacheEntry* cacheEntry)
{
  if (cacheEntry->WasSuccessful())
  {
    csRef<iMeshWrapper> mesh = cacheEntry->Create(entity->GetName(), entity->GetMeshName());
    entity->SetMesh(mesh);
  }
  else
  {
    pointerlib->getReporter()->Report(PT::Error,  "ComponentMesh: Failed to load mesh: %s",
      entity->GetMeshName().c_str());
  }
} // end Loaded()
