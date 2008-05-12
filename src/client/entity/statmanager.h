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

#ifndef STATMANAGER_H
#define STATMANAGER_H

#include <vector>
#include <string>
#include "client/event/eventmanager.h"

namespace PT
{
  namespace Entity
  {

    struct Stat{
      std::string name;
      int level;
      int id;
      unsigned int entityId;
    };

    class StatManager
    {
    private:
      std::vector<Stat> stats;

    public:
      StatManager ();
      ~StatManager ();

      bool Initialize();
      bool AddStat(iEvent& ev);
      int GetStat(const char* name);
      bool UpdateStat(iEvent& ev);

    };

  } // Entity namespace
} // PT namespace

#endif // STATMANAGER_H
