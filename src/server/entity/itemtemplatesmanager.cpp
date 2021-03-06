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

#include "itemtemplatesmanager.h"

#include "server/server.h"
#include "server/database/tablemanager.h"
#include "server/database/table-itemtemplates.h"
#include "server/database/table-meshes.h"
#include "server/database/table-equiptypes.h"

#include "common/util/printhelper.h"

#include "itementity.h"

void ItemTemplate::LoadFromDB()
{
  ItemTemplatesTable* ttable =
    Server::getServer()->GetTableManager()->Get<ItemTemplatesTable>();

  ItemTemplatesTableVOp it = ttable->GetSingle(templateId);
  if (!it)
  {
    throw PT_EX(InvalidItemTemplate("Invalid item template"))
      << ItemTemplateIdInfo(templateId);
  }

  name = it->name;
  size_t meshId = it->meshes_id;

  size = it->size;

  description = it->description;
  size_t equipTypeId = it->equipType_id;

  EquipTypesTable* etable =
    Server::getServer()->GetTableManager()->Get<EquipTypesTable>();

  EquipTypesTableVOp e = etable->GetSingle(equipTypeId);
  if (!e)
  {
    throw PT_EX(InvalidItemTemplate("Invalid equip type for item template"))
      << EquipTypeIdInfo(equipTypeId) << ItemTemplateIdInfo(templateId);
  }
  equipType = e->slotName;
  equipTypeName = e->name;

  MeshesTable* mtable = Server::getServer()->GetTableManager()->Get<MeshesTable>();
  MeshesTableVOp m = mtable->GetSingle(meshId);
  if (!e)
  {
    throw PT_EX(InvalidItemTemplate("Invalid mesh for item template"))
      << MeshIdInfo(meshId) << ItemTemplateIdInfo(templateId);
  }
  meshName = m->factoryName;
  fileName = m->fileName;
}

void ItemTemplate::SaveToDB()
{
}

void ItemTemplate::SetDataOn(ItemEntity* item)
{
  item->SetName(name);
  item->SetSize(size);

  item->SetDescription(description);
  //item->SetWeight(weight);
  item->SetEquipType(equipType);
  item->SetEquipTypeName(equipTypeName);
  item->SetMeshName(meshName);
  item->SetFileName(fileName);
}

boost::shared_ptr<ItemTemplate> ItemTemplatesManager::Get(size_t templateId)
{
  std::map<size_t, boost::weak_ptr<ItemTemplate> >::const_iterator it;
  it = templates.find(templateId);
  if (it == templates.end() || it->second.expired())
  {
    boost::shared_ptr<ItemTemplate> t(new ItemTemplate(templateId));
    t->LoadFromDB();
    templates[templateId] = t;
    return t;
  }
  else
    return it->second.lock();
}

boost::shared_ptr<ItemEntity> ItemTemplatesManager::CreateItemFromTemplate(size_t templateId)
{
  Entityp entity = Server::getServer()->getEntityManager()
    ->CreateNew(Common::Entity::ItemEntityType);

  boost::shared_ptr<ItemEntity> item =
    boost::shared_polymorphic_downcast<ItemEntity>(entity);

  boost::shared_ptr<ItemTemplate> temp =
    Server::getServer()->GetItemTemplatesManager()->Get(templateId);

  item->SetItemTemplate(temp);
  return item;
}
