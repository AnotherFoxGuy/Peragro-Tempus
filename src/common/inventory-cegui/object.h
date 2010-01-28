
#ifndef OBJECT_H
#define OBJECT_H

//#include <CEGUI.h>
#include <CEGUIBase.h>
#include <CEGUIWindow.h>
#include <elements/CEGUIDragContainer.h>
#include <CEGUIWindowFactory.h>


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

#include "common/inventory/object.h"
#include "common/inventory/positionref.h"

namespace CEGUI
{
  struct IconRenderer;

  /*!
  \brief
    Class for a Object widget
  */
  class /*CEGUIEXPORT*/ Object : public DragContainer 
  {
  private:
    /// Fix Object size and location when the parent window changes.
    bool OnEventParentSized(const CEGUI::EventArgs& e);
    /// For Object swapping, passes DragDropped event to Object's parent.
    bool HandleDragDropped(const CEGUI::EventArgs& args);

  private:
    void SetPositionAndSize();

  protected:
  	Common::Inventory::Object* object;
    Common::Inventory::PositionRef position;

    IconRenderer* iconrenderer;

  protected:
    virtual bool testClassName_impl(const String& class_name) const
    {
      if (class_name=="Object") return true;
      return Window::testClassName_impl(class_name);
    }

    virtual void initialiseComponents();

  public:
    /// Namespace for global events
    static const String EventNamespace;
    /// Window factory name
    static const String WidgetTypeName;
    ///
    static const String ImageNameSuffix;

  public:
    Object(const String& type, const String& name);
    virtual ~Object();

    Common::Inventory::Object* getObject() const;
    void setObject(Common::Inventory::Object* obj);

    const Common::Inventory::PositionRef& getPositionRef() const;
    void setPositionRef(const Common::Inventory::PositionRef& pos);

    void setIconRenderer(IconRenderer*);

    Window* getImageW() const;
  };

  CEGUI_DECLARE_WINDOW_FACTORY(Object);

} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // OBJECT_H
