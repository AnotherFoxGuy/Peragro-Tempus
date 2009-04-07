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

#include "server/entity/entitymanager.h"

#include "server/network/networkhelper.h"

#include "server/entity/character/character.h"
#include "server/entity/pcentity.h"
#include "server/entity/npcentity.h"

#include "common/util/printhelper.h"

namespace QuestUtils
{
  int static Apply( Character* character,const std::string& op, const std::vector<std::string>& args);

  void static RemoveSpaces(std::string& function)
  {
    //printf("RemoveSpaces: %s\n", function.c_str());
    size_t left = 0;
    for (size_t i = 0; i < function.length(); i++)
    {
      if (function[i] == ' ') left++;
      else break;
    }

    function = function.substr(left, function.length()-left);

    size_t right = function.length();
    for (size_t i = 0; i < function.length(); i++)
    {
      if (function[function.length()-1-i] == ' ') right--;
      else break;
    }

    function = function.substr(0, right);
    //printf("RemoveSpaces: %s\n", function.c_str());
  }

  bool static IsAtom(const std::string& function)
  {
    std::string::size_type idx =  function.find_first_of("(");
    return idx == std::string::npos;
  }

  std::string static GetOperation(const std::string& function)
  {
    std::string::size_type idx =  function.find_first_of(" ");
    std::string op = function.substr(1, idx-1);

    return op;
  }

  std::vector<std::string> static GetArguments(const std::string& function)
  {
    std::vector<std::string> argvec;

    std::string::size_type oplen = GetOperation(function).length()+1;
    std::string args = function.substr(oplen, function.length() - oplen);
    std::string::size_type right = args.find_last_of(")");
    if (right == std::string::npos) right = args.length();
    else args = args.substr(0, right);

    while (args.length() > 0)
    {
      int left = 0;
      std::string::size_type idx1 = std::string::npos;
      for(size_t i = 0; i < args.length(); i++)
      {
        if ('(' == args[i])
          left++;
        else if (')' == args[i])
        {
          left--;
          if (left == 0)
          {
            idx1 = i+1;
            break;
          }
        }
      }
      if (idx1 != std::string::npos)
      {
        std::string arg = args.substr(0, idx1);
        RemoveSpaces(arg);
        argvec.push_back(arg);
        args = args.substr(idx1, args.length()-1);
        //printf("GetArguments1: %s  rest: %s\n", arg.c_str(), args.c_str());
      }
      else
      {
        RemoveSpaces(args);
        std::string::size_type next = args.find_first_of(" ");
        if (next == std::string::npos)
        {
          //printf("GetArguments2: %s\n", args.c_str());
          argvec.push_back(args);
          break;
        }
        else
        {
          std::string arg = args.substr(0, next);
          argvec.push_back(arg);
          args = args.substr(next, args.length()-1);
          //printf("GetArguments3: %s  rest: %s\n", arg.c_str(), args.c_str());
        }

      }
    }
    return argvec;
  }

  void static SendDialog(Character* character, const NPCDialog* dialog)
  {
    if (character->GetType() != Common::Entity::PCEntityType) return;

    PcEntity* player = static_cast<PcEntity*>(character);
    NPCDialogState* dia_state = player->getNPCDialogState();

    NpcDialogMessage dialog_msg;
    dialog_msg.setDialogId((unsigned int)dialog->GetDialogId());
    dialog_msg.setDialogText(dialog->GetText().c_str());
    dialog_msg.setAnswersCount((unsigned char)dialog->GetAnswerCount());

    // The npc has nothing more to say, let him walk away.
    if (!dialog->GetAnswerCount())
    {
      NpcEntity* npc_entity = dia_state->getNpc();
      npc_entity->pause(false);

      NpcEndDialogMessage endmsg;
      endmsg.setNpcId(dia_state->getNpc()->GetId());
      ByteStream bs;
      endmsg.serialise(&bs);
      NetworkHelper::broadcast(bs);
    }

    for (size_t i = 0; i < dialog->GetAnswerCount(); i++)
    {
      const NPCDialogAnswer* answer = dialog->GetAnswer(i);
      dialog_msg.setAnswerId(i, (unsigned int)i);
      dialog_msg.setAnswerText(i, answer->GetText().c_str());
    }

    ByteStream bs;
    dialog_msg.serialise(&bs);

    NetworkHelper::sendMessage(character, bs);
  }

