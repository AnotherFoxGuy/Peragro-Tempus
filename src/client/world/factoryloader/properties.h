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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include "crystalspace.h"

static float GetDef (iDocumentNode* node, const char* attrname, float def)
{
  csRef<iDocumentAttribute> attr = node->GetAttribute (attrname);
  if (attr)
    return attr->GetValueAsFloat ();
  else
    return def;
}

/* Unused stuff
static bool GetFloat (char*& p, float& f)
{
  while (*p && isspace (*p)) p++;
  if (!*p) return false;
  char* start = p;
  while (*p && !isspace (*p)) p++;
  char old = *p;
  *p = 0;
  f = atof (start);
  *p = old;
  return true;
}

static bool GetInt (char*& p, int& f)
{
  while (*p && isspace (*p)) p++;
  if (!*p) return false;
  f = 0;
  while (*p && !isspace (*p))
  {
    f = f * 10 + (*p - '0');
    p++;
  }
  return true;
}

static bool GetVector3 (char*& p, csVector3& v)
{
  if (!GetFloat (p, v.x)) return false;
  if (!GetFloat (p, v.y)) return false;
  if (!GetFloat (p, v.z)) return false;
  return true;
}

static bool GetVector2 (char*& p, csVector2& v)
{
  if (!GetFloat (p, v.x)) return false;
  if (!GetFloat (p, v.y)) return false;
  return true;
}

static bool GetColor (char*& p, csColor4& v)
{
  if (!GetFloat (p, v.red)) return false;
  if (!GetFloat (p, v.green)) return false;
  if (!GetFloat (p, v.blue)) return false;
  if (!GetFloat (p, v.alpha)) return false;
  return true;
}

static bool GetTri (char*& p, csTriangle& v)
{
  if (!GetInt (p, v.a)) return false;
  if (!GetInt (p, v.b)) return false;
  if (!GetInt (p, v.c)) return false;
  return true;
}*/

static void AppendOrSetData (iGeneralFactoryState* factory,
                             const csDirtyAccessArray<csVector3>& mesh_vertices,
                             const csDirtyAccessArray<csVector2>& mesh_texels,
                             const csDirtyAccessArray<csVector3>& mesh_normals,
                             const csDirtyAccessArray<csTriangle>& mesh_triangles)
{
  csColor4 black (0, 0, 0);
  size_t cur_vt_count = factory->GetVertexCount ();
  size_t i;
  for (i = 0 ; i < mesh_vertices.GetSize () ; i++)
    factory->AddVertex (mesh_vertices[i], mesh_texels[i],
    mesh_normals[i], black);
  for (i = 0 ; i < mesh_triangles.GetSize () ; i++)
  {
    csTriangle tri = mesh_triangles[i];
    tri.a += cur_vt_count;
    tri.b += cur_vt_count;
    tri.c += cur_vt_count;
    factory->AddTriangle (tri);
  }
}

static bool SetFactoryProperties(FileLoader* fileloader, csObjectPrototype& proto, iMeshFactoryWrapper* mfactwrap)
{
  csRef<iLoaderContext> loaderContext = fileloader->GetLoaderContext();
  iObjectRegistry* object_reg = fileloader->GetObjectRegistry();
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  mfactwrap->GetFlags() = proto.flags;
  mfactwrap->HardTransform (proto.hardMove);
  mfactwrap->SetZBufMode (proto.zBuffMode);
  long pri = engine->GetRenderPriority (proto.priority.c_str());
  mfactwrap->SetRenderPriority (pri);

  //=[ ShaderVariables ]===========================
  csRef<iShaderVariableContext> svc = mfactwrap->GetSVContext();
  if (svc.IsValid())
  {
    for (size_t i = 0; i < proto.shaderVariables.GetSize(); i++)
    {
      csRef<iDocumentNode> current = proto.shaderVariables.Get(i);
      csRef<csShaderVariable> sv;
      sv.AttachNew (new csShaderVariable);
      if (!SyntaxService->ParseShaderVar (loaderContext, current, *sv)) continue;
      svc->AddVariable (sv);
    }
  }

  //=[ Submeshes ]==================================
  for (size_t i = 0; i < proto.submeshes.GetSize(); i++)
  {
    GeneralMeshSubMeshWrapper submeshwp = proto.submeshes.Get(i);
    csRef<iGeneralMeshSubMesh> submesh = submeshwp.submesh;
    if (!submesh) continue;
    if (!submeshwp.materialName.empty())
      submesh->SetMaterial(fileloader->GetMaterial(submeshwp.materialName));

    csRef<iShaderVariableContext> svc = scfQueryInterface<iShaderVariableContext> (submesh);
    if (!svc.IsValid()) continue;
    for (size_t i = 0; i < submeshwp.shaderVariables.GetSize(); i++)
    {
      csRef<iDocumentNode> current = submeshwp.shaderVariables.Get(i);
      csRef<csShaderVariable> sv;
      sv.AttachNew (new csShaderVariable);
      if (!SyntaxService->ParseShaderVar (loaderContext, current, *sv)) continue;
      svc->AddVariable (sv);
    }
  }

  return true;
}

