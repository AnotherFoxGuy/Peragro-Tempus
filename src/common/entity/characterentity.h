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

#ifndef _CHARACTERENTITY_H_
#define _CHARACTERENTITY_H_

#include <string.h>
#include <time.h>
#include <math.h>

#include "common/util/stringstore.h"

#include "inventory.h"
#include "characterstats.h"

class Item;

class CharacterEntity : public Entity
{
private:
  Inventory inventory;
  CharacterStats stats;

  float final_dst[3];
  size_t t_stop;

  float tmp_pos[3]; //used only for temporary calculations!

public:

  enum State
  {
    // My second most favorit activity, right after 'Resting'
    stIdle=0,

    // Walking, you can't walk and fight/cast.
    stMoving=1,

    // Hitting with weapon or meele
    stAttacking=2,

    // Casting a spell, can be interruped when character gets distracted (attacked).
    stCasting=3,

    // Recovering faster, when sitting or so.
    // You have to change to idle before to any other state.
    stResting=4,

    // Following: Like Moving' but stops when near target and resumes when target moves.
    stFollowing=5,

    // Well... you're dead, you can't move, speak or act in any other way.
    // don't get confused about zombies, they are 'undead' not dead! :)
    stDead=6
  };

protected:
  State state;

  CharacterEntity(EntityType type) : Entity(type) 
  {
    final_dst[0] = 0;
    final_dst[1] = 0;
    final_dst[2] = 0;
    t_stop = 0;
  }

public:
  Inventory* getInventory()
  {
    return &inventory;
  }

  CharacterStats* getStats()
  {
    return &stats;
  }

  State getState()
  {
    return state;
  }

  void setState(State st)
  {
    state = st;
  }

  void walkTo(float* dst_pos, float speed)
  {
    final_dst[0] = dst_pos[0];
    final_dst[1] = dst_pos[1];
    final_dst[2] = dst_pos[2];

    float dist_x = fabsf(final_dst[0] - pos[0]);
    float dist_y = fabsf(final_dst[1] - pos[1]);
    float dist_z = fabsf(final_dst[2] - pos[2]);
    float dist = sqrtf(dist_x*dist_x + dist_y*dist_y + dist_z*dist_z);

    //v = s / t => t = s / v
    t_stop = (size_t) (dist / speed + time(0));

    state = stMoving;
  }

  float* getPos()
  {
    if (state != stMoving)
    {
      return pos;
    }
    else
    {
      if (time(0) >= t_stop)
      {
        setPos(final_dst);
        state = stIdle;
        return final_dst;
      }
      else
      {
        //Not sure that's correct...
        size_t delta = t_stop - (size_t) time(0);
        tmp_pos[0] = (final_dst[0] - pos[0]) * delta;
        tmp_pos[1] = (final_dst[1] - pos[1]) * delta;
        tmp_pos[2] = (final_dst[2] - pos[2]) * delta;
        return tmp_pos;
      }
    }
  }

};

#endif // _CHARACTERENTITY_H_
