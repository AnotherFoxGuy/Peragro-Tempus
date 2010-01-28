
#include "object.h"
#include "inventory.h"
#include "iconrenderer.h"

#include "elements/CEGUICombobox.h"
#include "elements/CEGUIEditbox.h"
#include "CEGUIWindowManager.h"

#include "common/inventory/object.h"
#include "common/inventory/inventory.h"

#include "common/inventory/slotinventory.h"
#include "common/inventory/gridinventory.h"

namespace CEGUI
{
  const String Object::EventNamespace("Inventory");
  const String Object::WidgetTypeName("CEGUI/Object");

  //Child Widget name suffix constants
  const String Object::ImageNameSuffix( "__auto_image__" );

  CEGUI_DEFINE_WINDOW_FACTORY(Object)

  Object::Object(const String& type, const String& name) 
    : DragContainer (type, name), object(0), iconrenderer(0)
  {
  }

  Object::~Object()
  {
    //if (iconrenderer) iconrenderer->UnRegister(this);
  }

  void Object::initialiseComponents() 
  {
    printf("Object::initialiseComponents.\n");

    this->subscribeEvent(CEGUI::Window::EventParentSized,
      CEGUI::Event::Subscriber(&Object::OnEventParentSized, this));

    this->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
          CEGUI::Event::Subscriber(&Object::HandleDragDropped, this));

    this->setDraggingEnabled(true);

    Window* win = getImageW();
    win->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
      CEGUI::Event::Subscriber(&Object::HandleDragDropped, this));
  }

  Window* Object::getImageW() const
  {
    return static_cast<Window*>(WindowManager::getSingleton().getWindow(getName() + ImageNameSuffix));
  }

  bool Object::OnEventParentSized(const CEGUI::EventArgs& e)
  {
    SetPositionAndSize();
    return true;
  } // end OnEventSized()

  bool Object::HandleDragDropped(const CEGUI::EventArgs& args)
  {
    using namespace CEGUI;

    const DragDropEventArgs& ddea =
      static_cast<const DragDropEventArgs&>(args);

    if (ddea.window && ddea.window->getParent())
      ddea.window->getParent()->notifyDragDropItemDropped(ddea.dragDropItem);

    printf("Object::HandleDragDropped.\n");

    return true;
  } // end handleDragDroppedIcon()

  Common::Inventory::Object* Object::getObject() const
  {
    return object;
  }

  void Object::setObject(Common::Inventory::Object* obj)
  {
    object = obj;
    //this->setTooltipType("ice/Tooltip");
    //this->setTooltipText(object->GetObjectName().c_str());
  }

  void Object::SetPositionAndSize()
  {
    if (!object) return;
    if (!getParent()) return;
    CEGUI::Inventory* p = static_cast<CEGUI::Inventory*>(getParent());
    if (!p) return;
    float width = p->GetColumnWidth();
    float height = p->GetRowHeight();
    this->setPosition( UVector2(UDim(0.0f, width*position.column), UDim(0.0f, height*position.row)) );

    Common::Inventory::SlotInventory* s = dynamic_cast<Common::Inventory::SlotInventory*>(p->GetInventory().get());
    if (s)
      this->setSize( UVector2(UDim(0.0f, width), UDim(0.0f, height)) );
    Common::Inventory::GridInventory* g = dynamic_cast<Common::Inventory::GridInventory*>(p->GetInventory().get());
    if (g)
    {
      const Common::Inventory::Size& size = object->GetSize();
      this->setSize( UVector2(UDim(0.0f, width*size.width), UDim(0.0f, height*size.height)) );
    }
    CEGUI::Size winsize = this->getPixelSize();
    
    printf("SetPositionAndSize (%d, %d) (%f, %f)\n", position.column, position.row, winsize.d_width, winsize.d_height);
  }

  const Common::Inventory::PositionRef& Object::getPositionRef() const
  {
    return position;
  }

  void Object::setPositionRef(const Common::Inventory::PositionRef& pos)
  {
    position = pos;
    SetPositionAndSize();
  }

  void Object::setIconRenderer(IconRenderer* r)
  {
    if (iconrenderer) iconrenderer->UnRegister(this);
    iconrenderer = r;
     
    if (iconrenderer) iconrenderer->Register(this);
  }

} // end CEGUI
