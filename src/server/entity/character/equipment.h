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

#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <string>
#include <map>
#include <boost/shared_ptr.hpp>

#include "server/entity/itementity.h"

#include "common/util/exception.h"

class TableManager;
class Entity;
class EquipmentFactory;

PT_DEFINE_EXCEPTION(InvalidSlot);
PT_DEFINE_ERRORINFO(SlotId, size_t);
PT_DEFINE_EXCEPTION_WHAT(SlotNotFound, "Slot not found");
PT_DEFINE_ERRORINFO(SlotName, std::string);

class Equipment
{
private:
  EquipmentFactory* fact;
  Entity* entity;
  TableManager* db;

  std::map<size_t, boost::shared_ptr<ItemEntity> > equipment;
  typedef std::map<size_t, boost::shared_ptr<ItemEntity> >::iterator Iterator;

private:
  void SaveItemToDB(size_t slotId, boost::shared_ptr<ItemEntity> item);
  void DeleteItemFromDB(size_t slotId);

public:
  Equipment(EquipmentFactory* fact, Entity* entity, TableManager* db);

  boost::shared_ptr<ItemEntity> Equipped(size_t slotId);
  boost::shared_ptr<ItemEntity> Equipped(const std::string& slotName);

  void Equip(size_t slotId, boost::shared_ptr<ItemEntity> item);
  void Equip(const std::string& slotName, boost::shared_ptr<ItemEntity> item);

  boost::shared_ptr<ItemEntity> UnEquip(size_t slotId);
  boost::shared_ptr<ItemEntity> UnEquip(const std::string& slotName);

  void LoadFromDB();
  void SaveToDB();

  template<class T>
  void AddEquipment(T& msg)
  {
    msg.setEquipmentCount(equipment.size());
    Iterator it;
    size_t n = 0;
    for (it=equipment.begin(); it != equipment.end(); it++)
    {
      boost::shared_ptr<ItemEntity> item(it->second);

      msg.setSlotId(n, (unsigned char) it->first);
      msg.setItemId(n, item->GetId());

      msg.setFileName(n, item->GetFileName());
      msg.setMeshName(n, item->GetMeshName());

      //TODO
      //msg.setName(n, item->GetName());
      //msg.setIconName(n, item->GetIcon());
      //msg.setDescription(n, item->GetDescription());
      //msg.setWeight(n, item->GetWeight());
      //msg.setEquipType(n, item->GetEquipType()); //TODO
      n++;
    }
  }

};

class EquipmentFactory
{
private:
  std::map<std::string, size_t> slots;
  std::map<size_t, std::string> ids;

protected:
  TableManager* db;
  void LoadFromDB();
  void Add(size_t id, const std::string& name)
  {
    slots[name] = id;
    ids[id] = name;
  }

public:
  EquipmentFactory(TableManager* db);
  virtual ~EquipmentFactory() {}

  size_t GetSlotID(const std::string& name) const
  {
    std::map<std::string, size_t>::const_iterator it = slots.find(name);
    if (it == slots.end())
      throw PT_EX(SlotNotFound()) << SlotNameInfo(name);
    return it->second;
  }

  const std::string& GetSlotName(size_t id) const
  {
    std::map<size_t, std::string>::const_iterator it = ids.find(id);
    if (it == ids.end())
      throw PT_EX(SlotNotFound()) << SlotIdInfo(id);
    return it->second;
  }

  boost::shared_ptr<Equipment> Create(Entity* entity);
};

#endif // EQUIPMENT_H
