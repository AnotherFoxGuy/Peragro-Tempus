
#ifndef SETTINGSLIDER_H
#define SETTINGSLIDER_H

//#include <CEGUI.h>
#include <CEGUIBase.h>
#include <CEGUIWindow.h>
#include <CEGUIWindowFactory.h>

#include "settingproperties.h"

#include "setting.h"
#include <boost/shared_ptr.hpp>


#if defined(_MSC_VER)
#	pragma warning(push)
#	pragma warning(disable : 4251)
#endif

namespace CEGUI
{
  /*!
  \brief
    Base class for the SettingSlider widget
  */
  class /*CEGUIEXPORT*/ SettingSlider : public Window
  {
  private:
    static SettingProperties::ConfigType d_ConfigType;
    static SettingProperties::ConfigName d_ConfigName;
    static SettingProperties::EventName d_EventName;
    static SettingProperties::MinimumValue d_MinimumValue;

  private:
    virtual void onTextChanged(WindowEventArgs& e);
    virtual void onFontChanged(WindowEventArgs& e);

  private:
    bool onSliderChanged(const EventArgs& e);

  private:
    void Update();
    PT::GUI::Setting setting;
    String configType;
    String configName;
    String eventName;
    float minVal;

  protected:
    virtual bool testClassName_impl(const String& class_name) const
    {
      if (class_name=="SettingSlider")	return true;
      return Window::testClassName_impl(class_name);
    }

    Window* getNameW() const;
    Slider* getSliderW() const; 
    Window* getValueW() const;

  public:
    /// Namespace for global events
    static const String EventNamespace;	
    /// Window factory name
    static const String WidgetTypeName; 

    static const String NameNameSuffix;
    static const String SliderNameSuffix;
    static const String ValueNameSuffix;

  public:
    virtual bool isHit(const Point& position) const {return false;}
    virtual void initialiseComponents(void);

  public:
    SettingSlider(const String& type, const String& name);
    virtual ~SettingSlider(void);

    const String& getConfigType() const;
    void setConfigType(const String& value);

    const String& getConfigName() const;
    void setConfigName(const String& value);

    const String& getEventName() const;
    void setEventName(const String& value);

    float getMinimumValue() const;
    void setMinimumValue(const float& value);
  };

  CEGUI_DECLARE_WINDOW_FACTORY(SettingSlider);

} // end CEGUI

#if defined(_MSC_VER)
#	pragma warning(pop)
#endif

#endif	// SETTINGSLIDER_H
