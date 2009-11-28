
#ifndef CSICONRENDERER_H
#define CSICONRENDERER_H

#include <crystalspace.h>
#include "common/inventory/object.h"
#include "common/inventory/iconrenderer.h"

#include "ir2tmanager.h"


/*!
\brief
  Interface for IconRenderer
*/
class CSIconRenderer : public CEGUI::IconRenderer
{
private:
  csRef<iR2TWidgetManager> widgetmgr;
  csRef<iLoader> loader;
  csRef<iEngine> engine;

public:
  CSIconRenderer(iObjectRegistry* object_reg) 
  {
    csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
    widgetmgr = csLoadPlugin<iR2TWidgetManager> (plugin_mgr, "peragro.r2twidgetmanager");
    if (widgetmgr.IsValid())
      object_reg->Register (widgetmgr, "iR2TWidgetManager");
    else
      printf("Failed to load the iR2TWidgetManager!\n");

    engine = csQueryRegistry<iEngine> (object_reg);
    loader = csQueryRegistry<iLoader> (object_reg);
  }

  virtual ~CSIconRenderer() {}

  iMeshWrapper* LoadMesh (const char* file, const char* factName)
  {
    if (!loader->LoadLibraryFile(file))
      printf ("Error LoadMesh(%s, %s)!\n", file, factName);

    csRef<iSector> room = engine->FindSector("room");
    csRef<iMeshFactoryWrapper> fact = engine->FindMeshFactory(factName);
    csRef<iMeshWrapper> m = engine->CreateMeshWrapper(fact, factName, room);

    return m;
  }

  virtual bool Register(CEGUI::Object* obj)
  {
    if (!obj->getObject()) return false;
    csRef<iMeshWrapper> mesh = LoadMesh(obj->getObject()->GetFileName().c_str(), obj->getObject()->GetMeshFactName().c_str());
    mesh->GetMovable()->GetTransform().RotateOther(csVector3(0,1,0), -1.54f);
    mesh->GetMovable()->UpdateMove();
    return widgetmgr->Register(obj->getImageW(), mesh);
  }

  virtual bool UnRegister(CEGUI::Object* obj)
  {
    return widgetmgr->UnRegister(obj->getImageW());
  }
};

#endif // CSICONRENDERER_H
