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

#ifndef RACE_H
#define RACE_H

#include <string.h>
#include <time.h>
#include <math.h>

#include "common/util/stringstore.h"

#include "racestats.h"
#include "raceskills.h"
#include "mesh.h"

class Race
{
private:

  int id;

  ptString name_id;

  const Mesh* mesh;
  //start sector
  ptString sector_id;
  //start pos
  WFMath::Point<3> pos;

  RaceStats stats;
  RaceSkills skills;

public:
  Race() : id(-1)
  {
  }

  ~Race()
  {
  }

  bool compare(Race* other)
  {
    if (this == other)
      return true;

    return (this->name_id == other->name_id);
  }

  void setId(int id)
  {
    this->id = id;
  }
  int GetId()
  {
    return id;
  }

  void SetPosition(const WFMath::Point<3>& p) { pos = p; }
  WFMath::Point<3> GetPosition() { return pos; }

  ptString& getName() { return name_id; }
  void setName(ptString name) { name_id = name; }

  const Mesh* getMesh() const { return mesh; }
  void setMesh(const Mesh* mesh) { this->mesh = mesh; }

  ptString& GetSector() { return sector_id; }
  void SetSector(ptString name) { sector_id = name; }

  RaceStats* getStats() { return &stats; }
  RaceSkills* getSkills() { return &skills; }
};

#endif // RACE_H
