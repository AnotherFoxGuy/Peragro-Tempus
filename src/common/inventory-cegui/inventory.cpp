
#include "inventory.h"

#include "object.h"

#include "elements/CEGUICombobox.h"
#include "elements/CEGUIEditbox.h"
#include "CEGUIWindowManager.h"
#include "CEGUIImagesetManager.h"
#include "CEGUIImageset.h"

#include "CEGUIFontManager.h"
#include "CEGUITexture.h"
#include "CEGUICoordConverter.h"

#include "CEGUIPropertyHelper.h"

#include "common/inventory/object.h"
#include "common/inventory/inventory.h"

namespace CEGUI
{
  const String Inventory::EventNamespace("Inventory");
  const String Inventory::WidgetTypeName("CEGUI/Inventory");

  //Child Widget name suffix constants
  const String Inventory::ImageNameSuffix( "__auto_image__" );

  CEGUI_DEFINE_WINDOW_FACTORY(Inventory)

  Inventory::Inventory(const String& type, const String& name) 
    : Window(type, name), inputcallback(0), iconrenderer(0)
  {
    this->subscribeEvent(CEGUI::Window::EventSized,
      CEGUI::Event::Subscriber(&Inventory::OnEventSized, this));
    this->subscribeEvent(CEGUI::Window::EventParentSized,
      CEGUI::Event::Subscriber(&Inventory::OnEventParentSized, this));

    this->subscribeEvent(CEGUI::Window::EventDragDropItemEnters,
      CEGUI::Event::Subscriber(&Inventory::handleDragEnter, this));
    this->subscribeEvent(CEGUI::Window::EventDragDropItemLeaves,
      CEGUI::Event::Subscriber(&Inventory::handleDragLeave, this));
    this->subscribeEvent(CEGUI::Window::EventDragDropItemDropped,
      CEGUI::Event::Subscriber(&Inventory::handleDragDropped, this));
  }

  void Inventory::initialiseComponents()
  {
    SetPositionAndSize();
  }

  void Inventory::SetPositionAndSize()
  {
    printf("inventory::SetPositionAndSize\n");
    if (!inventory.lock()) return;
    Imageset& set = ImagesetManager::getSingleton().get("Inventory");
    if (set.isImageDefined(getName()+"_Slot"))
      set.undefineImage(getName()+"_Slot");
    Texture* t = set.getTexture();
    float width = t->getOriginalDataSize().d_width * inventory.lock()->GetColumnCount();
    float height = t->getOriginalDataSize().d_height * inventory.lock()->GetRowCount();
    
    set.defineImage(getName()+"_Slot", Rect(0,0,width,height), Point(0,0));
    this->getImageW()->setProperty("Image", CEGUI::PropertyHelper::imageToString(&set.getImage(getName()+"_Slot")));
  }

  Inventory::~Inventory()
  {
    DefaultInputInventoryCallBack* i = dynamic_cast<DefaultInputInventoryCallBack*>(inputcallback);
    if (i) delete i;

    if (inventory.lock()) inventory.lock()->RemoveInventoryCallBack(this);
  }

  bool Inventory::OnEventSized(const CEGUI::EventArgs& e)
  {
    SetPositionAndSize();
    return true;
  } // end OnEventSized()

  bool Inventory::OnEventParentSized(const CEGUI::EventArgs& e)
  {
    SetPositionAndSize();
    return true;
  } // end OnEventSized()

  bool Inventory::handleDragEnter(const CEGUI::EventArgs& args)
  {
    using namespace CEGUI;

    return true;
  } // end handleDragEnter()

  bool Inventory::handleDragLeave(const CEGUI::EventArgs& args)
  {
    using namespace CEGUI;

    return true;
  } // end handleDragLeave()

  bool Inventory::handleDragDropped(const CEGUI::EventArgs& args)
  {
    using namespace CEGUI;

    const DragDropEventArgs& ddea =
      static_cast<const DragDropEventArgs&>(args);

    CEGUI::Object* o = static_cast<CEGUI::Object*>(ddea.dragDropItem);
    if (!o) return true;

    Common::Inventory::PositionRef curref = o->getPositionRef();
    Common::Inventory::PositionRef ref = WindowToRef(o);

    printf("Inventory::handleDragDropped in %s at %d, %d\n", inventory.lock()->GetName().c_str(), ref.column, ref.row);

    if (o->getObject()->GetParent() == inventory.lock().get())
    {
      if (curref == ref)
        return true; // Same position and same inventory, do nothing.
      boost::shared_ptr<Common::Inventory::Object> obj1 = o->getObject()->GetParent()->GetObjectAt(curref);
      boost::shared_ptr<Common::Inventory::Object> obj2;
      inputcallback->ObjectMoved(obj1, curref, obj2, ref);
    }
    else if (o->getObject()->GetParent())
    {
      //TODO: change pointer in Object to weak_ref
      boost::shared_ptr<Common::Inventory::Object> obj = o->getObject()->GetParent()->GetObjectAt(curref); 
      if (obj)  
        inputcallback->ObjectAdded(obj, ref);
      else
        printf("ERROR: Inventory::handleDragDropped Moved2\n");
    }
    else
    {
      printf("ERROR: Inventory::handleDragDropped Moved3\n");
    }

    return true;
  } // end handleDragDropped()

