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
/**
 * @file entityevent.h
 *
 * @brief Helper classes for entity events.
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
    /**
     * @ingroup events
     * Provides helper functions for entity events.
     */
    struct EntityHelper
    {
      /**
       * Provides helper functions for equipment data.
       */
      struct EquipmentData
      {
        /**
         * Structure to store equipment data passed with an entity event.
         */
        struct Equipment
        {
          /// The slot ID.
          unsigned int slotId;
          /// The item ID.
          unsigned int itemId;
          /// The variation ID.
          unsigned int variation;
          /// The file name.
          std::string file;
          /// The mesh name.
          std::string mesh;
        };
        /// The array of equipment.
        csArray<Equipment> equipment;

        /// Destructor.
        ~EquipmentData()
        {
          equipment.DeleteAll();
        }

        /**
         * Add an item of equipment to the array.
         * @param slotId The slot ID.
         * @param itemId The item ID.
         * @param variation The variation ID.
         * @param file The file name.
         * @param mesh The mesh name.
         */
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

        /// Returns the size of the equipment array.
        size_t GetSize() { return equipment.GetSize(); }
        /// Returns the equipment item at the index from the array.
        Equipment Get(size_t idx) { return equipment.Get(idx); }
      };

      /**
       * Returns a string with the event name and an ID appended.
       * @param eventname The event name.
       * @param id The event ID.
       * @return A string with the name, a period, then the ID.
       */
      static std::string MakeEntitySpecific(const char* eventname, unsigned int id)
      {
        std::string str;
        std::stringstream out;
        out << eventname << "." <<id;
        str = out.str();
        return str;
      }

      /**
       * Create an equipment data stucture from an entity event.
       * @param ev An entity event.
       * @return The equipment data.
       */
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

      /**
       * Add coordinates to an event parameter, as a csVector3.
       * @param ev An entity event.
       * @param name The name of the event parameter to add the coordinates to.
       * @param pos The coordinates.
       */
      static void SetVector3(iEvent* ev, const char* name, csVector3 pos)
      {
        float tmp[3];
        tmp[0] = pos.x;
        tmp[1] = pos.y;
        tmp[2] = pos.z;
        SetVector3(ev, name, tmp);
      }

      /**
       * Add coordinates to an event parameter, as a float pointer.
       * @param ev An entity event.
       * @param name The name of the event parameter to add the coordinates to.
       * @param pos The coordinates.
       */
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

      /**
       * Get coordinates from an event parameter.
       * @param ev An entity event.
       * @param name The name of the event parameter to get the coordinates from.
       * @return The coordinates.
       */
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

      /**
       * Set the position for an entity event.
       * @param ev An entity event.
       * @param pos The coordinates.
       */
      static void SetPosition(iEvent* ev, float* pos)
      {
        SetVector3(ev, "position", pos);
      }

      /**
       * Get the position from an entity event.
       * @param ev An entity event.
       * @return The coordinates.
       */
      static csVector3 GetPosition(const iEvent* ev)
      {
        return GetVector3(ev, "position");
      }

      /**
       * Get the entity's ID from an event.
       * @param ev An entity event.
       * @return The entity's ID.
       */
      static unsigned int GetEntityID(const iEvent* event)
      {
        unsigned int id = -1;
        if (event->Retrieve("entityId", id) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetEntityID failed!");

        return id;
      }

      /**
       * Get the entity's type from an event.
       * @param ev An entity event.
       * @return The entity's type.
       */
      static unsigned int GetEntityType(const iEvent* event)
      {
        unsigned int type = -1;
        if (event->Retrieve("entityType", type) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetEntityType failed!");

        return type;
      }

      /**
       * Get the entity's sector ID from an event.
       * @param ev An entity event.
       * @return The entity's sector ID.
       */
      static unsigned int GetSectorId(const iEvent* event)
      {
        unsigned int sector = -1;
        if (event->Retrieve("sectorId", sector) != csEventErrNone)
          Report(PT::Error, "EntityHelper::GetSectorId failed!");

        return sector;
      }

      /**
       * Get an entity event parameter's value.
       * @param ev An entity event.
       * @param name The name of the parameter.
       * @return The parameter's value.
       */
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
