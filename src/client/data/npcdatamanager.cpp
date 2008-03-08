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

#include "npcdatamanager.h"
#include "npc.h"

namespace PT
{
  namespace Data
  {

    NpcDataManager::NpcDataManager()
    {
      file = "/peragro/xml/npcs/npcs.xml";
    }

    NpcDataManager::~NpcDataManager()
    {
      for (size_t i = 0; i<npcs.size(); i++) delete npcs[i];
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

      npc->SetDialog(node->GetNode("dialog")->GetContentsValueAsInt());

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
        node->GetNode("inventory")->GetNodes("item")
      );

      while (inventory->HasNext())
      {
        csRef<iDocumentNode> item = inventory->Next();
        int slot = item->GetAttributeValueAsInt("slot");
        int itemid = item->GetAttributeValueAsInt("item");
        int variation = item->GetAttributeValueAsInt("variation");
        npc->SetInventory(slot, itemid, variation);
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
