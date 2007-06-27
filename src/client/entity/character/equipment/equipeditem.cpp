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

#include "equipeditem.h"

#include "client/pointer/pointer.h"

EquipedItem::EquipedItem(PtEntity* entity, unsigned int itemid, unsigned int slotid)
{
  this->id         = itemid;
  this->slotid     = slotid;
  this->entity     = entity;
  this->itementity = 0;
}

void EquipedItem::ConstructMesh()
{
  if(itementity) return;
  if(!entity->GetCelEntity()) return;

  csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

  ClientItem* item = PointerLibrary::getInstance()->getItemManager()->GetItemById(id);

  csRef<iCelEntity> itement = pl->CreateEntity();
  this->itementity = itement;
  pl->CreatePropertyClass(itementity, "pcobject.mesh");
  csRef<iPcMesh> itempcmesh = CEL_QUERY_PROPCLASS_ENT(itementity, iPcMesh);

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcMesh);

  if(item)
  {
    itempcmesh->SetMesh(item->GetMeshName().GetData(), item->GetFileName().GetData());

    iMeshWrapper* mesh = itempcmesh->GetMesh();
    iMeshWrapper* parent = pcmesh->GetMesh();
    if(parent && mesh)
    {
      iSpriteCal3DSocket* socket = GetSocket(parent, slotid);
      if (socket)
      {
        mesh->QuerySceneNode()->SetParent(parent->QuerySceneNode ());
        socket->SetMeshWrapper(mesh);
        printf("EquipedItem: equipment attached!\n");
        return;
      }
    }
  }
  printf("EquipedItem: ERROR Failed to construct mesh %d!\n", id);
}

void EquipedItem::DestructMesh()
{
  if(!itementity) return;

  csRef<iObjectRegistry> obj_reg = PointerLibrary::getInstance()->getObjectRegistry();
  csRef<iCelPlLayer> pl =  csQueryRegistry<iCelPlLayer> (obj_reg);

  csRef<iPcMesh> pcmesh = CEL_QUERY_PROPCLASS_ENT(entity->GetCelEntity(), iPcMesh);

  iMeshWrapper* parent = pcmesh->GetMesh();
  if(parent)
  {
    iSpriteCal3DSocket* socket = GetSocket(parent, slotid);
    if (socket)
    {
      socket->SetMeshWrapper(0);
      pl->RemoveEntity(itementity);
      itementity = 0;
      printf("EquipedItem: equipment destroyed!\n");
      return;
    }
  }
  
  printf("EquipedItem: ERROR Failed to destruct mesh %d\n!", id);
}

iSpriteCal3DSocket* EquipedItem::GetSocket(iMeshWrapper* parent, unsigned int slotid)
{
  csString socketName;

  switch(slotid)
  {
  case 0:
    socketName = "righthand";
    break;
  default: printf("EquipedItem: Unknown slot with  ID %d !\n", slotid); return 0;
  }

  csRef<iSpriteCal3DState> sprcal3d = scfQueryInterface<iSpriteCal3DState> (parent->GetMeshObject());
  if (sprcal3d)
  {
    iSpriteCal3DSocket* socket = sprcal3d->FindSocket(socketName.GetData());
    if(socket)
      return socket;
    else
    {
      printf("EquipedItem: Unknown socket %s!\n", socketName.GetData());
      return 0;
    }
  }
  else
  {
    printf("EquipedItem: entity %s has no cal3d!\n", entity->GetName().GetData());
    return 0;
  }
}
