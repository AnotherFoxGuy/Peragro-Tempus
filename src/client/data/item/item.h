/*
    Copyright (C) 2007-2008 Development Team of Peragro Tempus

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
 * @file item.h
 *
 * @basic Helper class that contains basic information about item type.
 */

#ifndef PT_DATAITEM_H
#define PT_DATAITEM_H

#include <string>

namespace PT
{
  namespace Data
  {

    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about item type. Usually not
     * created directly, but accessed via ItemDataManager instance. Use the
     * Set/Get methods to access the item properties.
     * @see ItemDataManager
     */
    class Item
    {
    private:
      /// Unique ID of the item type (ie '3').
      unsigned int id;
      /// Name of the item (ie 'apple').
      std::string name;
      /// name of the icon used for the item in inventory view (ie
      /// Set:Inventory image:apple').
      std::string icon;
      /// Description of the item type (ie 'A kind of fruit').
      std::string description;
      /// relative path to the mesh file containing the item mesh
      /// (ie '/peragro/meshes/apple/apple.xml').
      std::string meshFile;
      /// meshName - name of the mesh in the mesh file used for this item (ie
      /// 'apple').
      std::string meshName;
      /// Weight of a single item in kilograms (ie '0.1').
      float weight;
      /// Type of item.
      std::string itemtype;
      /// Where the item can be equipped. (for example 'weapon2h' for a
      /// 2-handed weapon).
      std::string equiptype;
      /// The distance the attacker can stand from his target and make a
      /// non-ranged attack with this weapon.
      float reach;
      /// Base damage.
      float damage;
      /// The quality of the item.
      float quality;
      /// Related to the force needed to swing the item.
      float heft;
      /// This trait is set at the time the item is created and steadily erodes
      /// from there.
      float durability;
      /// For ranged weapons.
      float pull;
      /// For ranged weapons.
      float range;
      /// For ammo.
      float amount;
      /// For armour.
      float mobility;
      /// Hardness.
      float hardness;
      /// For armour.
      float bulk;
      /// For armour.
      float toughness;

    public:
      /**
       * Constructor.
       */
      Item() : id(0), weight(0) {}

      void SetId(unsigned int value) { id = value; }
      unsigned int GetId() const { return id; }

      void SetName(const std::string& value) { name = value; }
      const std::string& GetName() const { return name; }

      void SetIconName(const std::string& value) { icon = value; }
      const std::string& GetIconName() const { return icon; }

      void SetDescription(const std::string& value) { description = value; }
      const std::string& GetDescription() const { return description; }

      void SetMeshFile(const std::string& value) { meshFile = value; }
      const std::string& GetMeshFile() const { return meshFile; }

      void SetMeshName(const std::string& value) { meshName = value; }
      const std::string& GetMeshName() const { return meshName; }

      void SetWeight(float value) { weight = value; }
      float GetWeight() const { return weight; }

      void SetItemtype(const std::string& value) { itemtype = value; }
      const std::string& GetItemtype() const { return itemtype; }

      void SetEquiptype(const std::string& value) { equiptype = value; }
      const std::string& GetEquiptype() const { return equiptype; }

      void SetReach(float value) { reach = value; }
      float GetReach() const { return reach; }

      void SetDamage(float value) { damage = value; }
      float GetDamage() const { return damage; }

      void SetQuality(float value) { quality = value; }
      float GetQuality() const { return quality; }

      void SetHeft(float value) { heft = value; }
      float GetHeft() const { return heft; }

      void SetDurability(float value) { durability = value; }
      float GetDurability() const { return durability; }

      void SetPull(float value) { pull = value; }
      float GetPull() const { return pull; }

      void SetRange(float value) { range = value; }
      float GetRange() const { return range; }

      void SetAmount(float value) { amount = value; }
      float GetAmount() const { return amount; }

      void SetMobility(float value) { mobility = value; }
      float GetMobility() const { return mobility; }

      void SetHardness(float value) { hardness = value; }
      float GetHardness() const { return hardness; }

      void SetBulk(float value) { bulk = value; }
      float GetBulk() const { return bulk; }

      void SetToughness(float value) { toughness = value; }
      float GetToughness() const { return toughness; } 
    };

  } // Data namespace
} // PT namespace

#endif // PT_DATAITEM_H
