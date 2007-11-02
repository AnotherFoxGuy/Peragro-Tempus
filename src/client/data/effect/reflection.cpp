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

namespace PT
{
  namespace Reflection
  {
    /// Yes they may as well be globals. So shoot me :-)
    csArray<iMeshWrapper*> ReflectionUtils::reflective_meshes = 0;
    csStringID ReflectionUtils::reflection_resolution_str = 0;
    csStringID ReflectionUtils::reflection_enable_str = 0;
    csStringID ReflectionUtils::reflection_sides_str = 0;
    csStringID ReflectionUtils::reflection_texture0_str = 0;
    size_t ReflectionUtils::frame = 0;
    size_t ReflectionUtils::frameskip = 3;

    void ReflectionUtils::ApplyReflection(
      csRef<iView> view,
      csRef<iObjectRegistry> obj_reg
    ) {
      reflective_meshes.DeleteAll();
      iEngine *engine = view->GetEngine();
      iGraphics3D *g3d = view->GetContext();
      iTextureManager *texm = g3d->GetTextureManager();

      /// Initialize strings.
      csRef<iStringSet> stringset = csQueryRegistryTagInterface<iStringSet> (
        obj_reg, "crystalspace.shared.stringset"
      );

      reflection_enable_str = stringset->Request("reflection_enable");
      reflection_resolution_str = stringset->Request("reflection_resolution");
      reflection_sides_str = stringset->Request("reflection_sides");
      reflection_texture0_str = stringset->Request("reflection_texture_0");

      /// Iterate over the meshes, finding `reflection_enable=true` shadervar.
      iMeshList *meshes = engine->GetMeshes();
      for (int i=0; i<meshes->GetCount(); i++)
      {
        iMeshWrapper *mesh = meshes->Get(i);
        iShaderVariableContext *vars = mesh->GetSVContext();

        /// Test if we care about this mesh.
        if (!vars) continue;
        csShaderVariable *enabled_var =
          vars->GetVariable(reflection_enable_str);
        if (!enabled_var) continue;
        int reflection_enable = 0;
        enabled_var->GetValue(reflection_enable);
        if (!reflection_enable) continue;
        reflective_meshes.Push(mesh);

        /// How big should the reflection be? Remember it's this number
        /// squared, times the # of sides.
        csShaderVariable *resolution_var =
          vars->GetVariable(reflection_resolution_str);
        int rez = 256;
        resolution_var->GetValue(rez);
        if (rez < 2) rez = 256;

        /// Create the textures.
        /// @todo This is currently hard-coded for the MaxY plane.
        csShaderVariable *reflection_texture0_var =
          new csShaderVariable(reflection_texture0_str);
        iTextureWrapper *a0 = engine->CreateBlackTexture(
          "a0", rez, rez, NULL, CS_TEXTURE_3D
        );
        a0->Register(texm);
        reflection_texture0_var->SetValue(a0);
        vars->AddVariable(reflection_texture0_var);
      }
    }

    void ReflectionUtils::RenderReflections(csRef<iView> view)
    {
      /// Throttle. Reflections are expensive.
      ReflectionUtils::frame++;
      if (ReflectionUtils::frame < ReflectionUtils::frameskip) return;
      ReflectionUtils::frame = 0;

      size_t len = reflective_meshes.GetSize();
      for (size_t i=0; i<len; i++)
      {
        /// @todo This is all currently hard-coded for the MaxY plane.

        iMeshWrapper *m = reflective_meshes.Get(i);
        iShaderVariableContext *vars = m->GetSVContext();

        /// Marshall the texture handle.
        iTextureWrapper *a0 = NULL;
        csShaderVariable *reflection_texture0_var =
          vars->GetVariable(reflection_texture0_str);
        reflection_texture0_var->GetValue(a0);
        iTextureHandle *t = a0->GetTextureHandle();

        iGraphics3D *g3d = view->GetContext();

        /// Make a clipping plane from the world bounding box.
        csBox3 bbox = m->GetWorldBoundingBox();
        csVector3 v1(bbox.MinX(), bbox.MaxY(), bbox.MaxZ());
        csVector3 v2(bbox.MaxX(), bbox.MaxY(), bbox.MaxZ());
        csVector3 v3(bbox.MaxX(), bbox.MaxY(), bbox.MinZ());
        csPlane3 newnp(v1, v2, v3);
        csPlane3 orignp = g3d->GetNearPlane();

        csReversibleTransform watert = m->GetMovable()->GetFullTransform();
        csOrthoTransform origt = view->GetCamera()->GetTransform();
        csOrthoTransform newt;

        /// Mirror transformation.
        newt.SetO2T(origt.GetO2T() * csYScaleMatrix3(-1));
        newt.SetOrigin(
          csVector3(origt.GetOrigin().x,
                  -(origt.GetOrigin ().y * 2)+watert.GetOrigin().y,
                    origt.GetOrigin().z)
        );

        /// The rendering sequence is: 1) set the camera, 2) set the
        /// rendering target texture, 3) set the near plane, 4) draw,
        /// 5) be nice and reset things.
        view->GetCamera()->SetTransform(newt);
        view->GetCamera()->SetMirrored(true);
        g3d->SetRenderTarget(t);
        /// "We need to hack CS!" "No, use a portal!" OMG <3 g3d
        g3d->SetNearPlane(newnp);
        g3d->BeginDraw(CSDRAW_CLEARSCREEN|CSDRAW_3DGRAPHICS);
        view->Draw();
        g3d->FinishDraw();
        g3d->SetNearPlane(orignp);
        view->GetCamera()->SetTransform(origt);
        view->GetCamera()->SetMirrored(false);
      }
    }

    void ReflectionUtils::SetFrameSkip(size_t skip) {
      ReflectionUtils::frameskip = skip;
      ReflectionUtils::frame = skip; // draw on first frame
    }

  } // Reflection namespace

} // PT namespace
