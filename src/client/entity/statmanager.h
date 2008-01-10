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
#include "client/event/eventmanager.h"

namespace PT
{
  namespace Entity
  {

    struct Stat{
      const char* name;
      int level;
    };

    class StatManager
    {
    private:
      std::vector<Stat> stats;

    public:
      StatManager ();
      ~StatManager ();

      bool Initialize();
      bool AddStat(PT::Events::Eventp ev);
      int GetStat(const char* name);

    };

  } // Entity namespace
} // PT namespace

#endif // STATMANAGER_H
