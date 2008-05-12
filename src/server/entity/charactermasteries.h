/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#ifndef CHARACTERMASTERIES_H_
#define CHARACTERMASTERIES_H_

#include "charactermastery.h"
#include "server/database/table-charactermasteries.h"
#include "common/util/monitorable.h"

class Connection;

class CharacterMasteries : public ptMonitorable<CharacterMasteries>
{
private:
  Array<CharacterMastery*> masteries;


  CharacterMasteriesTable* cmtab;

  int cm_id;

public:
  CharacterMasteries() : cmtab(0) {}
  ~CharacterMasteries() { masteries.delAll(); }
  CharacterMastery* findEntry(size_t masteryID);

  /**
   *
   * Given a CharacterMasteriesTable it loads all the masteries.
   *
   * @param cmt The CharacterMasteriesTable to load from.
   * @param masteryID The id of the mastery.
   *
   */
  void loadFromDatabase(CharacterMasteriesTable* cmt, int masteryID)
  {
    cm_id = masteryID;
    cmtab = cmt;

    //Load all Skills from Database
    Array<CharacterMasteryTableVO*> vos = cmt->getAllMasteries(masteryID);
    for (size_t i = 0; i < vos.getCount(); i++)
    {
      CharacterMasteryTableVO* vo = vos.get(i);

      CharacterMastery* mastery = new CharacterMastery();
      mastery->masteryID = vo->masteryID;
      mastery->masteryType = vo->masteryType;
      mastery->masteryLevel = vo->masteryLevel;

      masteries.add(mastery);
    }

  }

  void sendAllMasteries(Connection* conn);
};

#endif // CHARACTERMASTERIES_H_
