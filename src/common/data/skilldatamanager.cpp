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

#include "skilldatamanager.h"
#include "skill.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    SkillDataManager::~SkillDataManager()
    {
      for (size_t i = 0; i<skills.size(); i++) delete skills[i];
    }

    ///@internal Currently the skills vector is not preallocated, since there's
    ///no efficient way to get the count of items described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of skills in file, and preallocate memory
    ///space.
    bool SkillDataManager::LoadSkillData()
    {
      TiXmlDocument doc;
      std::string file;

      file = dataPath + "/xml/skills/skills.xml";
      if (!doc.LoadFile(file.c_str())) return false;

      TiXmlElement* skillsXML = doc.FirstChildElement("skills");

      if (!skillsXML) return false;

      TiXmlElement* skillNode = skillsXML->FirstChildElement("skill");
      for (; skillNode; skillNode = skillNode->NextSiblingElement("skill"))
      {
        Skill* skill = new Skill();

        skill->SetId(atoi(skillNode->FirstChildElement("id")->FirstChild()
            ->ToText()->Value()));

        skill->SetName(skillNode->FirstChildElement("name")->FirstChild()
            ->ToText()->Value());

        skill->SetIconName(skillNode->FirstChildElement("icon") ->FirstChild()
            ->ToText()->Value());

        skill->SetDescription(skillNode->FirstChildElement("description")
            ->FirstChild()->ToText()->Value());

        skill->SetStartString(skillNode->FirstChildElement("startstring")
            ->FirstChild()->ToText()->Value());

        skill->SetCompleteString(skillNode->FirstChildElement("completestring")
            ->FirstChild()->ToText()->Value());

        //TODO make it parse the Effects, Stats, Cost, Damage, Requirements
        // elements.

        skills.push_back(skill);
      }

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
