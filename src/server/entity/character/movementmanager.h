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

#ifndef MOVEMENTMANAGER_H
#define MOVEMENTMANAGER_H

#include <string>
#include <map>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include "common/util/timer.h"

class Character;

class MovementManager : public Timer
{
private:
  typedef std::map<boost::weak_ptr<Character>, size_t> EntityList;
  EntityList entities;

private:
  void timeOut();
  bool Degenerate(boost::shared_ptr<Character> entity, size_t elapsedTime);

public:
  MovementManager();
  virtual ~MovementManager() {}

  void Register(boost::shared_ptr<Character>);
  void UnRegister(boost::weak_ptr<Character>);

  float GetMovementSpeed(boost::shared_ptr<Character> entity);
};

#endif // MOVEMENTMANAGER_H
