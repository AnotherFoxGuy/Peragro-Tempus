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
#include <cstool/proctex.h>
#include <igeom/clip2d.h>

namespace PT
{
  namespace Reflection
  {
    /// Yes they may as well be globals. So shoot me :-)
    csArray<iMeshWrapper*> ReflectionUtils::reflective_meshes = 0;
    csArray<iMeshWrapper*> ReflectionUtils::refractive_meshes = 0;
    csStringID ReflectionUtils::reflection_resolution_str = 0;
    csStringID ReflectionUtils::reflection_enable_str = 0;
    csStringID ReflectionUtils::reflection_sides_str = 0;
    csStringID ReflectionUtils::reflection_texture0_str = 0;
    csStringID ReflectionUtils::refraction_texture_str = 0;
    csStringID ReflectionUtils::refraction_enable_str = 0;
    size_t ReflectionUtils::frame = 0;
    size_t ReflectionUtils::frameskip = 3;

    void ReflectionUtils::ApplyReflection(
      csRef<iView> view,
      csRef<iObjectRegistry> obj_reg
    ) {
      reflective_meshes.DeleteAll();
      iEngine* engine = view->GetEngine();
      iGraphics3D* g3d = view->GetContext();
      iTextureManager* texm = g3d->GetTextureManager();

      /// Initialize strings.
      csRef<iStringSet> stringset = csQueryRegistryTagInterface<iStringSet> (
        obj_reg, "crystalspace.shared.stringset"
      );

      reflection_enable_str = stringset->Request("reflection_enable");
      refraction_enable_str = stringset->Request("refraction_enable");
      reflection_resolution_str = stringset->Request("reflection_resolution");
      reflection_sides_str = stringset->Request("reflection_sides");
      reflection_texture0_str = stringset->Request("reflection_texture_0");
      refraction_texture_str = stringset->Request("refraction_texture");

      /// Iterate over the meshes, finding `reflection_enable=true` shadervar.
      iMeshList* meshes = engine->GetMeshes();
      for (int i=0; i<meshes->GetCount(); i++)
      {
        iMeshWrapper* mesh = meshes->Get(i);
        iShaderVariableContext* vars = mesh->GetSVContext();

        /// Test if we care about this mesh.
        if (!vars) continue;
        csShaderVariable* enabled_var =
          vars->GetVariable(reflection_enable_str);
        if (!enabled_var) continue;
        int reflection_enable = 0;
        enabled_var->GetValue(reflection_enable);
        if (!reflection_enable) continue;
        reflective_meshes.Push(mesh);

        /// How big should the reflection be? Remember it's this number
        /// squared, times the # of sides.
        csShaderVariable* resolution_var =
          vars->GetVariable(reflection_resolution_str);
        int rez = 256;
        resolution_var->GetValue(rez);
        if (rez < 2) rez = 256;

        /// Reflection texture.
        /// @todo This is currently hard-coded for the MaxY plane.
        csShaderVariable* reflection_texture0_var =
          new csShaderVariable(reflection_texture0_str);
        iTextureWrapper* a0 = engine->CreateBlackTexture(
          "a0", rez, rez, NULL, CS_TEXTURE_2D
        );
        a0->Register(texm);
        reflection_texture0_var->SetValue(a0);
        vars->AddVariable(reflection_texture0_var);
      }

      /// Iterate over the meshes, finding `refraction_enable=true` shadervar.
      for (int i=0; i<meshes->GetCount(); i++)
      {
        iMeshWrapper* mesh = meshes->Get(i);
        iShaderVariableContext* vars = mesh->GetSVContext();

        /// Test if we care about this mesh.
        if (!vars) continue;
        csShaderVariable* enabled_var =
          vars->GetVariable(refraction_enable_str);
        if (!enabled_var) continue;
        int refraction_enable = 0;
        enabled_var->GetValue(refraction_enable);
        if (!refraction_enable) continue;
        refractive_meshes.Push(mesh);

        /// How big should the refraction be? Remember it's this number
        /// squared, times the # of sides.
        csShaderVariable* resolution_var =
          vars->GetVariable(reflection_resolution_str);
        int rez = 256;
        resolution_var->GetValue(rez);
        if (rez < 2) rez = 256;

        /// Refraction texture.
        csShaderVariable* refraction_texture_var =
          new csShaderVariable(refraction_texture_str);
        iTextureWrapper* rt = engine->CreateBlackTexture(
          "a0", rez, rez, NULL, CS_TEXTURE_2D
        );
        rt->Register(texm);
        refraction_texture_var->SetValue(rt);
        vars->AddVariable(refraction_texture_var);
      }
    }

