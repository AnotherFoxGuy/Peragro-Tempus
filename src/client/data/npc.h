/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_DATANPC_H
#define PT_DATANPC_H

#include <map>
#include <string>
#include "common/util/ptvector3.h"

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about npc. Usually not
     * created directly, but accessed via NpcDataManager instance. Use the
     * Set/Get methods to access the npc properties.
     * @see NpcDataManager
     */
    class Npc
    {
    private:
      ///Name of the npc.
      std::string name;
      ///meshName - name of the mesh in the mesh file used for this npc.
      std::string meshName;
      ///Position of the npc in a sector (ie x='20', y='30', z='40').
      PtVector3 position;
      ///Name of the sector where the npc resides (ie 'room').
      std::string sectorName;
      
      std::string race;

      unsigned int hair[3];
      unsigned int skin[3];
      unsigned int decal[3];

      int dialog;

      std::string ai;

      std::map<std::string, std::string> setting;

      std::map<int, std::pair<int, int>> inventory;

    public:
      Npc() {}

      void SetName(const std::string& value) { name = value; }
      const std::string& GetName() const { return name; }

      void SetMeshName(const std::string& value) { meshName = value; }
      const std::string& GetMeshName() const { return meshName; }

      void SetPosition(double x,double y,double z)
      { position = PtVector3(x,y,z); }
      void SetPosition(const PtVector3& value) { position = value;}
      const PtVector3& GetPosition() const { return position; }

      void SetSectorName(const std::string& value) { sectorName = value; }
      const std::string& GetSectorName() const { return sectorName; }

      void SetRace(const std::string& value) { race = value; }
      const std::string& GetRace() const { return race; }

      void SetHairColor(unsigned int r, unsigned int g, unsigned int b)
      { hair[0] = r; hair[1] = g; hair[2] = b; }
      const unsigned int* GetHairColor() { return hair; }

      void SetSkinColor(unsigned int r, unsigned int g, unsigned int b)
      { skin[0] = r; skin[1] = g; skin[2] = b; }
      const unsigned int* GetSkinColor() { return skin; }

      void SetDecalColor(unsigned int r, unsigned int g, unsigned int b)
      { decal[0] = r; decal[1] = g; decal[2] = b; }
      const unsigned int* GetDecalColor() { return decal; }

      void SetDialog(unsigned int value) { dialog = value; }
      unsigned int GetDialog() const { return dialog; }

      void SetAi(const std::string& value) { ai = value; }
      const std::string& GetAi() const { return ai; }

      void SetSetting(const std::string& key, const std::string& value)
      { setting[key] = value; }
      const std::string& GetSetting(const std::string& key)
      { return setting[key]; }

      void SetInventory(int slot, int item, int variation)
      { inventory[slot] = std::pair<int, int>(item, variation); }
      const std::pair<int, int>& GetInventory(int slot)
      { return inventory[slot]; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATANPC_H