  int static Parse(Character* character, const std::string& function)
  {
    std::string func = function;
    RemoveSpaces(func);

    if (IsAtom(func))
    {
      if (func.compare("true") == 0)
        return 1;
      else if (func.compare("false") == 0)
        return 0;
      else
        return atoi(func.c_str());
    }
    else
    {
      return Apply(character, GetOperation(func), GetArguments(func));
    }
  }

  bool static IsPrimitive(const std::string& op)
  {
    std::string operands[7] = {"not", "and", "or", ">", "<", "!=", "="};
    for (size_t i = 0; i < 7; i++)
    {
      if (op.compare(operands[i]) == 0)
        return true;
    }

    return false;
  }

  int static ApplyPrimitive( Character* character, const std::string& op, const std::vector<std::string>& args)
  {
    bool result = false;
    if (op.compare("not") == 0)
    {
      if (args.size() < 1)
      {
        printf("ERROR: Not enough params for operation '%s'\n", op.c_str());
        return 0;
      }
      result = (Parse(character, args[0])) == 0;
    }

    if (args.size() < 2)
    {
      printf("ERROR: Not enough params for operation '%s'\n", op.c_str());
      return 0;
    }

    if (op.compare("and") == 0)
    {
      result = (Parse(character, args[0]) && Parse(character, args[1]));
    }
    else if (op.compare("or") == 0)
    {
      result = (Parse(character, args[0]) || Parse(character, args[1]));
    }
    else if (op.compare(">") == 0)
    {
      result = (Parse(character, args[0]) > Parse(character, args[1]));
    }
    else if (op.compare("<") == 0)
    {
      result = (Parse(character, args[0]) < Parse(character, args[1]));
    }
    else if (op.compare("!=") == 0)
    {
      result = (Parse(character, args[0]) != Parse(character, args[1]));
    }
    else if (op.compare("=") == 0)
    {
      result = (Parse(character, args[0]) == Parse(character, args[1]));
    }

    return result ? 1 : 0;
  }

