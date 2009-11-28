/*
    Copyright (C) 2009 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <csutil/util.h>
#include <ivaria/reporter.h>
#include <iengine/sector.h>
#include <csgfx/textureformatstrings.h>
#include <ivideo/texture.h>

#include <ivideo/txtmgr.h>

#include <iengine/movable.h>

#include "csutil/custom_new_disable.h"
#include <CEGUI.h>
#include <ivaria/icegui.h>
#include "csutil/custom_new_enable.h"

#include <iengine/camera.h>
#include <iengine/engine.h>
#include <iengine/rendermanager.h>

#include "r2tmanager.h"

SCF_IMPLEMENT_FACTORY(R2TWidgetManager)

//----------------------------------------------------------------

R2TWidgetManager::R2TWidget::R2TWidget(R2TWidgetManager* m, CEGUI::Window* w, iMeshWrapper* o) 
  : manager(m), window(w), mesh(o), rotate(false), imageSet(0), renderer(manager->object_reg)
{
  cur_w = cur_h = -1;

  // Save original transform.
  otrans = mesh->GetMovable()->GetTransform();

  window->subscribeEvent(CEGUI::Window::EventDestructionStarted,
    CEGUI::Event::Subscriber(&R2TWidget::OnEventDestructionStarted, this));

  // Listen for all visibility changes in the hierarchy.
  CEGUI::Window* win = window;
  bool vis = true;
  while (win)
  {
    win->subscribeEvent(CEGUI::Window::EventHidden,
      CEGUI::Event::Subscriber(&R2TWidget::OnEventHidden, this));

    win->subscribeEvent(CEGUI::Window::EventShown,
      CEGUI::Event::Subscriber(&R2TWidget::OnEventShown, this));

    vis = vis ? win->isVisible() : vis;
    win = win->getParent();
  }

  // No windows in the hierarchy were hidden.
  if (vis)
  {
    window->subscribeEvent(CEGUI::Window::EventWindowUpdated,
      CEGUI::Event::Subscriber(&R2TWidget::EventWindowUpdated, this));
  }

  window->subscribeEvent(CEGUI::Window::EventSized,
    CEGUI::Event::Subscriber(&R2TWidget::OnEventSized, this));

  window->subscribeEvent(CEGUI::Window::EventMouseEnters,
    CEGUI::Event::Subscriber(&R2TWidget::OnEventMouseEnters, this));

  window->subscribeEvent(CEGUI::Window::EventMouseLeaves,
    CEGUI::Event::Subscriber(&R2TWidget::OnEventMouseLeaves, this));

  CEGUI::Size size = window->getPixelSize();
  SizeChanged(size.d_width, size.d_height);
}

R2TWidgetManager::R2TWidget::~R2TWidget()
{
  // Unsubscribe eventhandlers.
  window->removeEvent(CEGUI::Window::EventDestructionStarted);
  window->removeEvent(CEGUI::Window::EventWindowUpdated);
  window->removeEvent(CEGUI::Window::EventMouseEnters);
  window->removeEvent(CEGUI::Window::EventMouseLeaves);
  window->removeEvent(CEGUI::Window::EventSized);

  // Stop rendering to the texhandle.
  csRef<iEngine> engine = csQueryRegistry<iEngine> (manager->object_reg);
  csRef<iRenderManagerTargets> rmTargets = scfQueryInterface<iRenderManagerTargets>(engine->GetRenderManager());
  if (texh) rmTargets->UnregisterRenderTarget(texh);

  // Destroy the imageset.
  csRef<iCEGUI> cegui = csQueryRegistry<iCEGUI> (manager->object_reg);
  if (imageSet) cegui->GetImagesetManagerPtr()->destroyImageset(imageSet);

  manager = 0;
}

void R2TWidgetManager::R2TWidget::SizeChanged(int w, int h)
{ 
  if (cur_w != w || cur_h != h)
  {
    cur_w = w;
    cur_h = h;

    if (cur_w <= 0) cur_w = 2;
    if (cur_h <= 0) cur_h = 2;

    printf("SizeChanged %d %d \n", cur_w, cur_h);

    csRef<iGraphics3D> g3d = csQueryRegistry<iGraphics3D> (manager->object_reg);
    csRef<iCEGUI> cegui = csQueryRegistry<iCEGUI> (manager->object_reg);
    csRef<iEngine> engine = csQueryRegistry<iEngine> (manager->object_reg);
    csRef<iTextureManager> texman = g3d->GetTextureManager();

    // Destroy the imageset releasing its texture.
    if (imageSet) cegui->GetImagesetManagerPtr()->destroyImageset(imageSet);

    // Unregister the handler.
    csRef<iRenderManagerTargets> rmTargets = scfQueryInterface<iRenderManagerTargets>(engine->GetRenderManager());
    if (texh) rmTargets->UnregisterRenderTarget(texh);

    // Allocate a texture image.
    texh = texman->CreateTexture(cur_w, cur_h,
    csimg2D, "rgba8", CS_TEXTURE_3D);
    texh->SetAlphaType (csAlphaMode::alphaBinary);
    texh->SetTextureClass ("cegui");

    // Create CEGUI texture from CS one.
    CEGUI::Texture* ceguiTxt = cegui->CreateTexture(texh);

    // Create imageset with texture.
    imageSet = cegui->GetImagesetManagerPtr()->createImageset(window->getName()+"_ImageSet", ceguiTxt);
    imageSet->defineImage("mesh", 
      CEGUI::Point(0.0f, 0.0f),
      CEGUI::Size(ceguiTxt->getWidth(), ceguiTxt->getHeight()),
      CEGUI::Point(0.0f,0.0f));

    // Assign the set to our window.
    window->setProperty("Image", CEGUI::PropertyHelper::imageToString(&imageSet->getImage("mesh")));

    int w, h;
    texh->GetRendererDimensions (w, h);

    renderer.ScaleCamera(mesh, w, h);
/*
    CS::Math::Matrix4 perspectiveFixup;
    //float scaleX = float(cur_w)/float (w);
    //float scaleY = float(cur_h)/float (h);
    
    float scaleX = float(h)/float (cur_h);
    float scaleY = float(w)/float (cur_w);

    printf("SizeChanged1 %f %f \n", scaleX, scaleY);
    perspectiveFixup = CS::Math::Matrix4 (
      scaleX, 0, 0, 0,
      0, scaleY, 0, 0,
      0, 0, 1, 0,
      0, 0, 0, 1);

    csRef<iCustomMatrixCamera> ccam = engine->CreateCustomMatrixCamera(renderer.GetView()->GetCamera ());
    ccam->SetProjectionMatrix(perspectiveFixup*renderer.GetView()->GetCamera ()->GetProjectionMatrix());
    renderer.GetView()->SetCustomMatrixCamera (ccam);
*/
    
    renderer.Render(mesh, texh);
  }
} // SizeChanged()

