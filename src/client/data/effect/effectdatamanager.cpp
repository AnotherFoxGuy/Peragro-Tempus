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

#include "client/reporter/reporter.h"
#include "client/pointer/pointer.h"
#include "effectdatamanager.h"
#include "dataeffect.h"

#include "ext/tinyxml/tinyxml.h"

namespace PT
{
  namespace Data
  {

    EffectDataManager::EffectDataManager()
    {
      vfs = csQueryRegistry<iVFS> (PointerLibrary::getInstance()->getObjectRegistry());
      if (!vfs) Report(PT::Error, "Failed to locate VFS!");
    }

    EffectDataManager::~EffectDataManager()
    {
      for (size_t i = 0; i<effects.size(); i++) delete effects[i];
    }

    ///@internal Currently the effects vector is not preallocated, since there's
    ///no efficient way to get the count of effects described in XML file. If need
    ///be, we could traverse the XML file once before adding the actual data in
    ///order to determine the number of effects in file, and preallocate memory
    ///space.
    bool EffectDataManager::LoadEffectData()
    {
      const char* fname="/peragro/xml/effects/effects.xml";
      csRef<iDataBuffer> xmlfile = vfs->ReadFile (fname);
      if (!xmlfile){return Report(PT::Error, "Can't load file '%s'!", fname);}

      TiXmlDocument doc;
      if (!doc.Parse(xmlfile->GetData())) return false;

      TiXmlElement* effectsXML = doc.FirstChildElement("effects");

      if (!effectsXML) return false;

      TiXmlElement* effectNode = effectsXML->FirstChildElement("effect");
      for (; effectNode; effectNode = effectNode->NextSiblingElement("effect"))
      {
        Effect* effect = new Effect();

        effect->SetId(atoi(effectNode->FirstChildElement("id")->FirstChild()
            ->ToText()->Value()));

        effect->SetName(effectNode->FirstChildElement("name")->FirstChild()
            ->ToText()->Value());

        effect->SetMeshFile(effectNode->FirstChildElement("file")->FirstChild()
            ->ToText()->Value());

        effect->SetMeshName(effectNode->FirstChildElement("mesh")->FirstChild()
            ->ToText()->Value());

        effect->SetDuration(atoi(effectNode->FirstChildElement("duration")->FirstChild()
            ->ToText()->Value()));

        PtVector3 pos;
        double temp; //The Attribute() call requires a damn double
        effectNode->FirstChildElement("offset")->Attribute("x", &temp);
        pos.x = temp;
        effectNode->FirstChildElement("offset")->Attribute("y", &temp);
        pos.x = temp;
        effectNode->FirstChildElement("offset")->Attribute("z", &temp);
        pos.x = temp;
        effect->SetOffset(pos);

        effects.push_back(effect);
      }

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
