
#ifndef CSICONRENDERER_H
#define CSICONRENDERER_H

#include <crystalspace.h>
#include "common/inventory-cegui/object.h"
#include "common/inventory-cegui/iconrenderer.h"

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
  csRef<iSector> room;

public:
  CSIconRenderer(iObjectRegistry* object_reg) 
  {
    csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));
    widgetmgr = csLoadPluginCheck<iR2TWidgetManager> (plugin_mgr, "peragro.r2twidgetmanager");
    if (widgetmgr.IsValid())
      object_reg->Register (widgetmgr, "iR2TWidgetManager");
    else
      printf("Failed to load the iR2TWidgetManager!\n");

    engine = csQueryRegistry<iEngine> (object_reg);
    loader = csQueryRegistry<iLoader> (object_reg);
    room = engine->CreateSector("CSIconRenderer");
    room->SetDynamicAmbientLight(csColor(1,1,1));

    // Now we need light to see something.
    csRef<iLight> light;
    iLightList* ll = room->GetLights ();

    light = engine->CreateLight (0, csVector3 (-3, 5, 0), 10, csColor (1, 0, 0));
    ll->Add (light);

    light = engine->CreateLight (0, csVector3 (3, 5,  0), 10, csColor (0, 0, 1));
    ll->Add (light);

    light = engine->CreateLight (0, csVector3 (0, 5, -3), 10, csColor (0, 1, 0));
    ll->Add (light);
  }

  virtual ~CSIconRenderer() {}

  iMeshWrapper* LoadMesh (const char* file, const char* factName)
  {
    if (!loader->LoadLibraryFile(file))
    {
      printf ("Error LoadMesh(%s, %s)!\n", file, factName);
      return 0;
    }
    else
      printf ("Success LoadMesh(%s, %s)!\n", file, factName);

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
