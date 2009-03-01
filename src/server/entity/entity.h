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

#include "common/entity/entity.h"

#include "common/util/monitorable.h"
#include <wfmath/point.h>
#include "common/util/ptstring.h"

#include "mesh.h"

class PcEntity;
class NpcEntity;
class ItemEntity;
class DoorEntity;
class MountEntity;

class Entity : public Common::Entity::Entity, public ptMonitorable<Entity>
{
public:
  static const unsigned int NoEntity = 0;

private:
  ptMonitor<PcEntity> pc_entity;
  ptMonitor<NpcEntity> npc_entity;
  ptMonitor<ItemEntity> item_entity;
  ptMonitor<DoorEntity> door_entity;
  ptMonitor<MountEntity> mount_entity;

protected:
  const Mesh* mesh;
  WFMath::Point<3> pos_last_saved;
  float rot_last_saved;

public:
  Entity(Common::Entity::EntityType type)
    : Common::Entity::Entity(type), mesh(0),
    pos_last_saved(0.0f), rot_last_saved(0.0f)
  {
  }

  virtual ~Entity();

  bool compare(const Entity* other) const
  {
    if (this == other)
      return true;

    if (this->type != other->type)
      return false;

    if (this->type == Common::Entity::ItemEntityType)
    {
      return this->id == other->id;
    }
    else if (this->type == Common::Entity::PlayerEntityType)
    {
      return (this->name == other->name);
    }

    return false;
  }

  ptString GetNameId () const { return ptString(name.c_str(), strlen(name.c_str())); }

  void resetSavePos() { pos_last_saved = GetPosition(); }
  WFMath::Point<3> getLastSaved() const { return pos_last_saved; }
  virtual void SetPosition(const WFMath::Point<3>& p);
  virtual void SetPosition(float x, float y, float z)
  { SetPosition(WFMath::Point<3>(x,y,z)); }

  const Mesh* getMesh() const { return mesh; }
  void setMesh(const Mesh* mesh) { this->mesh = mesh; }

  unsigned short GetSector() const;
  void SetSector(unsigned short id);

  virtual void SetSectorName(const std::string& value);

  float getDistanceTo(const WFMath::Point<3>& target) const
  { return WFMath::Distance<3>(GetPosition(), target); }
  float getDistanceTo(const Entity* target) const
  { return WFMath::Distance<3>(GetPosition(), target->GetPosition()); }
  float getDistanceTo2(const WFMath::Point<3>& target) const
  { return WFMath::SquaredDistance<3>(GetPosition(), target); }
  float getDistanceTo2(const Entity* target) const
  { return WFMath::SquaredDistance<3>(GetPosition(), target->GetPosition()); }

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
