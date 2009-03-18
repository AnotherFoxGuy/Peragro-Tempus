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

class Tables;

#define SAFEMUTABLE(funcname, type)         \
virtual void funcname(type value)           \
{                                           \
  mutex.lock();                             \
  Common::Entity::Entity::funcname(value);  \
  mutex.unlock();                           \
}                                           \

class Entity : public Common::Entity::Entity
{
private:
  Mutex mutex;

protected:
  WFMath::Point<3> pos_last_saved;
  float rot_last_saved;

public:
  Entity(Common::Entity::EntityType type)
    : Common::Entity::Entity(type),
    pos_last_saved(0.0f), rot_last_saved(0.0f)
  {
  }

  virtual ~Entity();

  void resetSavePos() { pos_last_saved = GetPosition(); }
  WFMath::Point<3> getLastSaved() const { return pos_last_saved; }

/*
  unsigned short GetSector() const;
  void SetSector(unsigned short id);
*/
  virtual void SetSectorName(const std::string& value);

  SAFEMUTABLE(SetId, unsigned int)
  SAFEMUTABLE(SetName, const std::string&)
  SAFEMUTABLE(SetMeshName, const std::string&)
  SAFEMUTABLE(SetFileName, const std::string&)
  SAFEMUTABLE(SetPosition, const WFMath::Point<3>&)
  virtual void SetPosition(float x, float y, float z)
  { SetPosition(WFMath::Point<3>(x,y,z)); }
  SAFEMUTABLE(SetRotation, float)

  float getDistanceTo(const WFMath::Point<3>& target) const
  { return WFMath::Distance<3>(GetPosition(), target); }
  float getDistanceTo(Entity* target) const
  { return WFMath::Distance<3>(GetPosition(), target->GetPosition()); }
  float getDistanceTo2(const WFMath::Point<3>& target) const
  { return WFMath::SquaredDistance<3>(GetPosition(), target); }
  float getDistanceTo2(Entity* target) const
  { return WFMath::SquaredDistance<3>(GetPosition(), target->GetPosition()); }

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // ENTITY_H
