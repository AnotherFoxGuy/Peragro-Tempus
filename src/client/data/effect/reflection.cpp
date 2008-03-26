/*
    Copyright (C) 2005 Development Team of Peragro Tempus

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

#include "reflection.h"
#include <iengine/mesh.h>
#include <iutil/object.h>
#include <iengine/portal.h>
#include <iengine/sector.h>
#include <iengine/rview.h>
#include <iengine/material.h>
#include <iengine/movable.h>
#include <imesh/object.h>
#include <ivideo/material.h>
#include <ivideo/txtmgr.h>
#include <cstool/proctex.h>
#include <igeom/clip2d.h>

#include "client/event/eventmanager.h"
#include "client/event/regionevent.h"

#include "client/pointer/pointer.h"

#include "client/entity/player/playerentity.h"

namespace PT
{
  namespace Reflection
  {
    csStringID ReflectionUtils::reflection_resolution_str = 0;
    csStringID ReflectionUtils::reflection_enable_str = 0;
    csStringID ReflectionUtils::reflection_sides_str = 0;
    csStringID ReflectionUtils::reflection_texture0_str = 0;
    csStringID ReflectionUtils::refraction_texture_str = 0;
    csStringID ReflectionUtils::refraction_enable_str = 0;

    ReflectionRenderer::ReflectionRenderer()
    {
      frame = 0;
      frameskip = 3;
    }

    ReflectionRenderer::~ReflectionRenderer()
    {
    }

    bool ReflectionRenderer::Initialize()
    {
      iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

      reflectiveMeshes.DeleteAll();
      engine = csQueryRegistry<iEngine> (obj_reg);
      if (!engine) return false;
      g3d = csQueryRegistry<iGraphics3D> (obj_reg);
      if (!g3d) return false;
      texm = g3d->GetTextureManager();
      if (!texm) return false;

       /// Initialize strings.
      csRef<iStringSet> stringset = csQueryRegistryTagInterface<iStringSet> (
        obj_reg, "crystalspace.shared.stringset");

      ReflectionUtils::reflection_enable_str = stringset->Request("reflection_enable");
      ReflectionUtils::refraction_enable_str = stringset->Request("refraction_enable");
      ReflectionUtils::reflection_resolution_str = stringset->Request("reflection_resolution");
      ReflectionUtils::reflection_sides_str = stringset->Request("reflection_sides");
      ReflectionUtils::reflection_texture0_str = stringset->Request("reflection_texture_0");
      ReflectionUtils::refraction_texture_str = stringset->Request("refraction_texture");

      // Register for PreProcess events.
      csBaseEventHandler::Initialize (obj_reg);
      RegisterQueue (obj_reg, csevPreProcess (obj_reg));

      using namespace PT::Events;
      // Register listener for RegionLoadedEvent.
      EventHandler<ReflectionRenderer>* cbLoaded = new EventHandler<ReflectionRenderer>(&ReflectionRenderer::TileLoaded, this);
      PointerLibrary::getInstance()->getEventManager()->AddListener("world.loaded", cbLoaded);

      return true;
    }

    void ReflectionRenderer::SetFrameSkip(size_t skip)
    {
      frameskip = skip;
      frame = skip; // draw on first frame
    }

    bool ReflectionRenderer::HandleEvent(iEvent &ev)
    {
      if (ev.Name == PreProcess)
        Render();

      return true;
    }

    void ReflectionRenderer::Render()
    {
      PT::Entity::PlayerEntity* player = PT::Entity::PlayerEntity::Instance();
      if (!player) return;

      csRef<iPcDefaultCamera> cam = player->GetCamera();
      if (!cam) return;

      csRef<iView> view = cam->GetView();
      if (!view) return;

      /// Throttle. Reflections are expensive.
      bool render_reflections = false;
      bool render_refractions = false;
      frame++;
      if (frameskip <= 1)
      {
        render_reflections = true;
        render_refractions = true;
      }
      else if (frame == frameskip-1)
      {
        render_refractions = true;
      }
      else if (frame >= frameskip)
      {
        render_reflections = true;
	frame = 0;
      }
      if (!render_reflections && !render_refractions)
      {
        return;
      }

      /// Render reflections.
      if (render_reflections)
      {
        size_t len = reflectiveMeshes.GetSize();
        for (size_t i=0; i<len; i++)
        {
          iMeshWrapper* m = reflectiveMeshes.Get(i);
          ReflectionUtils::RenderReflection(m, view);
        }
      } // Render reflections.

      /// Render refractions.
      if (render_refractions)
      {
        size_t len = refractiveMeshes.GetSize();
        for (size_t i=0; i<len; i++)
        {
          iMeshWrapper* m = refractiveMeshes.Get(i);
          ReflectionUtils::RenderRefraction(m, view);
        }
      } // Render refractions.
    } // end Render()

    bool ReflectionRenderer::TileLoaded (PT::Events::Eventp ev)
    {
      /// Refresh our mesh list.
      reflectiveMeshes.DeleteAll();
      refractiveMeshes.DeleteAll();

      /// Iterate over the meshes, finding `reflection_enable=true` shadervar.
      iMeshList* meshes = engine->GetMeshes();
      for (int i=0; i<meshes->GetCount(); i++)
      {
        csRef<iMeshWrapper> mesh = meshes->Get(i);
        iShaderVariableContext* vars = mesh->GetSVContext();

        /// Test if we care about this mesh.
        if (!vars) continue;
        csShaderVariable* enabled_var =
          vars->GetVariable(ReflectionUtils::reflection_enable_str);
        if (!enabled_var) continue;
        int reflection_enable = 0;
        enabled_var->GetValue(reflection_enable);
        if (!reflection_enable) continue;
        reflectiveMeshes.Push(mesh);

        /// How big should the reflection be? Remember it's this number
        /// squared, times the # of sides.
        csShaderVariable* resolution_var = vars->GetVariable(ReflectionUtils::reflection_resolution_str);
        int rez = 256;
        resolution_var->GetValue(rez);
        if (rez < 2) rez = 256;

        /// Reflection texture.
        /// @todo This is currently hard-coded for the MaxY plane.
        csRef<csShaderVariable> reflection_texture0_var = vars->GetVariableAdd(ReflectionUtils::reflection_texture0_str);
        csRef<iTextureHandle> a0 = texm->CreateTexture(rez, rez, csimg2D, "rgba8", CS_TEXTURE_3D | CS_TEXTURE_NOMIPMAPS );
        reflection_texture0_var->SetValue(a0);  
      }

      /// Iterate over the meshes, finding `refraction_enable=true` shadervar.
      for (int i=0; i<meshes->GetCount(); i++)
      {
        csRef<iMeshWrapper> mesh = meshes->Get(i);
        iShaderVariableContext* vars = mesh->GetSVContext();

        /// Test if we care about this mesh.
        if (!vars) continue;
        csShaderVariable* enabled_var =
          vars->GetVariable(ReflectionUtils::refraction_enable_str);
        if (!enabled_var) continue;
        int refraction_enable = 0;
        enabled_var->GetValue(refraction_enable);
        if (!refraction_enable) continue;
        refractiveMeshes.Push(mesh);

        /// How big should the refraction be? Remember it's this number
        /// squared, times the # of sides.
        csShaderVariable* resolution_var =
          vars->GetVariable(ReflectionUtils::reflection_resolution_str);
        int rez = 256;
        resolution_var->GetValue(rez);
        if (rez < 2) rez = 256;

        /// Refraction texture.
        csRef<csShaderVariable> refraction_texture_var = vars->GetVariableAdd(ReflectionUtils::refraction_texture_str);
        csRef<iTextureHandle> a0 = texm->CreateTexture(rez, rez, csimg2D, "rgba8", CS_TEXTURE_3D | CS_TEXTURE_NOMIPMAPS );
        refraction_texture_var->SetValue(a0);
      }

      return true;
    }

    void ReflectionUtils::RenderReflection(iMeshWrapper* m, iView* view)
    {
      /// @todo This is all currently hard-coded for the MaxY plane.

      iCamera* cam = view->GetCamera();

      /// Bounding box based visibility test.
      csScreenBoxResult sbbox = m->GetScreenBoundingBox(cam);
      if (view->GetClipper()->ClassifyBox(sbbox.sbox)==-1)
      {
        return;
      }

      iShaderVariableContext* vars = m->GetSVContext();
      csBox3 bbox = m->GetWorldBoundingBox();

      /// Marshall the texture handle.
      iTextureHandle* t = 0;
      csShaderVariable* reflection_texture0_var = vars->GetVariable(reflection_texture0_str);
      reflection_texture0_var->GetValue(t);

      /// Make a clipping plane from the world bounding box.
      csVector3 v1(bbox.MinX(), bbox.MaxY(), bbox.MaxZ());
      csVector3 v2(bbox.MaxX(), bbox.MaxY(), bbox.MaxZ());
      csVector3 v3(bbox.MaxX(), bbox.MaxY(), bbox.MinZ());
      csPlane3 newnp(v1, v2, v3);

      csVector3 watert = m->GetMovable()->GetFullPosition();
      csOrthoTransform origt = cam->GetTransform();
      csOrthoTransform newt;

      /*csVector3 a(0,1,0);
      float mat[3][3];

      for (int j = 0; j < 3; j++)
      {
        for (int i = 0; i < 3; i++)
        {
          //KroneckerDelta(i,j)
          float delta = 0;
          if (i == j) delta = 1;
          mat[j][i] = delta - 2 * (a[i]*a[j])/a.SquaredNorm();
        }
      }

      csMatrix3 matr;
      matr.m11 = mat[0][0]; matr.m12 = mat[0][1]; matr.m13 = mat[0][2];
      matr.m21 = mat[1][0]; matr.m22 = mat[1][1]; matr.m23 = mat[1][2];
      matr.m31 = mat[2][0]; matr.m32 = mat[2][1]; matr.m33 = mat[2][2];

      csOrthoTransform reflect(matr, csVector3(0,0,0));*/

      
      /// Mirror transformation.
      newt.SetO2T(origt.GetO2T() * csYScaleMatrix3(-1));
      //newt.SetO2T(origt.GetO2T());
      newt.SetOrigin(csVector3(origt.GetOrigin().x,
                     bbox.MaxY() - (origt.GetOrigin().y - bbox.MaxY()),
                     origt.GetOrigin().z));

      //newt = newt * reflect;

      ReflectionUtils::Render2Texture(view, m, origt, newt, newnp, t, true);

    } // end RenderReflection()

    void ReflectionUtils::RenderRefraction(iMeshWrapper* m, iView* view)
    {
      /// @todo This is all currently hard-coded for the MaxY plane.

      iCamera* cam = view->GetCamera();

      /// Bounding box based visibility test.
      csScreenBoxResult sbbox = m->GetScreenBoundingBox(cam);
      if (view->GetClipper()->ClassifyBox(sbbox.sbox)==-1)
      {
        return;
      }

      iShaderVariableContext* vars = m->GetSVContext();
      csBox3 bbox = m->GetWorldBoundingBox();

      /// Marshall the texture handle.
      iTextureHandle* t = 0;
      csShaderVariable* refraction_texture0_var =
        vars->GetVariable(refraction_texture_str);
      refraction_texture0_var->GetValue(t);

      /// Make a clipping plane from the world bounding box.
      csVector3 v1(bbox.MinX(), bbox.MaxY(), bbox.MaxZ());
      csVector3 v2(bbox.MaxX(), bbox.MaxY(), bbox.MaxZ());
      csVector3 v3(bbox.MaxX(), bbox.MaxY(), bbox.MinZ());
      csPlane3 newnp(v3, v2, v1);

      csVector3 watert = m->GetMovable()->GetFullPosition();
      csOrthoTransform ctrans = cam->GetTransform();

      ReflectionUtils::Render2Texture(view, m, ctrans, ctrans, newnp, t, false);

    } // end RenderReflection()

    void ReflectionUtils::Render2Texture(
      iView* view,
      iMeshWrapper* m,
      csOrthoTransform& oldcamera,
      csOrthoTransform& newcamera,
      csPlane3& nearclip,
      iTextureHandle* texture,
      bool mirror) 
    {
      iCamera* cam = view->GetCamera();
      iGraphics3D* g3d = view->GetContext();

      csPlane3 orignp = g3d->GetNearPlane();

      /// The rendering sequence is: 
      /// 1) Hide the reflection mesh
      /// 2) set the camera 
      /// 3) set the rendering target texture
      /// 4) set the near plane
      /// 5) draw
      /// 6) be nice and reset things

      // Setup
      m->SetFlagsRecursive(CS_ENTITY_INVISIBLEMESH, CS_ENTITY_INVISIBLEMESH);
      cam->SetTransform(newcamera);
      if (mirror) cam->SetMirrored(true);
      g3d->SetRenderTarget(texture);

      g3d->ValidateRenderTargets ();
      //printf("%s\n", g3d->ValidateRenderTargets ()? "VALID" : "ERROR");

      // "We need to hack CS!" "No, use a portal!" OMG <3 g3d
      g3d->SetNearPlane(nearclip);

      // Draw
      g3d->BeginDraw(CSDRAW_CLEARZBUFFER|CSDRAW_CLEARSCREEN|CSDRAW_3DGRAPHICS);
      view->Draw();
      g3d->FinishDraw();

      // Reset
      g3d->SetNearPlane(orignp);
      cam->SetTransform(oldcamera);
      if (mirror) cam->SetMirrored(false);
      m->SetFlagsRecursive(CS_ENTITY_INVISIBLEMESH, 0);
    }

  } // Reflection namespace

} // PT namespace
