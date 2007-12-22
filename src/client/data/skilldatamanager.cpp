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

#include "skilldatamanager.h"
#include "skill.h"

namespace PT
{
  namespace Data
  {

    SkillDataManager::SkillDataManager()
    {
      file = "/peragro/xml/skills/skills.xml";
    }

    SkillDataManager::~SkillDataManager()
    {
      for (size_t i = 0; i<skills.size(); i++) delete skills[i];
    }

    ///@internal Currently the skills vector is not preallocated, since there's
    ///no efficient way to get the count of items described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of skills in file, and preallocate memory
    ///space.
    bool SkillDataManager::parseElement(iDocumentNode* node)
    {
      Skill* skill = new Skill();

      skill->SetId(node->GetNode("id")->GetContentsValueAsInt());

      skill->SetName(node->GetNode("name")->GetContentsValue());

      skill->SetIconName(node->GetNode("icon") ->GetContentsValue());

      skill->SetDescription(node->GetNode("description")->GetContentsValue());

      skill->SetStartString(node->GetNode("startstring")->GetContentsValue());

      skill->SetCompleteString(node->GetNode("completestring")->GetContentsValue());

      // Effects.
      csRef<iDocumentNode> effectsNode = node->GetNode("effects");

      Skill::Effects effects;

      effects.caster = effectsNode->GetNode("caster")->GetContentsValue();

      effects.castanim = effectsNode->GetNode("castanim")->GetContentsValue();

      effects.target = effectsNode->GetNode("target")->GetContentsValue();

      effects.targetanim = effectsNode->GetNode("targetanim")->GetContentsValue();

      effects.weapon = effectsNode->GetNode("weapon")->GetContentsValue();

      effects.projectile = effectsNode->GetNode("projectile")->GetContentsValue();

      skill->SetEffects(effects);

      //TODO make it parse the Stats, Cost, Damage, Requirements
      // elements.

      skills.push_back(skill);

      return true;
    }

    Skill* SkillDataManager::GetSkillById(unsigned int id) const
    {
      for (size_t i = 0; i<skills.size(); i++)
        if (skills[i]->GetId() == id) return skills[i];

      return 0;
    }

    Skill* SkillDataManager::GetSkillByName(const std::string& name) const
    {
      for (size_t i = 0; i<skills.size(); i++)
        if (skills[i]->GetName() == name) return skills[i];

      return 0;
    }
  } // Data namespace
} // PT namespace
