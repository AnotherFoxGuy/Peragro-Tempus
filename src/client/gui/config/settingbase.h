
#ifndef SETTINGBASE_H
#define SETTINGBASE_H

//#include <CEGUI.h>
#include <CEGUIBase.h>
#include <CEGUIWindow.h>
#include <CEGUIWindowFactory.h>

#include "setting.h"
#include "settingproperties.h"


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

namespace CEGUI
{
  /*!
  \brief
    Base class for a Setting widget
  */
  class /*CEGUIEXPORT*/ SettingBase : public Window
  {
  private:
    static SettingProperties::ConfigType d_ConfigType;
    static SettingProperties::ConfigName d_ConfigName;
    static SettingProperties::EventName d_EventName;

  private:
    virtual void onTextChanged(WindowEventArgs& e);
    virtual void onFontChanged(WindowEventArgs& e);

  protected:
    virtual void Update() = 0;
    PT::GUI::Setting setting;
    String configType;
    String configName;
    String eventName;

  protected:
    Window* getNameW() const;

  public:
    static const String NameNameSuffix;

  public:
    virtual bool isHit(const Point& position) const {return false;}
    //virtual void initialiseComponents(void);

  public:
    SettingBase(const String& type, const String& name);
    virtual ~SettingBase(void);

    const String& getConfigType() const;
    void setConfigType(const String& value);

    const String& getConfigName() const;
    void setConfigName(const String& value);

    const String& getEventName() const;
    void setEventName(const String& value);
  };
} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // SETTINGBASE_H