bool R2TWidgetManager::R2TWidget::OnEventDestructionStarted(const CEGUI::EventArgs& e)
{
  manager->UnRegister(window);

  return true;
} // end OnEventDestructionStarted()

bool R2TWidgetManager::R2TWidget::OnEventHidden(const CEGUI::EventArgs& e)
{
  window->removeEvent(CEGUI::Window::EventWindowUpdated);

  return true;
} // end OnEventHidden()

bool R2TWidgetManager::R2TWidget::OnEventShown(const CEGUI::EventArgs& e)
{
  window->subscribeEvent(CEGUI::Window::EventWindowUpdated,
      CEGUI::Event::Subscriber(&R2TWidget::EventWindowUpdated, this));

  return true;
} // end OnEventShown()

bool R2TWidgetManager::R2TWidget::OnEventMouseEnters(const CEGUI::EventArgs& e)
{
  rotate = true;
  return true;
} // end OnEventMouseEnters()

bool R2TWidgetManager::R2TWidget::OnEventMouseLeaves(const CEGUI::EventArgs& e)
{
  rotate = false;
  // Restore original transform.
  mesh->GetMovable()->SetTransform(otrans);
  mesh->GetMovable()->UpdateMove();
  return true;
} // end OnEventMouseLeaves()

bool R2TWidgetManager::R2TWidget::EventWindowUpdated(const CEGUI::EventArgs& e)
{
  //printf("OnEventRenderingStarted\n");

  if (rotate) 
  {
    mesh->GetMovable()->GetTransform().RotateOther(csVector3(0,1,0), -0.02f);
    mesh->GetMovable()->UpdateMove();
  }

  renderer.Render(mesh, texh);

  //printf("blah: %s\n", renderer.GetView()->GetCamera()->GetTransform().GetOrigin().Description().GetData());

  return true;
} // end OnEventRenderingStarted()

bool R2TWidgetManager::R2TWidget::OnEventSized(const CEGUI::EventArgs& e)
{
  printf("OnEventSized\n");
  CEGUI::Rect size = window->getInnerRect();
  SizeChanged(size.getWidth(), size.getHeight());

  return true;
} // end OnEventSized()

//----------------------------------------------------------------
R2TWidgetManager::R2TWidgetManager(iBase* parent)
  : scfImplementationType(this, parent), object_reg(0)
{
}

R2TWidgetManager::~R2TWidgetManager()
{
  while (!widgets.empty()) 
  {
		R2TWidget* element = widgets.back();
    widgets.pop_back();
		delete element;
  }
}

bool R2TWidgetManager::Initialize(iObjectRegistry* r)
{
  object_reg = r;

  return true;
} // end Initialize()

bool R2TWidgetManager::Register (CEGUI::Window* w, iMeshWrapper* m)
{
  UnRegister(w);

  R2TWidget* r = new R2TWidget(this, w, m);

  widgets.push_back(r);

  return true;
} // end Register()

bool R2TWidgetManager::UnRegister (CEGUI::Window* w)
{
  std::list<R2TWidget*>::iterator it = widgets.begin();
  for (; it != widgets.end(); it++)
  {
    if ((*it)->window == w)
    {
      R2TWidget* e = *it;
      widgets.erase(it);
      delete e;
      return true;
    }
  }

  return false;
} // end UnRegister()
