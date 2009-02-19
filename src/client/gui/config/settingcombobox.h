
#ifndef SETTINGCOMBOBOX_H
#define SETTINGCOMBOBOX_H

#include "settingbase.h"

#include <map>


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

namespace CEGUI
{
  /*!
  \brief
    Base class for the SettingComboBox widget
  */
  class /*CEGUIEXPORT*/ SettingComboBox : public SettingBase
  {
  private:
    static SettingProperties::Values d_Values;

  private:
    virtual void onFontChanged(WindowEventArgs& e);
    virtual void onSized(WindowEventArgs& e);

  private:
    bool onSelectionAccepted(const EventArgs& e);

  private:
    virtual void Update();
    String values;

    String GetValue(const String& key);
    String GetKey(const String& value);
    std::vector<String> GetKeys();

  protected:
    virtual bool testClassName_impl(const String& class_name) const
    {
      if (class_name=="SettingComboBox") return true;
      return Window::testClassName_impl(class_name);
    }

    Combobox* getComboBoxW() const;

  public:
    /// Namespace for global events
    static const String EventNamespace;
    /// Window factory name
    static const String WidgetTypeName;

    static const String ComboboxSuffix;

  public:
    virtual void initialiseComponents(void);

  public:
    SettingComboBox(const String& type, const String& name);
    virtual ~SettingComboBox(void);

    String getValues() const;
    void setValues(const String& value);
  };

  CEGUI_DECLARE_WINDOW_FACTORY(SettingComboBox);

} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // SETTINGCOMBOBOX_H
