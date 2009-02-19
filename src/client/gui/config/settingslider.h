
#ifndef SETTINGSLIDER_H
#define SETTINGSLIDER_H

#include "settingbase.h"


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

namespace CEGUI
{
  /*!
  \brief
    Base class for the SettingSlider widget
  */
  class /*CEGUIEXPORT*/ SettingSlider : public SettingBase
  {
  private:
    static SettingProperties::MinimumValue d_MinimumValue;

  private:
    virtual void onFontChanged(WindowEventArgs& e);

  private:
    bool onSliderChanged(const EventArgs& e);

  private:
    virtual void Update();
    float minVal;

  protected:
    virtual bool testClassName_impl(const String& class_name) const
    {
      if (class_name=="SettingSlider") return true;
      return Window::testClassName_impl(class_name);
    }

    Slider* getSliderW() const;
    Window* getValueW() const;

  public:
    /// Namespace for global events
    static const String EventNamespace;
    /// Window factory name
    static const String WidgetTypeName;

    static const String SliderNameSuffix;
    static const String ValueNameSuffix;

  public:
    virtual void initialiseComponents(void);

  public:
    SettingSlider(const String& type, const String& name);
    virtual ~SettingSlider(void);

    float getMinimumValue() const;
    void setMinimumValue(const float& value);
  };

  CEGUI_DECLARE_WINDOW_FACTORY(SettingSlider);

} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // SETTINGSLIDER_H
