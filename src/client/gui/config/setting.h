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

#ifndef SETTING_H
#define SETTING_H

#include <cssysdef.h>
#include <iutil/cfgfile.h>
#include <iutil/cfgmgr.h>

#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>
#include <csutil/eventnames.h>

#include "client/pointer/pointer.h"

#include <string>

#include "ceguistring.h"


namespace PT
{
  namespace GUI
  {
    class Setting
    {
    public:
      enum SettingType
      {
        Bool = 0,
        Int,
        Float,
        String,
        Undefined
      };

    private:
      std::string name;
      SettingType settingType;
      std::string evName;

      template<typename T>
      void FireEvent();

    public:
      Setting() : settingType(Undefined) {}
      ~Setting() {}
      void SetValueName(const std::string& value) { name = value; }
      void SetValueType(const std::string& value);
      void SetEventName(const std::string& value) { evName = value; }

      operator bool() { return !name.empty() && settingType != Undefined && !evName.empty(); }

      template<typename T>
      void Get(T& value);

      template<typename T>
      void Set(const T& value);

      CEGUI::String GetAsString();
    };

    template<> void Setting::Get<bool>(bool& value);
    template<> void Setting::Get<int>(int& value);
    template<> void Setting::Get<float>(float& value);

  } // GUI namespace
} // PT namespace

#endif // SETTING_H
