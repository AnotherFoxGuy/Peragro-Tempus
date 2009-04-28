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

#include "chatdatamanager.h"
#include "chat.h"

#define TILESIZE 512

namespace PT
{
  namespace Data
  {

    ChatDataManager::ChatDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/chat/chat.xml";
    }

    ChatDataManager::~ChatDataManager()
    {
      for (size_t i = 0; i<chanspaces.size(); i++) delete chanspaces[i];
    }

    bool ChatDataManager::parseElement(iDocumentNode* node)
    {
      if (!node) return false;
      csRef<iDocumentNode> tnode;

      if ( (tnode=node->GetNode("initgroups")) )
      {
        csRef<iDocumentNodeIterator> groups(tnode->GetNodes("group"));

        while (groups->HasNext())
        {
          csRef<iDocumentNode> gnode = groups->Next();
          const char* group = gnode->GetContentsValue();
          if (!group) group = "";
          initgroups.push_back(group);
        }
      }

      if ( (tnode=node->GetNode("defgroups")) )
      {
        csRef<iDocumentNodeIterator> groups(tnode->GetNodes("group"));

        while (groups->HasNext())
        {
          csRef<iDocumentNode> gnode = groups->Next();
          const char* group = gnode->GetContentsValue();
          if (!group) group = "";
          std::pair<std::string, bool> defgroup(group, false);
          if (gnode->GetAttribute("permanent"))
            defgroup.second = gnode->GetAttributeValueAsBool("permanent");
          defgroups.push_back(defgroup);
        }
      }

      listdef = -1;
      listmax = -1;
      if ( (tnode=node->GetNode("list")) )
      {
        if (tnode->GetAttribute("default"))
          listdef = tnode->GetAttributeValueAsInt("default");
        if (tnode->GetAttribute("max"))
          listmax = tnode->GetAttributeValueAsInt("max");
      }

      maxchannels = -1;
      if ( (tnode=node->GetNode("list")) )
      {
        if (tnode->GetAttribute("value"))
          maxchannels = tnode->GetAttributeValueAsInt("value");
      }

      csRef<iDocumentNodeIterator> cspaces(node->GetNodes("chanspace"));
      while (cspaces->HasNext())
      {
        csRef<iDocumentNode> cnode = cspaces->Next();
        csRef<iDocumentNode> tnode;
        ChanSpace* chanspace = new ChanSpace();
        if (!chanspace) continue;

        const char* str = cnode->GetAttributeValue("type");
        if (str) chanspace->SetType(str);
        str = cnode->GetAttributeValue("form");
        if (str) chanspace->SetForm(str);
        if (cnode->GetAttribute("max"))
          chanspace->SetMaxChannels(cnode->GetAttributeValueAsInt("max"));
        if (cnode->GetNode("maxchannels"))
          chanspace->SetMaxChannels(cnode->GetNode("maxchannels")->GetAttributeValueAsInt("value"));

        if ( (tnode=cnode->GetNode("create")) )
        {
          if (tnode->GetAttributeValue("type"))
            chanspace->SetCreateType(tnode->GetAttributeValue("type"));
          if (tnode->GetAttribute("value"))
            chanspace->SetCreateVal(tnode->GetAttributeValueAsInt("value"));
          if (tnode->GetAttribute("permanent"))
            chanspace->SetCreatePerm(tnode->GetAttributeValueAsBool("permanent"));
          if (tnode->GetAttribute("max"))
            chanspace->SetCreateMax(tnode->GetAttributeValueAsInt("max"));
        }

        if ( (tnode=cnode->GetNode("join")) )
        {
          if (tnode->GetAttributeValue("type"))
            chanspace->SetJoinType(tnode->GetAttributeValue("type"));
          if (tnode->GetAttribute("value"))
            chanspace->SetJoinVal(tnode->GetAttributeValueAsInt("value"));
          if (tnode->GetAttribute("permanent"))
            chanspace->SetJoinPerm(tnode->GetAttributeValueAsBool("permanent"));
        }

        if ( (tnode=cnode->GetNode("invite")) )
        {
          if (tnode->GetAttributeValue("type"))
            chanspace->SetInviteType(tnode->GetAttributeValue("type"));
          if (tnode->GetAttribute("value"))
            chanspace->SetInviteVal(tnode->GetAttributeValueAsInt("value"));
        }

        if ( (tnode=cnode->GetNode("visibility")) )
        {
          if (tnode->GetAttribute("channel"))
            chanspace->SetVisibilityChannel(tnode->GetAttributeValueAsBool("channel"));
          if (tnode->GetAttribute("members"))
            chanspace->SetVisibilityMembers(tnode->GetAttributeValueAsBool("members"));
        }

        chanspaces.push_back(chanspace);
      }

      return true;
    }

    ChanSpace* ChatDataManager::GetChanSpaceByName(const std::string& name) const
    {
      for (size_t i = 0; i < chanspaces.size(); i++ )
        if (chanspaces[i]->GetType() == name) return chanspaces[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
