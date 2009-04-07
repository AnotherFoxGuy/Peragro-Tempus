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

#ifndef ITEM_SPAWNER_H
#define ITEM_SPAWNER_H

#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <wfmath/point.h>

#include "common/util/timer.h"

class ItemEntity;


class ItemSpawner : public Timer
{
private:
  class SpawnPoint
  {
  public:
    boost::weak_ptr<ItemEntity> itemEntity;
    size_t itemTemplateId;
    WFMath::Point<3> position;
    size_t spawnInterval;
    size_t pickTime;

    SpawnPoint() :  itemTemplateId(0), spawnInterval(0), pickTime(0) {}
    ~SpawnPoint() {}
  };

  size_t timeCounter;

  std::vector<boost::shared_ptr<SpawnPoint> > spawnpoints;

  Mutex mutex;

  // Thread implementation
  void timeOut();
  void CheckSpawnPoint(boost::shared_ptr<SpawnPoint> sp);
  void AddSpawnPoint(size_t itemTemplateId, WFMath::Point<3> position, size_t spawnInterval);

public:
  ItemSpawner();
  ~ItemSpawner();

  void LoadFromDB();
  size_t GetSpawnPointCount() const { return spawnpoints.size(); }
  void CreateSpawnPoint(size_t itemTemplateId, WFMath::Point<3> position, size_t spawnInterval);
  void RemoveAllSpawnPoints();
};

#endif // ITEM_SPAWNER_H
