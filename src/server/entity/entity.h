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
#include "common/geom/ptvector3.h"
#include "common/util/ptstring.h"

#include "mesh.h"

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
  ptString file_id;
  const Mesh* mesh;
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

  PtVector3 pos_last_saved;
  PtVector3 pos;

  float rot_last_saved;
  float rotation;

public:
  Entity(EntityType type)
    : id(0), name_id(0, 0), mesh(0), sector_id(0), type(type),
    pos_last_saved(0.0f), pos(0.0f), rot_last_saved(0.0f), rotation(0.0f)
  {
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
    pos_last_saved = pos;
  }
  PtVector3 getLastSaved() const { return pos_last_saved; }

  void setPos(float x, float y, float z) { setPos(PtVector3(x, y, z)); }
  void setPos(const PtVector3& p);
  PtVector3 getPos() const { return pos; }

  void setRotation(float rot) { rotation = rot; }
  float getRotation() const { return rotation; }

  const ptString& getName() const { return name_id; }
  void setName(ptString id) { name_id = id; }

  const Mesh* getMesh() const { return mesh; }
  void setMesh(const Mesh* mesh) { this->mesh = mesh; }

  const unsigned short getSector() const { return sector_id; }
  const ptString& getSectorName() const;
  void setSector(unsigned short id);
  void setSector(ptString name);

  EntityType getType() const { return type; }

  float getDistanceTo(const PtVector3& target) const
  { return Distance(pos, target); }
  float getDistanceTo(const Entity* target) const
  { return Distance(pos, target->getPos()); }
  float getDistanceTo2(const PtVector3& target) const
  { return Distance2(pos, target); }
  float getDistanceTo2(const Entity* target) const
  { return Distance2(pos, target->getPos()); }

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
