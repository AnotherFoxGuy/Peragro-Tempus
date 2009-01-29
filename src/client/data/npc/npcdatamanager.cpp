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

      IFNODE(node, file)
        npc->SetFileName(file->GetContentsValue());

      npc->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      WFMath::Point<3> pos;
      pos[0] = node->GetNode("position")->GetAttributeValueAsFloat("x");
      pos[1] = node->GetNode("position")->GetAttributeValueAsFloat("y");
      pos[2] = node->GetNode("position")->GetAttributeValueAsFloat("z");
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


      csRef<iDocumentNode> aisettings = node->GetNode("aisetting");
      if (aisettings)
      {
        npc->SetAi(aisettings->GetAttributeValue("type"));

        csRef<iDocumentNodeIterator> setting (aisettings->GetNodes("value"));
        while (setting->HasNext())
        {
          csRef<iDocumentNode> value = setting->Next();
          npc->SetSetting(value->GetAttributeValue("name"), value->GetContentsValue());
        }
      }

      FOREACHNODE(node, item, inventory)
      {
        int slot = item->GetAttributeValueAsInt("slot");
        int itemid = item->GetAttributeValueAsInt("item");
        int variation = item->GetAttributeValueAsInt("variation");
        npc->SetInventory(slot, itemid, variation);
      }

      FOREACHNODE(node, reputation, reputations)
      {
        std::string name = reputation->GetAttributeValue("name");
        int value = reputation->GetContentsValueAsInt();
        npc->SetReputation(name, value);
      }

      FOREACHNODE(node, trait, traits)
      {
        std::string name = trait->GetAttributeValue("name");
        int value = trait->GetContentsValueAsInt();
        npc->SetReputation(name, value);
      }

      IFNODE(node, shop)
      {
        {
          FOREACHNODE(shop, item, buy)
          {
            ShopItem* shopItem = new ShopItem();
            shopItem->id = item->GetAttributeValueAsInt("id");
            shopItem->quantity = item->GetAttributeValueAsInt("quantity");
            shopItem->rate = item->GetAttributeValueAsInt("rate");
            npc->AddBuyShopItem(shopItem);
          }
        }

        {
          FOREACHNODE(shop, item, sell)
          {
            ShopItem* shopItem = new ShopItem();
            shopItem->id = item->GetAttributeValueAsInt("id");
            shopItem->quantity = item->GetAttributeValueAsInt("quantity");
            shopItem->rate = item->GetAttributeValueAsInt("rate");
            npc->AddSellShopItem(shopItem);
          }
        }
      }

      FOREACHNODE(node, occupation, occupations)
      {
        csRef<iDocumentNode> positionNode = occupation->GetNode("position");
        WFMath::Point<3> pos;
        pos[0] = positionNode->GetAttributeValueAsFloat("x");
        pos[1] = positionNode->GetAttributeValueAsFloat("y");
        pos[2] = positionNode->GetAttributeValueAsFloat("z");
        std::string sector = positionNode->GetAttributeValue("sector");
      }

      FOREACHNODE(node, hobby, hobbies)
      {
        hobby->GetAttributeValue("sector");
      }

      FOREACHNODE(node, skill, skills)
      {
      }

      FOREACHNODE(node, dialog, dialogs)
      {
        NpcDialog* npcdialog = new NpcDialog();
        npcdialog->id = dialog->GetAttributeValueAsInt("id");
        csRef<iDocumentNode> action;
        if(action = dialog->GetNode("text"))
        {
          npcdialog->action = ptString("text", 4);
          npcdialog->value = action->GetContentsValue();
        }
        else if(action = dialog->GetNode("teleport"))
        {
          npcdialog->action = ptString("teleport", 8);
          char buf[128];
          sprintf(buf, "2<%f,%f,%f>",
            action->GetAttributeValueAsFloat("x"),
            action->GetAttributeValueAsFloat("y"),
            action->GetAttributeValueAsFloat("z"));
          npcdialog->value = buf;
        }
        else if(action = dialog->GetNode("buy"))
        {
          npcdialog->action = ptString("buy", 3);
          npcdialog->value = "";
        }
        else if(action = dialog->GetNode("sell"))
        {
          npcdialog->action = ptString("sell", 4);
          npcdialog->value = "";
        }
        else if(action = dialog->GetNode("function"))
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

        csRef<iDocumentNodeIterator> answers (dialog->GetNodes("answer"));
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
        } //end while

        npc->AddDialog(npcdialog);
      } //end foreach

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
