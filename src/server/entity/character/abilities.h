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

#ifndef ABILITIES_H
#define ABILITIES_H

#include <string.h>
#include <boost/shared_ptr.hpp>

#include "base.h"

class AbilitiesFactory;

class Abilities : public Bases<10, float, 10>
{
public:
  Abilities(AbilitiesFactory* fact, Entity* entity, TableManager* db);

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

class AbilitiesFactory : public BasesFactory
{
private:
  virtual void LoadFromDB();

public:
  AbilitiesFactory(TableManager* db);
  virtual ~AbilitiesFactory() {}

  boost::shared_ptr<Abilities> Create(Entity* entity);
};

#endif // ABILITIES_H
