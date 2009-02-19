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
      if (evName.empty() || evName == "None" || evName == "none") return;
      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
      csRef<iEventNameRegistry> nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
      csEventID evNameId = nameRegistry->GetID(evName.c_str());
      csRef<iEvent> ev = eventQueue->CreateBroadcastEvent(evNameId);
      T v; Get(v);
      ev->Add(name.c_str(), v);
      eventQueue->Post(ev);
    }

    template<>
    void Setting::FireEvent<CEGUI::String>()
    {
      if (evName.empty() || evName == "None" || evName == "none") return;
      iObjectRegistry* object_reg = PointerLibrary::getInstance()->getObjectRegistry();
      csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
      csRef<iEventNameRegistry> nameRegistry = csEventNameRegistry::GetRegistry(object_reg);
      csEventID evNameId = nameRegistry->GetID(evName.c_str());
      csRef<iEvent> ev = eventQueue->CreateBroadcastEvent(evNameId);
      CEGUI::String v; Get(v);
      ev->Add(name.c_str(), v.c_str());
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
      case String:
        {
          CEGUI::String v; Get(v);
          return v;
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
      else if (value == "string")
        settingType = String;
      else
        settingType = Undefined;
    }

#define GETAPP_CFG                                                  \
    csRef<iConfigManager> app_cfg = csQueryRegistry<iConfigManager> \
          (PointerLibrary::getInstance()->getObjectRegistry());     \

    //--[Generic]---------------------------
    void Setting::SetFromString(const CEGUI::String& value)
    {
      GETAPP_CFG;
      switch (settingType)
      {
      case Bool:
        {
          Set<bool>(CEGUI::PropertyHelper::stringToBool(value));
        } break;
      case Int:
        {
          Set<int>(CEGUI::PropertyHelper::stringToInt(value));
        } break;
      case Float:
        {
          Set<float>(CEGUI::PropertyHelper::stringToFloat(value));
        } break;
      case String:
        {
          Set<CEGUI::String>(value);
        } break;
      }
    }

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

    //--[String]------------------------------
    template<>
    void Setting::Get<CEGUI::String>(CEGUI::String& value)
    {
      GETAPP_CFG;
      value = app_cfg->GetStr(name.c_str(), "");
    }

    template<>
    void Setting::Set<CEGUI::String>(const CEGUI::String& value)
    {
      GETAPP_CFG;
      app_cfg->SetStr(name.c_str(), value.c_str());
      app_cfg->Save();
      FireEvent<CEGUI::String>();
    }

  } // GUI namespace
} // PT namespace
