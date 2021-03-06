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

#ifndef ITEMTEMPLATESMANAGER_H
#define ITEMTEMPLATESMANAGER_H

#include <map>
#include <string>
#include <boost/shared_ptr.hpp>

#include "common/inventory/size.h"

#include "common/utility/exception.h"

class ItemEntity;

PT_DEFINE_EXCEPTION(InvalidItemTemplate);
PT_DEFINE_ERRORINFO(ItemTemplateId, size_t);
PT_DEFINE_ERRORINFO(EquipTypeId, size_t);
PT_DEFINE_ERRORINFO(MeshId, size_t);

class ItemTemplate
{
private:
  size_t templateId;

  virtual void LoadFromDB();
  virtual void SaveToDB();
  friend class ItemTemplatesManager;

public:
  std::string name;

  Common::Inventory::Size size;

  std::string description;
  std::string equipType;
  std::string equipTypeName;
  std::string meshName;
  std::string fileName;

public:
  ItemTemplate(size_t templateId) : templateId(templateId) {}
  virtual ~ItemTemplate() {}

  size_t GetTemplateId() { return templateId; }
  virtual void SetDataOn(ItemEntity* item);
};

class ItemTemplatesManager
{
private:
  std::map<size_t, boost::weak_ptr<ItemTemplate> > templates;

public:
  ItemTemplatesManager() {}
  ~ItemTemplatesManager() {}

  boost::shared_ptr<ItemTemplate> Get(size_t templateId);

  boost::shared_ptr<ItemEntity> CreateItemFromTemplate(size_t templateId);
};

#endif // ITEMTEMPLATESMANAGER_H
