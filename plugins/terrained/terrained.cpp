/*
    Copyright (C) 2008 Development Team of Peragro Tempus

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <csutil/ref.h>
#include <iutil/document.h>
#include <iutil/vfs.h>
#include <iutil/plugin.h>

#include <ivideo/graph3d.h>

#include <iengine/engine.h>
#include <iengine/material.h>
#include <iutil/object.h>
#include <ivideo/material.h>

#include "csutil/custom_new_disable.h"
#include <CEGUI.h>
#include <CEGUIWindowManager.h>
#include <CEGUILogger.h>
#include "csutil/custom_new_enable.h"

#include "terrained.h"

#if defined(__GNUC__)
#define SIZE_T_FORMAT_SPECIFIER "z"
#else
#define SIZE_T_FORMAT_SPECIFIER "l"
#endif

SCF_IMPLEMENT_FACTORY (TerrainEd)

TerrainEd::TerrainEd(iBase* parent) :
scfImplementationType (this, parent), object_reg(0)
{
}

TerrainEd::~TerrainEd()
{
}

bool TerrainEd::Initialize (iObjectRegistry* r)
{
  printf("==============================================================================\n");

  object_reg = r;

  csRef<iPluginManager> plugin_mgr (csQueryRegistry<iPluginManager> (object_reg));

  cegui =  csQueryRegistry<iCEGUI> (object_reg);
  if (!cegui.IsValid())
  {
    printf("I: Loading CEGUI plugin!\n");
    cegui = csLoadPlugin<iCEGUI> (plugin_mgr, "crystalspace.cegui.wrapper");
  }

  if (!cegui.IsValid())
  {
    printf("E: Failed to locate CEGUI plugin!\n");
    return false;
  }

  csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (object_reg);
  if (!g3d)
  {
    printf("E: Failed to locate iGraphics3D plugin!\n");
    return false;
  }

  if (!g3d->Open())
  {
    printf("E: Failed to open iGraphics3D plugin!\n");
    return false;
  }

  cegui->Initialize ();

  // Set the logging level
  cegui->GetLoggerPtr ()->setLoggingLevel(CEGUI::Informative);

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  vfs->ChDir ("/ceguitest/0.5/");

  // Load the ice skin (which uses Falagard skinning system)
  cegui->GetSchemeManagerPtr ()->loadScheme("ice.scheme");

  cegui->GetSystemPtr ()->setDefaultMouseCursor("ice", "MouseArrow");

  CEGUI::Font* font = cegui->GetFontManagerPtr ()->createFont("FreeType",
    "Vera", "/fonts/ttf/Vera.ttf");
  font->setProperty("PointSize", "10");
  font->load();

  //CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();

  // Create the Root window and set it as the GUI sheet.
  CEGUI::Window* root = createWindow("DefaultWindow", "Root");
  root->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
    CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
  root->setMaxSize(CEGUI::UVector2(CEGUI::UDim(1.0f,0), CEGUI::UDim(1.0f,0)));
  cegui->GetSystemPtr()->setGUISheet(root);

  CEGUI::Window* win = createWindow("ice/FrameWindow", "Frame");
  root->addChildWindow(win);
  win->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
    CEGUI::UVector2(CEGUI::UDim(0.45f,0), CEGUI::UDim(0.35f,0)));


  CEGUI::Window* box = createWindow("ice/Combobox", "Frame/Combobox");
  win->addChildWindow(box);
  box->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,10), CEGUI::UDim(0.0f,30)),
    CEGUI::UVector2(CEGUI::UDim(1.0f,-20), CEGUI::UDim(0.0f,130)));
  ((CEGUI::Combobox*)box)->setReadOnly(true);
  box->subscribeEvent(CEGUI::Combobox::EventListSelectionAccepted,
          CEGUI::Event::Subscriber(&TerrainEd::OnDropList, this));


  CEGUI::Window* list = createWindow("ice/StaticImage", "Frame/Listbox");
  win->addChildWindow(list);
  list->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,10), CEGUI::UDim(0.0f,65)),
    CEGUI::UVector2(CEGUI::UDim(1.0f,-20), CEGUI::UDim(1.0f,-70)));
  list->setProperty("FrameEnabled", "False");
  list->setProperty("BackgroundEnabled", "False");

  // Register for frame events.
  csBaseEventHandler::Initialize (object_reg);
  RegisterQueue (object_reg, csevFrame(object_reg));

  printf("==============================================================================\n");

  return true;
} // end Initialize()

CEGUI::Window* TerrainEd::createWindow(const char* type, const char* name)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* win;

  if (winMgr->isWindowPresent(name))
    win = winMgr->getWindow(name);
  else
    win = winMgr->createWindow(type, name);

  return win;
} // end createWindow()

void TerrainEd::AddMaterials()
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iMaterialList> list = engine->GetMaterialList ();
  if (numberOfTex >= (size_t)list->GetCount()) return;

  ClearMaterials();

  printf("==============================================================================\n");
  printf("I: Adding %d materials!\n", list->GetCount());
  printf("==============================================================================\n");

  for (int i = 0; i < list->GetCount(); i++)
  {
    csRef<iMaterialWrapper> mat = list->Get(i);
    AddMaterial(i, mat);
  }

  numberOfTex = (size_t)list->GetCount();

} // end AddMaterials()

void TerrainEd::AddMaterial(uint i, iMaterialWrapper* mat)
{
  csString name = mat->QueryObject()->GetName();
  printf("I: Adding material %d %s!\n", i, name.GetData());

  if (name.IsEmpty())
    name = "empty name";

  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* box = winMgr->getWindow("Frame/Combobox");
  CEGUI::Combobox* combo = static_cast<CEGUI::Combobox*>(box);

  //mat->QueryObject()->GetName()

  CEGUI::ListboxTextItem* item = new CEGUI::ListboxTextItem(name.GetData(), i);

  item->setTextColours(CEGUI::colour(0,0,0,1));
  item->setSelectionBrushImage((CEGUI::utf8*)"ice", (CEGUI::utf8*)"TextSelectionBrush");

  combo->addItem(item);

} // end AddMaterial()

void TerrainEd::ClearMaterials()
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* box = winMgr->getWindow("Frame/Combobox");
  CEGUI::Combobox* combo = static_cast<CEGUI::Combobox*>(box);

  combo->resetList();

} // end ClearMaterials()

bool TerrainEd::OnDropList(const CEGUI::EventArgs& e)
{
  ClearShaderVars();

  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* btn = winMgr->getWindow("Frame/Combobox");
  csString name = ((CEGUI::Combobox*)btn)->getSelectedItem()->getText().c_str();

  csRef<iStringSet> stringSet = csQueryRegistryTagInterface<iStringSet> (object_reg, "crystalspace.shared.stringset");

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iMaterialList> list = engine->GetMaterialList ();

  csRef<iMaterialWrapper> mat = list->FindByName(name.GetData());

  if (mat)
  {
    csRef<iMaterial> m = mat->GetMaterial();
    if (m.IsValid())
    {
      printf("R: Adding shadervars for %s!\n", name.GetData());

      csRefArray<csShaderVariable> vars = m->GetShaderVariables ();
      uint index = 0;
      for (size_t i = 0; i < vars.GetSize(); i++)
      {
        csRef<csShaderVariable> var = vars.Get(i);
        if (AddShaderVar(index, var))
          index += 1;
      }
    }
  }

  return true;
} // end OnDropList()

bool TerrainEd::AddShaderVar(uint i, csShaderVariable* var)
{
  csRef<iShaderVarStringSet> stringSet = csQueryRegistryTagInterface<iShaderVarStringSet> (object_reg, "crystalspace.shader.variablenameset");

  CS::ShaderVarStringID id = var->GetName();
  csShaderVariable::VariableType type = var->GetType();
  csString name = stringSet->Request(id);
  printf("I: Adding shadervar %d %s %d!\n", i, name.GetData(), type);

  switch (type)
  {
  case csShaderVariable::INT :
    {
      int ival;
      if (var->GetValue(ival))
      {
        csArray<float> vals;
        vals.Push((float)ival);
        CEGUI::Window* win = CreateShaderSpinner(i, name.GetData(), vals, false);
        win->setUserData(var);
      }
    }
    break;

  case csShaderVariable::FLOAT :
    {
      float fval;
      if (var->GetValue(fval))
      {
        csArray<float> vals;
        vals.Push(fval);
        CEGUI::Window* win = CreateShaderSpinner(i, name.GetData(), vals, true);
        win->setUserData(var);
      }
    }
    break;

  case csShaderVariable::VECTOR2 :
    {
      csVector2 v2val;
      if (var->GetValue(v2val))
      {
        csArray<float> vals;
        vals.Push(v2val.x);
        vals.Push(v2val.y);
        CEGUI::Window* win = CreateShaderSpinner(i, name.GetData(), vals, true);
        win->setUserData(var);
      }
    }
    break;

  case csShaderVariable::VECTOR3 :
    {
      csVector3 v3val;
      if (var->GetValue(v3val))
      {
        csArray<float> vals;
        vals.Push(v3val.x);
        vals.Push(v3val.y);
        vals.Push(v3val.z);
        CEGUI::Window* win = CreateShaderSpinner(i, name.GetData(), vals, true);
        win->setUserData(var);
      }
    }
    break;

  case csShaderVariable::VECTOR4 :
    {
      csVector4 v4val;
      if (var->GetValue(v4val))
      {
        csArray<float> vals;
        vals.Push(v4val.x);
        vals.Push(v4val.y);
        vals.Push(v4val.z);
        vals.Push(v4val.w);
        CEGUI::Window* win = CreateShaderSpinner(i, name.GetData(), vals, true);
        win->setUserData(var);
      }
    }
    break;

  default :
    {
      printf("E: Type not supported!\n");
      return false;
    }
  }

  return true;

} // end AddShaderVar()

CEGUI::Window* TerrainEd::CreateShaderSpinner(uint i, const char* name, csArray<float>& vals, bool flt)
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* list = winMgr->getWindow("Frame/Listbox");

  csString wname = "Frame/Listbox/";
  wname += name;

  csString sname = wname + "/Spinner";
  csString tname = wname + "/StaticText";

  CEGUI::Window* win = createWindow("ice/StaticImage", wname.GetData());
  list->addChildWindow(win);
  win->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,10), CEGUI::UDim(0.0f,(30*i))),
    CEGUI::UVector2(CEGUI::UDim(1.0f,-20), CEGUI::UDim(0.0f,30)));
  win->setProperty("FrameEnabled", "False");
  win->setProperty("BackgroundEnabled", "False");

  int spinnerWidth = 60;
  int totalOffsset = (int)vals.GetSize()*spinnerWidth;

  CEGUI::Window* text = createWindow("ice/StaticText", tname.GetData());
  win->addChildWindow(text);
  text->setArea(CEGUI::UVector2(CEGUI::UDim(0.0f,0), CEGUI::UDim(0.0f,0)),
    CEGUI::UVector2(CEGUI::UDim(1.0f,-totalOffsset), CEGUI::UDim(1.0f,0)));
  text->setText(name);
  text->setID(7);

  for (int t = 0; t < (int)vals.GetSize(); t++)
  {
    char buffer[8];
    snprintf(buffer, 8, "/%d", t);
    csString rname = sname + buffer;

    CEGUI::Window* spinner = createWindow("ice/Spinner", rname.GetData());
    win->addChildWindow(spinner);
    spinner->setArea(CEGUI::UVector2(CEGUI::UDim(1.0f,-(totalOffsset - (t*spinnerWidth))), CEGUI::UDim(0.0f,0)),
      CEGUI::UVector2(CEGUI::UDim(0.0f, spinnerWidth), CEGUI::UDim(1.0f,0)));

    ((CEGUI::Spinner*)spinner)->setCurrentValue(vals[t]);
    if (flt)
    {
      ((CEGUI::Spinner*)spinner)->setStepSize(0.01f);
      ((CEGUI::Spinner*)spinner)->setTextInputMode(CEGUI::Spinner::FloatingPoint);
    }
    else
    {
      ((CEGUI::Spinner*)spinner)->setStepSize(1.0f);
      ((CEGUI::Spinner*)spinner)->setTextInputMode(CEGUI::Spinner::Integer);
    }

    spinner->subscribeEvent(CEGUI::Spinner::EventValueChanged,
      CEGUI::Event::Subscriber(&TerrainEd::OnShaderSpinner, this));

    spinner->setID(t);
  }

  return win;
}

void TerrainEd::ClearShaderVars()
{
  CEGUI::WindowManager* winMgr = cegui->GetWindowManagerPtr ();
  CEGUI::Window* list = winMgr->getWindow("Frame/Listbox");

  for (size_t i = 0; i < list->getChildCount(); i++)
  {
    CEGUI::Window* w = list->getChildAtIdx(i);
    printf("I: Destroying window %s\n", w->getName().c_str());
    winMgr->destroyWindow(w);
  }
} // end ClearShaderVars()

csArray<float> TerrainEd::GetValues(CEGUI::Window* win)
{
  csArray<float> vals;

  printf("var :");
  for (size_t i = 0; i < 4; i++)
  {
    if (win->isChild(i))
    {
      CEGUI::Window* spinner = win->getChild(i);
      float v = ((CEGUI::Spinner*)spinner)->getCurrentValue();
      vals.Push( v );
      printf("%f, ", v);
    }
  }
  printf("\n");

  return vals;
} // end GetValues()

bool TerrainEd::OnShaderSpinner(const CEGUI::EventArgs& e)
{
  const CEGUI::WindowEventArgs& we = static_cast<const CEGUI::WindowEventArgs&>(e);

  CEGUI::Window* win = we.window->getParent();

  csShaderVariable* var = static_cast<csShaderVariable*>(win->getUserData());

  if (!var)
  {
    printf("E: Failed to locate csShaderVariable!\n");
    return true;
  }

  csArray<float> vals = GetValues(win);

  csShaderVariable::VariableType type = var->GetType();
  printf("I: Setting var of type %d  len %" SIZE_T_FORMAT_SPECIFIER "u!\n", type, vals.GetSize());
  switch (type)
  {
  case csShaderVariable::INT :
    {
      int val = static_cast<int>(vals[0]);
      var->SetValue(val);
    }
    break;

  case csShaderVariable::FLOAT :
    {
      float val = vals[0];
      var->SetValue(val);
    }
    break;

  case csShaderVariable::VECTOR2 :
    {
      csVector2 val(vals[0], vals[1]);
      var->SetValue(val);
    }
    break;

  case csShaderVariable::VECTOR3 :
    {
      csVector3 val(vals[0], vals[1], vals[2]);
      var->SetValue(val);
    }
    break;

  case csShaderVariable::VECTOR4 :
    {
      csVector4 val(vals[0], vals[1], vals[2], vals[3]);
      var->SetValue(val);
    }
    break;

  default :
    {
      printf("E: Type not supported!\n");
      return false;
    }
  }

  return true;
} // end OnShaderSpinner()

void TerrainEd::Frame()
{
  AddMaterials();
  cegui->Render();
} // end Frame()