static bool SetTextureProperties(FileLoader* fileloader, TexturePrototype& texProto)
{
  iObjectRegistry* object_reg = fileloader->GetObjectRegistry();
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  csRef<iTextureWrapper> tex = texProto.texture;
  if (!tex.IsValid()) return false;

  if (texProto.keepImage) tex->SetKeepImage (true);
  if (!texProto.Transparency.IsBlack())
    tex->SetKeyColor (csQint (texProto.Transparency.red * 255.99),
                      csQint (texProto.Transparency.green * 255.99),
                      csQint (texProto.Transparency.blue * 255.99));
  tex->SetTextureClass (texProto.textureClass.c_str());
  tex->GetTextureHandle()->SetAlphaType (texProto.alphaType);

  csRef<iProcTexture> ipt = scfQueryInterface<iProcTexture> (tex);
  if (ipt)
    ipt->SetAlwaysAnimate (texProto.alwaysAnimate);

  // Attach the keyvalue pairs.
  for (size_t i = 0 ; i < texProto.keyValuePairs.GetSize () ; i++)
  {
    csRef<iKeyValuePair> key = SyntaxService->ParseKey (texProto.keyValuePairs[i]);
    if (!key.IsValid())
      return false;
    tex->QueryObject()->ObjAdd(key->QueryObject());
  }

  return true;
}

static bool SetMaterialProperties(FileLoader* fileloader, MaterialPrototype& mat, iTextureWrapper* tex)
{
  csRef<iLoaderContext> loaderContext = fileloader->GetLoaderContext();
  iObjectRegistry* object_reg = fileloader->GetObjectRegistry();
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iShaderManager> shaderMgr = csQueryRegistry<iShaderManager> (object_reg);
  csRef<iStringSet> stringSet = csQueryRegistryTagInterface<iStringSet> (object_reg, "crystalspace.shared.stringset");
  csRef<iSyntaxService> SyntaxService = csQueryRegistryOrLoad<iSyntaxService> (object_reg, "crystalspace.syntax.loader.service.text");

  mat.material = engine->CreateBaseMaterial (tex);
  mat.materialWrapper = engine->GetMaterialList ()->NewMaterial (mat.material, mat.name.c_str());

  for (size_t i = 0; i < mat.shaders.GetSize (); i++)
  {
    csString shadertype = mat.shadertypes[i];
    csString shadername = mat.shaders[i];
    if (!shadertype.IsEmpty() && !shadername.IsEmpty())
    {
      iShader* shader = shaderMgr->GetShader(shadername.GetData());
      if (shader)
        mat.material->SetShader (stringSet->Request(shadertype), shader);
    }
  }

  //=[ ShaderVariables ]===========================
  for (size_t i = 0; i < mat.shaderVariables.GetSize(); i++)
  {
    csRef<iDocumentNode> current = mat.shaderVariables.Get(i);
    csRef<csShaderVariable> sv;
    sv.AttachNew (new csShaderVariable);
    if (!SyntaxService->ParseShaderVar (loaderContext, current, *sv)) continue;
    mat.material->AddVariable (sv);
  }


  return true;
}


#endif // PROPERTIES_H