  int static Apply(Character* character, const std::string& op, const std::vector<std::string>& args)
  {
    if (IsPrimitive(op))
    {
      return ApplyPrimitive(character, op, args);
    }
    else if (op.compare("if") == 0)
    {
      if (args.size() < 3) {printf("ERROR: Not enough params for operation 'if'\n"); return 0;}
      if (Parse(character, args[0]))
      {
        return Parse(character, args[1]);
      }
      else
      {
        if (args.size() < 3)
          return 0;
        else
          return Parse(character, args[2]);
      }
    }
    else if (op.compare("begin") == 0)
    {
      if (args.size() < 1) {printf("ERROR: Not enough params for operation 'begin'\n"); return 0;}
      for (size_t i = 0; i < args.size()-1; i++)
      {
        Parse(character, args[i]);
      }
      return Parse(character, args[args.size()-1]);
    }
    else if (op.compare("reputation") == 0)
    {
      //(reputation RepName [CharName] <add/sub/set> <#>)
      // CharName is supposed to allow NPCs to ask about other players or
      // NPCs, defaults to the current player if not supplied, for players this
      // would require variables, so for now I guess it's only fixed NPC names.
      if (args.size() < 1) {printf("ERROR: Not enough params for operation 'reputation'\n"); return 0;}
      Server* server = Server::getServer();
      EntityManager* entitymanager = server->getEntityManager();
      std::string reputation = args[0];
      try { character->GetReputations()->Get(reputation); } catch (Exception&) { return 0; }
      if (args.size() > 2)
      {
        if (args.size() > 3)
        {
          Common::Entity::Entityp entity = entitymanager->FindByName(args[1]);
          if (!entity) { printf("Error: Failed to find entity named \"%s\"\n", args[1].c_str()); return 0; }
          Character* character = dynamic_cast<Character*>(entity.get());
          if (!character) { printf("Error: Entity named \"%s\" isn't a character!\n", args[1].c_str()); return 0; }

          try { character->GetReputations()->Get(reputation); } catch (Exception&) { return 0; }

          int val = Parse(character, args[3]);
          if (args[2].compare("add") == 0)
            character->GetReputations()->Add(reputation, val);
          if (args[2].compare("sub") == 0)
            character->GetReputations()->Sub(reputation, val);
          if (args[2].compare("set") == 0)
            character->GetReputations()->Set(reputation, val);
          printf("REPUTATION: Updated %s for entity %d to %"SIZET"!\n", reputation.c_str(), entity->GetId(), character->GetReputations()->Get(reputation));
          return val;
        }
        else
        {
          int val = Parse(character, args[2]);
          if (args[1].compare("add") == 0)
            character->GetReputations()->Add(reputation, val);
          if (args[1].compare("sub") == 0)
            character->GetReputations()->Sub(reputation, val);
          if (args[1].compare("set") == 0)
            character->GetReputations()->Set(reputation, val);
          printf("REPUTATION: Updated %s to %"SIZET"!\n", reputation.c_str(), character->GetReputations()->Get(reputation));
          return val;
        }
      }
      else
      {
        if (args.size() > 1)
        {
          EntityManager* entitymanager = server->getEntityManager();
          Common::Entity::Entityp entity = entitymanager->FindByName(args[1]);
          if (!entity){printf("Error: Failed to find entity named \"%s\"\n", args[1].c_str());return 0;}
          Character* character = dynamic_cast<Character*>(entity.get());
          if (!character) { printf("Error: Entity named \"%s\" isn't a character!\n", args[1].c_str()); return 0; }

          try { character->GetReputations()->Get(reputation); } catch (Exception&) { return 0; }

          return character->GetReputations()->Get(reputation);
        }
        else
        {
          return character->GetReputations()->Get(reputation);
        }
      }
    }
    else if (op.compare("inventory") == 0)
    {
      /*
      //(inventory add/remove/count itemId <#>)
      if (args.size() < 2) {printf("ERROR: Not enough params for operation 'inventory'\n"); return 0;}
      printf("INVENTORY: Updated1 %s!\n", args[1].c_str());
      unsigned int itemId = Parse(character, args[1]);
      if (args[0].compare("count") == 0)
        return character->getInventory()->getTotalAmount(itemId, 0);
      else if (args[0].compare("add") == 0)
      {
        Server* server = Server::getServer();
        Item* item = server->getItemManager()->findById(itemId);
        if (!item) {printf("ERROR: inventory:No such item!!\n"); return 0;}

        InventoryEntry entry(itemId, 0);
        unsigned char slot = character->getInventory()->getFreeSlot();
        bool succes = character->getInventory()->addItem(entry, slot);

        if (succes)
        {
          const Entity* user_ent = character->getEntity();
          PickResponseMessage response_msg;
          response_msg.setItemId(item->GetId());
          response_msg.setSlotId(slot);

          response_msg.setName(item->getName());
          response_msg.setIconName(item->getIcon());
          response_msg.setDescription(item->getDescription());
          response_msg.setWeight(item->getWeight());
          response_msg.setEquipType(item->getEquiptype());

          ByteStream bs;
          response_msg.serialise(&bs);
          NetworkHelper::sendMessage(user_ent, bs);
        }

        return succes;
      }
      else if (args[0].compare("remove") == 0 && args.size() > 2)
      {
        unsigned int nr = Parse(character, args[2]);
        unsigned int amount = character->getInventory()->getTotalAmount(itemId, 0);
        if (nr != amount) {printf("ERROR: inventory: Not enough items of type %d !\n", itemId); return 0;}
        const Entity* user_ent = character->getEntity();
        for (size_t i = 0; i < amount+1; i++)
        {
          unsigned char slot = character->getInventory()->getSlot(itemId, 0);
          bool succes = character->getInventory()->takeItem(slot);
          if (succes)
          {
            DropResponseMessage response_msg;
            response_msg.setSlotId(slot);

            ByteStream bs;
            response_msg.serialise(&bs);
            NetworkHelper::sendMessage(user_ent, bs);
          }
        }
      }
      */
      return 0;
    }
    else if (op.compare("dialog") == 0)
    {
      if (args.size() < 1) {printf("ERROR: Not enough params for operation 'dialog'\n"); return 0;}
      if (character->GetType() != Common::Entity::PCEntityType) return 0;
      PcEntity* player = static_cast<PcEntity*>(character);

      unsigned int id = Parse(character, args[0]);
      NPCDialogState* dia_state = player->getNPCDialogState();
      const NPCDialog* dialog = dia_state->startDialog(dia_state->getNpc()->GetId(), id);

      if (dialog->GetAction() == NPCDialog::SHOW_TEXT)
      {
        SendDialog(character, dialog);
      }
      else if (dialog->GetAction() == NPCDialog::START_BUY)
      {
        const NpcEntity* c_npc = dia_state->getNpc();
        if (c_npc)
        {
          // hardcoded! just to test it!
          // All this should go to the trade handler anyway!
          TradeOffersListNpcMessage trade_msg;
          trade_msg.setIsBuy(1);
          trade_msg.setOffersCount(4);

          trade_msg.setItemId(0, 4);
          trade_msg.setName(0, ptString("Pot", 3));
          trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
          trade_msg.setPrice(0, 200);

          trade_msg.setItemId(1, 3);
          trade_msg.setName(1, ptString("Scythe", 6));
          trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
          trade_msg.setPrice(1, 100);

          trade_msg.setItemId(2, 6); //empty book
          trade_msg.setName(2, ptString("Empty book", 10));
          trade_msg.setIconName(2, ptString("set:Inventory image:oldbook", 27));
          trade_msg.setPrice(2, 10);

          trade_msg.setItemId(3, 7); //gate key
          trade_msg.setName(3, ptString("Gate key", 8));
          trade_msg.setIconName(3, ptString("set:Inventory image:key01", 25));
          trade_msg.setPrice(3, 300);

          ByteStream bs;
          trade_msg.serialise(&bs);

          NetworkHelper::sendMessage(character, bs);

          NpcEndDialogMessage endmsg;
          endmsg.setNpcId(dia_state->getNpc()->GetId());
          ByteStream bs2;
          endmsg.serialise(&bs2);
          NetworkHelper::broadcast(bs2);
        }
      }
      else if (dialog->GetAction() == NPCDialog::START_SELL)
      {
        const NpcEntity* c_npc = dia_state->getNpc();
        if (c_npc)
        {
          // hardcoded! just to test it!
          // All this should go to the trade handler anyway!
          TradeOffersListNpcMessage trade_msg;
          trade_msg.setIsBuy(0);
          trade_msg.setOffersCount(2);

          trade_msg.setItemId(0, 4);
          trade_msg.setName(0, ptString("Pot", 3));
          trade_msg.setIconName(0, ptString("set:Inventory image:tinyballpot", 31));
          trade_msg.setPrice(0, 175);

          trade_msg.setItemId(1, 3);
          trade_msg.setName(1, ptString("Scythe", 6));
          trade_msg.setIconName(1, ptString("set:Inventory image:Scythe", 26));
          trade_msg.setPrice(1,  75);

          ByteStream bs;
          trade_msg.serialise(&bs);

          NetworkHelper::sendMessage(character, bs);

          NpcEndDialogMessage endmsg;
          endmsg.setNpcId(dia_state->getNpc()->GetId());
          ByteStream bs2;
          endmsg.serialise(&bs2);
          NetworkHelper::broadcast(bs2);
        }
      }
      else if (dialog->GetAction() == NPCDialog::TELEPORT)
      {
        /*
        // yes, it's a hack. This shouldn't go here either.
        // sector_id <0.5, 0.6, 0.8>
        unsigned short sector = 0;
        float x = 0, y = 0, z = 0;
        sscanf(dialog->getText(), "%hd<%f,%f,%f>", &sector, &x, &y, &z);

        character->SetSector(sector);
        character->SetPosition(x, y, z);

        Server::getServer()->getCharacterManager()->checkForSave(character);

        TeleportResponseMessage telemsg;
        telemsg.setEntityId(ent->GetId());
        telemsg.SetPosition(ent->GetPosition());
        telemsg.SetRotation(ent->GetRotation());
        telemsg.SetSectorId(ent->GetSector());

        ByteStream bs;
        telemsg.serialise(&bs);
        Server::getServer()->broadCast(bs);

        NpcEndDialogMessage endmsg;
        endmsg.setNpcId(dia_state->getNpc()->GetId());
        ByteStream bs2;
        endmsg.serialise(&bs2);
        Server::getServer()->broadCast(bs2);
        */
      }
      else if (dialog->GetAction() == NPCDialog::FUNCTION)
      {
        Parse(character, dialog->GetText());
      }
      return id;
    }

    return 0;
  }
};
