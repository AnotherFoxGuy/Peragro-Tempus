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

#include <cssysdef.h>
#include <iutil/document.h>
#include <iutil/stringarray.h>

#include "npcdatamanager.h"
#include "npc.h"
#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

namespace PT
{
  namespace Data
  {

    NpcDataManager::NpcDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
    }

    NpcDataManager::~NpcDataManager()
    {
      for (size_t i = 0; i<npcs.size(); i++) delete npcs[i];
    }

    bool NpcDataManager::parseNPCs()
    {
      csRef<iVFS> vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getObjectRegistry());
      csRef<iStringArray> npcs=vfs->FindFiles("/peragro/xml/npcs/*.xml");
      while(npcs->GetSize()>0)
      {
        file = (std::string)npcs->Pop();
        printf("Parsing %s\n", file.c_str());
        parse();
      }
      return true;
    }

    ///@internal Currently the npcs vector is not preallocated, since there's
    ///no efficient way to get the count of npcs described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of npcs in file, and preallocate memory
    ///space.
    bool NpcDataManager::parseElement(iDocumentNode* node)
    {
      Npc* npc = new Npc();

      npc->SetName(node->GetNode("name")->GetContentsValue());

      npc->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      PtVector3 pos;
      pos.x = node->GetNode("position")->GetAttributeValueAsFloat("x");
      pos.y = node->GetNode("position")->GetAttributeValueAsFloat("y");
      pos.z = node->GetNode("position")->GetAttributeValueAsFloat("z");
      npc->SetPosition(pos);

      npc->SetSectorName(node->GetNode("position")->GetAttributeValue("sector"));

      npc->SetRace(node->GetNode("race") ->GetContentsValue());

      csRef<iDocumentNode> hair = node->GetNode("hair")->GetNode("color");
      npc->SetHairColor(hair->GetAttributeValueAsInt("r"),
                        hair->GetAttributeValueAsInt("g"),
                        hair->GetAttributeValueAsInt("b"));

      csRef<iDocumentNode> skin = node->GetNode("skin")->GetNode("color");
      npc->SetSkinColor(skin->GetAttributeValueAsInt("r"),
                        skin->GetAttributeValueAsInt("g"),
                        skin->GetAttributeValueAsInt("b"));

      csRef<iDocumentNode> decal = node->GetNode("decal")->GetNode("color");
      npc->SetDecalColor(decal->GetAttributeValueAsInt("r"),
                         decal->GetAttributeValueAsInt("g"),
                         decal->GetAttributeValueAsInt("b"));

      npc->SetAi(node->GetNode("ai")->GetContentsValue());

      csRef<iDocumentNodeIterator> setting (
        node->GetNode("aisetting")->GetNodes("value")
      );

      while (setting->HasNext())
      {
        csRef<iDocumentNode> value = setting->Next();
        npc->SetSetting(value->GetAttributeValue("name"), value->GetContentsValue());
      }

      csRef<iDocumentNodeIterator> inventory (
        node->GetNode("inventory")->GetNodes("item"));
      while (inventory->HasNext())
      {
        csRef<iDocumentNode> item = inventory->Next();
        int slot = item->GetAttributeValueAsInt("slot");
        int itemid = item->GetAttributeValueAsInt("item");
        int variation = item->GetAttributeValueAsInt("variation");
        npc->SetInventory(slot, itemid, variation);
      }

      csRef<iDocumentNode> reputationsNode = node->GetNode("reputations");
      if (reputationsNode)
      {
        csRef<iDocumentNodeIterator> reputations (reputationsNode->GetNodes("reputation"));
        while (reputations->HasNext())
        {
          csRef<iDocumentNode> reputation = reputations->Next();
          std::string name = reputation->GetAttributeValue("name");
          int value = reputation->GetContentsValueAsInt();
          npc->SetReputation(name, value);
        }
      }

      csRef<iDocumentNode> shopNode = node->GetNode("shop");
      if (shopNode)
      {
        csRef<iDocumentNode> buyNode = node->GetNode("buy");
        csRef<iDocumentNodeIterator> buyItems(buyNode->GetNodes("item"));
        while (buyItems->HasNext())
        {
          csRef<iDocumentNode> item = buyItems->Next();
          ShopItem* shopItem = new ShopItem();
          shopItem->id = item->GetAttributeValueAsInt("id");
          shopItem->quantity = item->GetAttributeValueAsInt("quantity");
          shopItem->rate = item->GetAttributeValueAsInt("rate");
          npc->AddBuyShopItem(shopItem);
        }

        csRef<iDocumentNode> sellNode = node->GetNode("sell");
        csRef<iDocumentNodeIterator> sellItems(sellNode->GetNodes("item"));
        while (sellItems->HasNext())
        {
          csRef<iDocumentNode> item = sellItems->Next();
          ShopItem* shopItem = new ShopItem();
          shopItem->id = item->GetAttributeValueAsInt("id");
          shopItem->quantity = item->GetAttributeValueAsInt("quantity");
          shopItem->rate = item->GetAttributeValueAsInt("rate");
          npc->AddSellShopItem(shopItem);
        }
      }

      csRef<iDocumentNodeIterator> dialogs (node->GetNodes("dialog"));
      while (dialogs->HasNext())
      {
        NpcDialog* npcdialog = new NpcDialog();
        csRef<iDocumentNode> dialognode = dialogs->Next();
        npcdialog->id = dialognode->GetAttributeValueAsInt("id");
        csRef<iDocumentNode> action;
        if(action = dialognode->GetNode("text"))
        {
          npcdialog->action = ptString("text", 4);
          npcdialog->value = action->GetContentsValue();
        }
        else if(action = dialognode->GetNode("teleport"))
        {
          npcdialog->action = ptString("teleport", 8);
          char buf[128];
          sprintf(buf, "2<%f,%f,%f>",
          action->GetAttributeValueAsFloat("x"),
          action->GetAttributeValueAsFloat("y"),
          action->GetAttributeValueAsFloat("z"));
          npcdialog->value = buf;
        }
        else if(action = dialognode->GetNode("buy"))
        {
          npcdialog->action = ptString("buy", 3);
          npcdialog->value = "";
        }
        else if(action = dialognode->GetNode("sell"))
        {
          npcdialog->action = ptString("sell", 4);
          npcdialog->value = "";
        }
        else if(action = dialognode->GetNode("function"))
        {
          npcdialog->action = ptString("function", strlen("function"));
          npcdialog->value = action->GetContentsValue();
        }
        else
        {
          Report(PT::Error, "Action type unknown (src/client/data/npcdatamanager.cpp)");
          npcdialog->action = ptString("unknown", 7);
          npcdialog->value = "unknown";
        }

        csRef<iDocumentNodeIterator> answers (dialognode->GetNodes("answer"));
        unsigned int answerId = 0;
        while (answers->HasNext())
        {
          NpcAnswer* npcanswer = new NpcAnswer();
          csRef<iDocumentNode> answernode = answers->Next();
          npcanswer->id = answerId;
          npcanswer->dialogId = npcdialog->id;
          npcanswer->value = (char*)answernode->GetContentsValue();
          csRef<iDocumentAttribute> href = answernode->GetAttribute("href");
          if (href.IsValid())
          {
            npcanswer->nextDialog = href->GetValueAsInt();
            npcanswer->isEnd = false;
          }
          else
          {
            npcanswer->nextDialog = 0;
            npcanswer->isEnd = true;
          }
          npc->AddDialogAnswer(npcanswer);
          answerId++;
        }

        npc->AddDialog(npcdialog);
      }

      npcs.push_back(npc);

      return true;
    }

    Npc* NpcDataManager::GetNpcByName(const std::string& name) const
    {
      for (size_t i = 0; i < npcs.size(); i++ )
        if (npcs[i]->GetName() == name) return npcs[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