    void ReflectionUtils::RenderReflections(csRef<iView> view)
    {
      /// Throttle. Reflections are expensive.
      bool render_reflections = false;
      bool render_refractions = false;
      ReflectionUtils::frame++;
      if (ReflectionUtils::frameskip <= 1)
      {
        render_reflections = true;
        render_refractions = true;
      }
      else if (ReflectionUtils::frame == ReflectionUtils::frameskip-1)
      {
        render_refractions = true;
      }
      else if (ReflectionUtils::frame >= ReflectionUtils::frameskip)
      {
        render_reflections = true;
	ReflectionUtils::frame = 0;
      }
      if (!render_reflections && !render_refractions)
      {
        return;
      }

      iCamera* cam = view->GetCamera();
      iGraphics3D* g3d = view->GetContext();

      /// Render reflections.
      if (render_reflections)
      {
        size_t len = reflective_meshes.GetSize();
        for (size_t i=0; i<len; i++)
        {
          /// @todo This is all currently hard-coded for the MaxY plane.

          iMeshWrapper* m = reflective_meshes.Get(i);

          /// Bounding box based visibility test.
          csScreenBoxResult sbbox = m->GetScreenBoundingBox(cam);
          if (view->GetClipper()->ClassifyBox(sbbox.sbox)==-1)
          {
            continue;
          }

          iShaderVariableContext* vars = m->GetSVContext();
          csBox3 bbox = m->GetWorldBoundingBox();

          /// Marshall the texture handle.
          iTextureWrapper* a0 = 0;
          csShaderVariable* reflection_texture0_var =
            vars->GetVariable(reflection_texture0_str);
          reflection_texture0_var->GetValue(a0);
          iTextureHandle* t = a0->GetTextureHandle();

          /// @todo
          t = m->GetMeshObject()->GetMaterialWrapper()->GetMaterial()->GetTexture();

          /// Make a clipping plane from the world bounding box.
          csVector3 v1(bbox.MinX(), bbox.MaxY(), bbox.MaxZ());
          csVector3 v2(bbox.MaxX(), bbox.MaxY(), bbox.MaxZ());
          csVector3 v3(bbox.MaxX(), bbox.MaxY(), bbox.MinZ());
          csPlane3 newnp(v1, v2, v3);

          csVector3 watert = m->GetMovable()->GetFullPosition();
          csOrthoTransform origt = cam->GetTransform();
          csOrthoTransform newt;

          /// Mirror transformation.
          newt.SetO2T(origt.GetO2T() * csYScaleMatrix3(-1));
          newt.SetOrigin(
            csVector3(origt.GetOrigin().x,
                    - origt.GetOrigin().y + watert.y-1,
                      origt.GetOrigin().z)
          );

          Render2Texture(view, g3d, cam, m, origt, newt, newnp, t, true);
        }
      } // Render reflections.

      /// Render refractions.
      if (render_refractions)
      {
        size_t len = refractive_meshes.GetSize();
        for (size_t i=0; i<len; i++)
        {
          /// @todo This is all currently hard-coded for the MaxY plane.

          iMeshWrapper* m = refractive_meshes.Get(i);

          /// Bounding box based visibility test.
          csScreenBoxResult sbbox = m->GetScreenBoundingBox(cam);
          if (view->GetClipper()->ClassifyBox(sbbox.sbox)==-1)
          {
            continue;
          }

          iShaderVariableContext* vars = m->GetSVContext();
          csBox3 bbox = m->GetWorldBoundingBox();

          /// Marshall the texture handle.
          iTextureWrapper* a0 = 0;
          csShaderVariable* refraction_texture0_var =
            vars->GetVariable(refraction_texture_str);
          refraction_texture0_var->GetValue(a0);
          iTextureHandle* t = a0->GetTextureHandle();

          t = m->GetMeshObject()->GetMaterialWrapper()->GetMaterial()->GetTexture();

          /// Make a clipping plane from the world bounding box.
          csVector3 v1(bbox.MinX(), bbox.MaxY(), bbox.MaxZ());
          csVector3 v2(bbox.MaxX(), bbox.MaxY(), bbox.MaxZ());
          csVector3 v3(bbox.MaxX(), bbox.MaxY(), bbox.MinZ());
          csPlane3 newnp(v3, v2, v1);

          csVector3 watert = m->GetMovable()->GetFullPosition();
          csOrthoTransform ctrans = cam->GetTransform();

          Render2Texture(view, g3d, cam, m, ctrans, ctrans, newnp, t, false);
        }
      } // Render refractions.
    }

    void ReflectionUtils::Render2Texture(
      csRef<iView>& view,
      iGraphics3D*& g3d,
      iCamera*& cam,
      iMeshWrapper*& m,
      csOrthoTransform& oldcamera,
      csOrthoTransform& newcamera,
      csPlane3& nearclip,
      iTextureHandle*& texture,
      bool mirror
    ) {
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

    void ReflectionUtils::SetFrameSkip(size_t skip)
    {
      ReflectionUtils::frameskip = skip;
      ReflectionUtils::frame = skip; // draw on first frame
    }

  } // Reflection namespace

} // PT namespace
