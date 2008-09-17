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

#ifndef ENTITY_H
#define ENTITY_H

#include <math.h>

#include "common/util/monitorable.h"
#include "common/util/ptstring.h"

class PcEntity;
class NpcEntity;
class ItemEntity;
class DoorEntity;
class MountEntity;

class Entity : public ptMonitorable<Entity>
{
public:
  static const unsigned int NoEntity = 0;

private:
  unsigned int id;

  ptString name_id;
  ptString mesh_id;
  unsigned short sector_id;

  ptMonitor<PcEntity> pc_entity;
  ptMonitor<NpcEntity> npc_entity;
  ptMonitor<ItemEntity> item_entity;
  ptMonitor<DoorEntity> door_entity;
  ptMonitor<MountEntity> mount_entity;

public:
  enum EntityType
  {
    PlayerEntityType=0,
    NPCEntityType=1,
    DoorEntityType=2,
    ItemEntityType=3,
    MountEntityType=4
  };

protected:
  EntityType type;

  float pos_last_saved[3];
  float pos[3];

  float rot_last_saved;
  float rotation;

public:
  Entity(EntityType type)
    : id(0), name_id(0, 0), mesh_id(0, 0), sector_id(0), type(type),
    rot_last_saved(0.0f), rotation(0.0f)
  {
    pos[0] = 0.0f;
    pos[1] = 0.0f;
    pos[2] = 0.0f;

    pos_last_saved[0] = 0.0f;
    pos_last_saved[1] = 0.0f;
    pos_last_saved[2] = 0.0f;
  }

  virtual ~Entity();

  bool compare(const Entity* other) const
  {
    if (this == other)
      return true;

    if (this->type != other->type)
      return false;

    if (this->type == Entity::ItemEntityType)
    {
      return this->id == other->id;
    }
    else if (this->type == Entity::PlayerEntityType)
    {
      return (this->name_id == other->name_id);
    }

    return false;
  }

  void setId(unsigned int id) { this->id = id; }
  unsigned int getId() const { return id; }

  void resetSavePos()
  {
    pos_last_saved[0] = pos[0];
    pos_last_saved[1] = pos[1];
    pos_last_saved[2] = pos[2];
  }
  const float* getLastSaved() const { return pos_last_saved; }

  void setPos(const float p[3]) { setPos(p[0],p[1],p[2]); }
  void setPos(float x, float y, float z);
  const float* getPos() const { return pos; }

  void setRotation(float rot) { rotation = rot; }
  float getRotation() const { return rotation; }

  const ptString& getName() const { return name_id; }
  void setName(ptString id) { name_id = id; }

  const ptString& getMesh() const { return mesh_id; }
  void setMesh(ptString id) { mesh_id = id; }

  const unsigned short getSector() const { return sector_id; }
  const ptString& getSectorName() const;
  void setSector(unsigned short id);
  void setSector(ptString name);

  EntityType getType() const { return type; }

  float getDistanceTo2(const float* target) const
  {
    return (target[0] - pos[0])*(target[0] - pos[0])
         + (target[1] - pos[1])*(target[1] - pos[1])
         + (target[2] - pos[2])*(target[2] - pos[2]);
  }

  float getDistanceTo2(const Entity* target) const
  {
    return getDistanceTo2(target->getPos());
  }

  float getDistanceTo(const Entity* target) const
  {
    return sqrtf(getDistanceTo2(target));
  }

  const PcEntity* getPlayerEntity() const { return pc_entity.get(); }
  const NpcEntity* getNpcEntity() const { return npc_entity.get(); }
  const ItemEntity* getItemEntity() const { return item_entity.get(); }
  const DoorEntity* getDoorEntity() const { return door_entity.get(); }
  const MountEntity* getMountEntity() const { return mount_entity.get(); }

  void setPlayerEntity(const PcEntity*);
  void setNpcEntity(const NpcEntity*);
  void setItemEntity(const ItemEntity*);
  void setDoorEntity(const DoorEntity*);
  void setMountEntity(const MountEntity*);
};

#endif // ENTITY_H
