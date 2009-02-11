
#ifndef SETTING_PROPERTIES_H
#define SETTING_PROPERTIES_H

#include "CEGUIProperty.h"


// Start of CEGUI namespace section
namespace CEGUI
{
namespace SettingProperties
{

  class ConfigType : public Property
  {
  public:
    ConfigType() : Property("ConfigType",
                            "Value is a text string.",
                            "")
    {}

    String	get(const PropertyReceiver* receiver) const;
    void	set(PropertyReceiver* receiver, const String& value);
  };

  class ConfigName : public Property
  {
  public:
    ConfigName() : Property("ConfigName",
                            "Value is a text string.",
                            "")
    {}

    String	get(const PropertyReceiver* receiver) const;
    void	set(PropertyReceiver* receiver, const String& value);
  };

  class EventName : public Property
  {
  public:
    EventName() : Property("EventName",
                            "Value is a text string.",
                            "")
    {}

    String	get(const PropertyReceiver* receiver) const;
    void	set(PropertyReceiver* receiver, const String& value);
  };

  class MinimumValue : public Property
  {
  public:
    MinimumValue() : Property("MinimumValue",
                            "Value is a float.",
                            "0.0")
    {}

    String	get(const PropertyReceiver* receiver) const;
    void	set(PropertyReceiver* receiver, const String& value);
  };


} // end SettingProperties
} // end CEGUI


#endif	// SETTING_PROPERTIES_H
