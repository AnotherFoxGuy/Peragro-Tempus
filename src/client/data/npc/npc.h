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
#include <wfmath/point.h>
#include "common/util/ptstring.h"

#include "common/entity/entity.h"

namespace PT
{
  namespace Data
  {
    /**
     * Class for storing data about answers to NPC dialogs.
     */
    class NpcAnswer
    {
    public:
      unsigned int id;
      unsigned int dialogId;
      std::string value;
      unsigned int nextDialog;
      bool isEnd;
    };

    /**
     * Class for storing data about NPC dialogs.
     */
    class NpcDialog
    {
    public:
      unsigned int id;
      ptString action;
      std::string value;
    };

    /**
     * Class for storing data about NPC shops.
     */
    class ShopItem
    {
    public:
      unsigned int id;
      unsigned int quantity;
      unsigned int rate;
    };

    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about npc. Usually not
     * created directly, but accessed via NpcDataManager instance. Use the
     * Set/Get methods to access the npc properties.
     * @see NpcDataManager
     */
    class Npc : public Common::Entity::Entity
    {
    private:
      std::string race;

      unsigned char hair[3];
      unsigned char skin[3];
      unsigned char decal[3];

      std::string ai;

      std::map<std::string, std::string> setting;

      std::map<int, std::pair<int, int> > inventory;

      Array<NpcDialog*> dialogs;
      Array<NpcAnswer*> answers;

      Array<ShopItem*> shopBuy;
      Array<ShopItem*> shopSell;

      std::map<std::string, int > reputations;

    public:
      Npc() {}

      ~Npc()
      {
        dialogs.delAll();
        answers.delAll();

        shopBuy.delAll();
        shopSell.delAll();
      }

      void SetRace(const std::string& value) { race = value; }
      const std::string& GetRace() const { return race; }

      void SetHairColor(unsigned char r, unsigned char g, unsigned char b)
      { hair[0] = r; hair[1] = g; hair[2] = b; }
      const unsigned char* GetHairColor() { return hair; }

      void SetSkinColor(unsigned char r, unsigned char g, unsigned char b)
      { skin[0] = r; skin[1] = g; skin[2] = b; }
      const unsigned char* GetSkinColor() { return skin; }

      void SetDecalColor(unsigned char r, unsigned char g, unsigned char b)
      { decal[0] = r; decal[1] = g; decal[2] = b; }
      const unsigned char* GetDecalColor() { return decal; }

      void SetAi(const std::string& value) { ai = value; }
      const std::string& GetAi() const { return ai; }

      void SetSetting(const std::string& key, const std::string& value)
      { setting[key] = value; }
      const std::string& GetSetting(const std::string& key)
      { return setting[key]; }
      const std::map<std::string, std::string>& GetAllSetting()
      { return setting; }

      void SetInventory(int slot, int item, int variation)
      { inventory[slot] = std::pair<int, int>(item, variation); }
      const std::pair<int, int>& GetInventory(int slot)
      { return inventory[slot]; }
      const std::map<int, std::pair<int, int> > & GetAllInventory()
      { return inventory; }

      void AddDialog(NpcDialog* dialog)
      { dialogs.add(dialog); }
      Array<NpcDialog*> GetDialogs()
      { return dialogs; }

      void AddDialogAnswer(NpcAnswer* answer)
      { answers.add(answer); }
      Array<NpcAnswer*> GetDialogAnswers()
      { return answers; }

      void AddBuyShopItem(ShopItem* item)
      { shopBuy.add(item); }
      Array<ShopItem*> GetBuyShopItems()
      { return shopBuy; }

      void AddSellShopItem(ShopItem* item)
      { shopSell.add(item); }
      Array<ShopItem*> GetSellShopItems()
      { return shopSell; }

      void SetReputation(const std::string& repname, const int& value)
      { reputations[repname] = value; }

      const std::map<std::string, int > & GetReputations()
      { return reputations; }

    };
  } // Data namespace
} // PT namespace

#endif // PT_DATANPC_H
