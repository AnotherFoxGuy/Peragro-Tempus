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

#ifndef SPECIES_ABILITIES_H
#define SPECIES_ABILITIES_H

#include <string.h>
#include <boost/shared_ptr.hpp>

#include "server/entity/statistic/abilities.h"

class SpeciesAbilities : Abilities
{
private:
  CharacterEntity* entity;
  boost::shared_ptr<Abilities> abilities;

  float GetAbilityXP(size_t speciesId, float age, const std::string& name);

public:
  SpeciesAbilities(CharacterEntity* entity);

  float Get(const std::string& name);
  void Set(const std::string& name, T xp);
  void Add(const std::string& name, T xp);
  void Sub(const std::string& name, T xp);
  size_t GetLevel(const std::string& name);

  void LoadFromDB();
  void SaveToDB();
};

#endif // SPECIES_ABILITIES_H
