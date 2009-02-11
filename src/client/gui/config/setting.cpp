/*
    Copyright (C) 2009 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "setting.h"
#include "CEGUIPropertyHelper.h"

namespace PT
{
  namespace GUI
  {
    template<typename T>
    void Setting::FireEvent()
    {
      if (evName.empty()) return;
      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
      csRef<iEventNameRegistry> nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
      csEventID evNameId = nameRegistry->GetID(evName.c_str());
      csRef<iEvent> ev = eventQueue->CreateBroadcastEvent(evNameId);
      T v; Get(v);
      ev->Add(name.c_str(), v);
      eventQueue->Post(ev);
    }

    CEGUI::String Setting::GetAsString()
    {
      switch (settingType)
      {
      case Bool:
        {
          bool v; Get(v);
          return CEGUI::PropertyHelper::boolToString(v);
        } break;
      case Int:
        {
          int v; Get(v);
          return CEGUI::PropertyHelper::intToString(v);
        } break;
      case Float:
        {
          float v; Get(v);
          return CEGUI::PropertyHelper::floatToString(v);
        } break;
      default:
        return "ERROR";
      } // end switch
    }

    void Setting::SetValueType(const std::string& value)
    {
      if (value == "bool")
        settingType = Bool;
      else if (value == "int")
        settingType = Int;
      else if (value == "float")
        settingType = Float;
      else
        settingType = Undefined;
    }

#define GETAPP_CFG                                                  \
    csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> \
          (PointerLibrary::getInstance()->getObjectRegistry());     \

    //--[Bool]------------------------------
    template<>
    void Setting::Get<bool>(bool& value)
    {
      GETAPP_CFG;
      value = app_cfg->GetBool(name.c_str(), false);
    }

    template<>
    void Setting::Set<bool>(const bool& value)
    {
      GETAPP_CFG;
      app_cfg->SetBool(name.c_str(), value);
      app_cfg->Save();
      FireEvent<bool>();
    }

    //--[Int]------------------------------
    template<>
    void Setting::Get<int>(int& value)
    {
      GETAPP_CFG;
      value = app_cfg->GetInt(name.c_str(), 0);
    }

    template<>
    void Setting::Set<int>(const int& value)
    {
      GETAPP_CFG;
      app_cfg->SetInt(name.c_str(), value);
      app_cfg->Save();
      FireEvent<int>();
    }

    //--[Float]------------------------------
    template<>
    void Setting::Get<float>(float& value)
    {
      GETAPP_CFG;
      value = app_cfg->GetFloat(name.c_str(), 0.0f);
    }

    template<>
    void Setting::Set<float>(const float& value)
    {
      GETAPP_CFG;
      app_cfg->SetFloat(name.c_str(), value);
      app_cfg->Save();
      FireEvent<float>();
    }

  } // GUI namespace
} // PT namespace
