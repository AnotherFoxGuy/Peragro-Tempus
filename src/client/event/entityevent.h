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

#ifndef PTENTITY_EVENT_H
#define PTENTITY_EVENT_H

#include <cssysdef.h>
#include <csgeom/vector3.h>
#include <csutil/array.h>
#include <csutil/parray.h>

#include "client/event/event.h"

#include <vector>
#include <string>
#include <sstream>

#include "client/reporter/reporter.h"

namespace PT
{
  namespace Events
  {
    struct EntityHelper
    {
      struct EquipmentData
      {
        struct Equipment
        {
          unsigned int slotId;
          unsigned int itemId;
          unsigned int variation;
          std::string file;
          std::string mesh;
        };
        csArray<Equipment> equipment;

        ~EquipmentData()
        {
          equipment.DeleteAll();
        }

        void Add(unsigned int slotId, unsigned int itemId, unsigned int variation, const char* file, const char* mesh)
        {
          Equipment eq;
          eq.slotId = slotId;
          eq.itemId = itemId;
          eq.variation = variation;
          eq.file = file;
          eq.mesh = mesh;
          equipment.Push(eq);
        }

        size_t GetSize() { return equipment.GetSize(); }
        Equipment Get(size_t idx) { return equipment.Get(idx); }
      };

      static std::string MakeEntitySpecific(const char* eventname, unsigned int id)
      {
        std::string str;
        std::stringstream out;
        out << eventname << "." <<id;
        str = out.str();
        return str;
      }

      static EquipmentData* GetEquipment(const iEvent* ev)
      {
        const void* constEquipment = 0;
        size_t size = sizeof(EquipmentData);
        if (ev->Retrieve("equipment", constEquipment, size) != csEventErrNone)
        {
          Report(PT::Error, "EntityHelper::GetEquipment failed!");
          return 0;
        }

        EquipmentData* equipment = 0;
        void* temp = const_cast<void*>(constEquipment);
        equipment = static_cast<EquipmentData*>(temp);
        if (!equipment)
        {
          Report(PT::Error, "EntityHelper::GetEquipment failed!");
          return 0;
        }

        return equipment;
      }

      static void SetVector3(iEvent* ev, const char* name, csVector3 pos)
      {
        float tmp[3];
        tmp[0] = pos.x;
        tmp[1] = pos.y;
        tmp[2] = pos.z;
        SetVector3(ev, name, tmp);
      }

      static void SetVector3(iEvent* ev, const char* name, float* pos)
      {
        std::string nm = name;
        std::string nmX = nm + "_x";
        std::string nmY = nm + "_y";
        std::string nmZ = nm + "_z";

        float x, y, z;
        x = pos[0];
        y = pos[1];
        z = pos[2];

        ev->Add(nmX.c_str(), x);
        ev->Add(nmY.c_str(), y);
        ev->Add(nmZ.c_str(), z);
      }

      static csVector3 GetVector3(const iEvent* ev, const char* name)
      {
        std::string nm = name;
        std::string nmX = nm + "_x";
        std::string nmY = nm + "_y";
        std::string nmZ = nm + "_z";

        float x, y, z;
        x = y = z =0;

        if (ev->Retrieve(nmX.c_str(), x) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetVector3 failed! X attribute not present!");
        if (ev->Retrieve(nmY.c_str(), y) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetVector3 failed! Y attribute not present!");
        if (ev->Retrieve(nmZ.c_str(), z) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetVector3 failed! Z attribute not present!");

        csVector3 pos(x, y, z);
        return pos;
      }

      static void SetPosition(iEvent* ev, float* pos)
      {
        SetVector3(ev, "position", pos);
      }

      static csVector3 GetPosition(const iEvent* ev)
      {
        return GetVector3(ev, "position");
      }

      static unsigned int GetEntityID(const iEvent* event)
      {
        unsigned int id = -1;
        if (event->Retrieve("entityId", id) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetEntityID failed!");

        return id;
      }

      static unsigned int GetEntityType(const iEvent* event)
      {
        unsigned int type = -1;
        if (event->Retrieve("entityType", type) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetEntityType failed!");

        return type;
      }

      static unsigned int GetSectorId(const iEvent* event)
      {
        unsigned int sector = -1;
        if (event->Retrieve("sectorId", sector) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetSectorId failed!");

        return sector;
      }

      static std::string GetString(const iEvent* event, const char* name)
      {
        const char* str = "";
        if (event->Retrieve(name, str) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetString '%s' failed!", name);

        std::string text = str;
        return text;
      }


    };
  } // Events namespace
} // PT namespace

#endif // PTENTITY_EVENT_H
