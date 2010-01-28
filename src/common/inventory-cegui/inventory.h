
#ifndef INVENTORY_H
#define INVENTORY_H

//#include <CEGUI.h>
#include <CEGUIBase.h>
#include <CEGUIWindow.h>
#include <CEGUIWindowFactory.h>


#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 4251)
#endif

#include <boost/weak_ptr.hpp>

#include "common/inventory/inventory.h"

namespace CEGUI
{
  struct IconRenderer;

  /*!
  \brief
    Class for a Inventory widget
  */
  class /*CEGUIEXPORT*/ Inventory : public Window, public Common::Inventory::InventoryCallBack
  {
  private:
    bool OnEventSized(const CEGUI::EventArgs& args);
    bool OnEventParentSized(const CEGUI::EventArgs& args);

    bool handleDragEnter(const CEGUI::EventArgs& args);
    bool handleDragLeave(const CEGUI::EventArgs& args);
    bool handleDragDropped(const CEGUI::EventArgs& args);

  private:
    virtual void ObjectAdded(boost::shared_ptr<Common::Inventory::Object>, const Common::Inventory::PositionRef&);
    virtual void ObjectRemoved(boost::shared_ptr<Common::Inventory::Object>, const Common::Inventory::PositionRef&);
    virtual void ObjectMoved(boost::shared_ptr<Common::Inventory::Object>, const Common::Inventory::PositionRef&,
                             boost::shared_ptr<Common::Inventory::Object>, const Common::Inventory::PositionRef&);

  private:
    struct DefaultInputInventoryCallBack : public Common::Inventory::InventoryCallBack
    {
      Common::Inventory::Inventory* inventory;
      DefaultInputInventoryCallBack(Common::Inventory::Inventory* inv) : inventory(inv) {}

      virtual void ObjectAdded(boost::shared_ptr<Common::Inventory::Object> o, const Common::Inventory::PositionRef& r)
      { if (!inventory->AddObjectAt(r, o)) printf("ERROR: DefaultInputInventoryCallBack::ObjectAdded()\n"); }

      virtual void ObjectRemoved(boost::shared_ptr<Common::Inventory::Object> o, const Common::Inventory::PositionRef& r)
      { if (!inventory->RemoveObjectAt(r)) printf("ERROR: DefaultInputInventoryCallBack::RemoveObjectAt()\n"); }

      virtual void ObjectMoved(boost::shared_ptr<Common::Inventory::Object> o1, const Common::Inventory::PositionRef& r1, 
                               boost::shared_ptr<Common::Inventory::Object> o2, const Common::Inventory::PositionRef& r2)
      { if (!inventory->MoveObject(r1, r2)) printf("ERROR: DefaultInputInventoryCallBack::MoveObject()\n"); }
    };
    Common::Inventory::InventoryCallBack* inputcallback;

  protected:
    boost::weak_ptr<Common::Inventory::Inventory> inventory;

    IconRenderer* iconrenderer;

  protected:
    virtual bool testClassName_impl(const String& class_name) const
    {
      if (class_name=="Inventory") return true;
      return Window::testClassName_impl(class_name);
    }

    virtual void initialiseComponents();

    void SetPositionAndSize();

  public:
    /// Namespace for global events
    static const String EventNamespace;
    /// Window factory name
    static const String WidgetTypeName;
    ///
    static const String ImageNameSuffix;

  public:
    Inventory(const String& type, const String& name);
    virtual ~Inventory();

    boost::shared_ptr<Common::Inventory::Inventory> GetInventory() const;
    void SetInventory(boost::shared_ptr<Common::Inventory::Inventory> inv, Common::Inventory::InventoryCallBack* cb = 0);

    float GetColumnWidth();
    float GetRowHeight();

    Common::Inventory::PositionRef WindowToRef(Window*);

    void setIconRenderer(IconRenderer*);

    Window* getImageW() const;
  };

  CEGUI_DECLARE_WINDOW_FACTORY(Inventory);

} // end CEGUI

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif

#endif // INVENTORY_H
