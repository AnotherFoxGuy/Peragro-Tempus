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

#ifndef SPECIESMANAGER_H
#define SPECIESMANAGER_H

#include <string>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <wfmath/point.h>

#include "common/util/exception.h"

class Character;
class PcEntity;
class NpcEntity;

PT_DEFINE_EXCEPTION(InvalidSpeciesId);
PT_DEFINE_ERRORINFO(SpeciesId, size_t);
PT_DEFINE_EXCEPTION(InvalidSpeciesMesh);
PT_DEFINE_ERRORINFO(MeshId, size_t);

class SpeciesManager
{
private:

public:
  SpeciesManager();
  virtual ~SpeciesManager();

  WFMath::Point<3> GetRandomPosition(size_t speciesId);

  std::list<boost::shared_ptr<Character> > GetEntitiesOfSpecies(size_t speciesId);

  size_t GetCurrentPopulation(size_t speciesId);
  size_t GetMaxPopulation(size_t speciesId);

  void Initialize(boost::shared_ptr<Character> c, size_t speciesId);

  boost::shared_ptr<PcEntity> CreatePCFromSpecies(size_t speciesId);
  boost::shared_ptr<NpcEntity> CreateNPCFromSpecies(size_t speciesId);
};

#endif // SPECIESMANAGER_H
