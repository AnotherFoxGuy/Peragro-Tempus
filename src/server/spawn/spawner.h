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

#ifndef SPAWNER_H
#define SPAWNER_H

#include "server/spawn/itemspawner.h"
#include "server/spawn/creaturespawner.h"

class Spawner
{
private:
  ItemSpawner* itemSpawner;
  CreatureSpawner* creatureSpawner;

public:
  Spawner()
  { 
    itemSpawner = new ItemSpawner();
    itemSpawner->LoadFromDB();
    itemSpawner->start();

    creatureSpawner = new CreatureSpawner();
    //creatureSpawner->LoadFromDB();
    creatureSpawner->start();
  }

  ~Spawner() 
  {
    itemSpawner->stop();
    creatureSpawner->stop();
    delete itemSpawner;
    delete creatureSpawner;
  }
};

#endif // SPAWNER_H