  boost::shared_ptr<Common::Inventory::Inventory> Inventory::GetInventory() const
  {
    return inventory.lock();
  }

  void Inventory::SetInventory(boost::shared_ptr<Common::Inventory::Inventory> inv, Common::Inventory::InventoryCallBack* cb)
  {
    inventory = inv;

    inputcallback = cb;
    if (!inputcallback)
    {
      inputcallback = new DefaultInputInventoryCallBack(inventory.lock().get());
    }

    std::list<Common::Inventory::PositionRef> positions; 
    std::list<boost::shared_ptr<Common::Inventory::Object> > objects;
    inventory.lock()->GetObjects(positions, objects);

    std::list<Common::Inventory::PositionRef>::const_iterator itp = positions.begin();
    std::list<boost::shared_ptr<Common::Inventory::Object> >::const_iterator ito = objects.begin();
    for (; itp != positions.end(); itp++, ito++)
    {
      ObjectAdded(*ito, *itp);
    }

    inventory.lock()->AddInventoryCallBack(this);

    SetPositionAndSize();
  }

  void Inventory::ObjectAdded(boost::shared_ptr<Common::Inventory::Object> obj, const Common::Inventory::PositionRef& ref)
  {
    WindowManager& winmgr = WindowManager::getSingleton();
    Object* win = (Object*)winmgr.createWindow("Peragro/Object");
    win->setObject(obj.get());
    win->setPositionRef(ref);
    this->addChildWindow(win);
    win->setIconRenderer(iconrenderer);
  }

  void Inventory::ObjectRemoved(boost::shared_ptr<Common::Inventory::Object> obj, const Common::Inventory::PositionRef& ref)
  {
    for (size_t i = 0; i < this->getChildCount(); i++)
    {
      CEGUI::Object* o = static_cast<CEGUI::Object*>(this->getChildAtIdx(i));
      if (o && o->getObject() == obj.get() /*&& o->GetPositionRef() == ref*/)
      {
        //this->removeChildWindow(o);
        WindowManager* winmgr = WindowManager::getSingletonPtr();
        winmgr->destroyWindow(o);
        return;
      }
    }
  }

  void Inventory::ObjectMoved(boost::shared_ptr<Common::Inventory::Object> obj1, const Common::Inventory::PositionRef& ref1,
                              boost::shared_ptr<Common::Inventory::Object> obj2, const Common::Inventory::PositionRef& ref2)
  {
    bool first = !obj1;
    bool second = !obj2;

    for (size_t i = 0; i < this->getChildCount(); i++)
    {
      CEGUI::Object* o = static_cast<CEGUI::Object*>(this->getChildAtIdx(i));
      if (o)
      {
        if (!first && o->getObject() == obj1.get()) {o->setPositionRef(ref1); first = true;}
        if (!second && o->getObject() == obj2.get()) {o->setPositionRef(ref2); second = true;}
        if (first && second) return;
      }
    }
  }

  float Inventory::GetColumnWidth()
  {
    CEGUI::Size size = getPixelSize();
    return size.d_width/(float)inventory.lock()->GetColumnCount();
  }

  float Inventory::GetRowHeight()
  {
    CEGUI::Size size = getPixelSize();
    return size.d_height/(float)inventory.lock()->GetRowCount();
  }

  Common::Inventory::PositionRef Inventory::WindowToRef(Window* win)
  {
    Vector2 posa = CEGUI::CoordConverter::windowToScreen(*win->getParent(), win->getPosition());
    Vector2 posr = CEGUI::CoordConverter::screenToWindow(*this, posa);
    CEGUI::Size winsize = win->getPixelSize();
    int x = (int)((posr.d_x+(winsize.d_width/2))/GetColumnWidth());
    int y = (int)((posr.d_y+(winsize.d_height/2))/GetRowHeight());
    return Common::Inventory::PositionRef(x, y);
  }

  void Inventory::setIconRenderer(IconRenderer* r)
  {
    iconrenderer = r;

    for (size_t i = 0; i < this->getChildCount(); i++)
    {
      CEGUI::Object* o = dynamic_cast<CEGUI::Object*>(this->getChildAtIdx(i));
      if (o) o->setIconRenderer(iconrenderer);
    }
  }

  Window* Inventory::getImageW() const
  {
    return static_cast<Window*>(WindowManager::getSingleton().getWindow(getName() + ImageNameSuffix));
  }

} // end CEGUI
