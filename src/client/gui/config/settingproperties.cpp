
#include "settingproperties.h"
#include "settingbase.h"
#include "settingslider.h"
#include "settingcombobox.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{
  namespace SettingProperties
  {

    String ConfigType::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingBase*>(receiver)->getConfigType();
    }

    void ConfigType::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingBase*>(receiver)->setConfigType(value);
    }

    String ConfigName::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingBase*>(receiver)->getConfigName();
    }

    void ConfigName::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingBase*>(receiver)->setConfigName(value);
    }

    String EventName::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingBase*>(receiver)->getEventName();
    }

    void EventName::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingBase*>(receiver)->setEventName(value);
    }

    String MinimumValue::get(const PropertyReceiver* receiver) const
    {
      return PropertyHelper::floatToString(static_cast<const SettingSlider*>(receiver)->getMinimumValue());
    }

    void MinimumValue::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingSlider*>(receiver)->setMinimumValue(PropertyHelper::stringToFloat(value));
    }

    String Values::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingComboBox*>(receiver)->getValues();
    }

    void Values::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingComboBox*>(receiver)->setValues(value);
    }

  } // end SettingProperties
} // end CEGUI
