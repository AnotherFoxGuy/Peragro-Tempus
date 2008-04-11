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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "effectdatamanager.h"
#include "dataeffect.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    EffectDataManager::EffectDataManager(PointerLibrary* ptrlib) : DataManager(ptrlib)
    {
      file = "/peragro/xml/effects/effects.xml";
    }

    EffectDataManager::~EffectDataManager()
    {
      for (size_t i = 0; i<effects.size(); i++) delete effects[i];
    }

    bool EffectDataManager::parseElement(iDocumentNode* node)
    {
      Effect* effect = new Effect();

      effect->SetId(node->GetNode("id")->GetContentsValueAsInt());

      effect->SetName(node->GetNode("name")->GetContentsValue());

      effect->SetMeshFile(node->GetNode("file")->GetContentsValue());

      effect->SetMeshName(node->GetNode("mesh")->GetContentsValue());

      effect->SetDuration(node->GetNode("duration")->GetContentsValueAsInt());

      PtVector3 pos;

      pos.x = node->GetNode("offset")->GetAttributeValueAsFloat("x");
      pos.y = node->GetNode("offset")->GetAttributeValueAsFloat("y");
      pos.z = node->GetNode("offset")->GetAttributeValueAsFloat("z");
      effect->SetOffset(pos);

      effects.push_back(effect);

      return true;
    }

    Effect* EffectDataManager::GetEffectById(unsigned int id) const
    {
      for (size_t i = 0; i<effects.size(); i++)
        if (effects[i]->GetId() == id) return effects[i];

      return 0;
    }

    Effect* EffectDataManager::GetEffectByName(const std::string& name) const
    {
      for (size_t i = 0; i<effects.size(); i++)
        if (effects[i]->GetName() == name) return effects[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
