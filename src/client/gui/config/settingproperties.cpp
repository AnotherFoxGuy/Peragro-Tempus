
#include "settingproperties.h"
#include "settingslider.h"
#include "CEGUIPropertyHelper.h"


// Start of CEGUI namespace section
namespace CEGUI
{
  namespace SettingProperties
  {

    String ConfigType::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingSlider*>(receiver)->getConfigType();
    }

    void ConfigType::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingSlider*>(receiver)->setConfigType(value);
    }

    String ConfigName::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingSlider*>(receiver)->getConfigName();
    }

    void ConfigName::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingSlider*>(receiver)->setConfigName(value);
    }

    String EventName::get(const PropertyReceiver* receiver) const
    {
      return static_cast<const SettingSlider*>(receiver)->getEventName();
    }

    void EventName::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingSlider*>(receiver)->setEventName(value);
    }

    String MinimumValue::get(const PropertyReceiver* receiver) const
    {
      return PropertyHelper::floatToString(static_cast<const SettingSlider*>(receiver)->getMinimumValue());
    }

    void MinimumValue::set(PropertyReceiver* receiver, const String& value)
    {
      static_cast<SettingSlider*>(receiver)->setMinimumValue(PropertyHelper::stringToFloat(value));
    }

  } // end SettingProperties
} // end CEGUI
