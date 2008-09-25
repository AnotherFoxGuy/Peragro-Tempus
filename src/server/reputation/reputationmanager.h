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

#ifndef REPUTATIONMANAGER_H
#define REPUTATIONMANAGER_H

#include "common/util/array.h"

class Reputation;
class ptString;
class ReputationsTable;

class ReputationManager
{
private:
  Array<Reputation*> reputations;

public:
  ReputationManager() {}

  ~ReputationManager() { reputations.delAll(); }

  size_t getReputationCount();

  Reputation* getReputation(size_t index);

  void addReputation(Reputation* reputation);

  Reputation* addReputation(ptString name);

  void delReputation(Reputation* reputation);

  bool exists(Reputation* reputation);

  Reputation* findByName(ptString name);

  Reputation* findById(int id);

  void loadFromDB(ReputationsTable* rt);

  void clear();

};

#endif // REPUTATIONMANAGER_H
