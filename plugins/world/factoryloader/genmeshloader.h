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

#ifndef GENMESHLOADER_H
#define GENMESHLOADER_H

#include "crystalspace.h"

#include "properties.h"

namespace GenMesh
{

  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "plugins/world/factoryloader/gmeshldr.tok"
#include <cstool/tokenlist.h>
#undef CS_TOKEN_ITEM_FILE


  bool ParseSubMesh(csObjectPrototype& proto, iDocumentNode *node,
    iGeneralFactoryState* factstate,
    iSyntaxService* synldr)
  {
    if(!node) return false;

    uint mixmode = (uint)~0;
    csZBufMode zmode = (csZBufMode)~0;
    CS::Graphics::RenderPriority renderPrio = -1;
    csRef<iRenderBuffer> indexbuffer;
    bool b2f = false;

    GeneralMeshSubMeshWrapper submeshWrapper;

    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
      case XMLTOKEN_MIXMODE:
        if (!synldr->ParseMixmode (child, mixmode))
          return 0;
        break;

      case XMLTOKEN_MATERIAL:
        {
          const char* matname = child->GetContentsValue ();
          submeshWrapper.materialName = matname;
          break;
        }
      case XMLTOKEN_INDEXBUFFER:
        {
          indexbuffer = synldr->ParseRenderBuffer (child);
          if (!indexbuffer.IsValid()) return false;
          if (!indexbuffer->IsIndexBuffer())
          {
            synldr->ReportError (
              "crystalspace.genmeshloader.parse.buffertype",
              child, "Buffer is not an index buffer");
            return false;
          }
          break;
        }
      case XMLTOKEN_SHADERVAR:
        {
          submeshWrapper.shaderVariables.Push(child);
          break;
        }
      case XMLTOKEN_BACK2FRONT:
        if (!synldr->ParseBool (child, b2f, true))
          return 0;
        break;
      default:
        if (synldr->ParseZMode (child, zmode)) break;
        synldr->ReportBadToken (child);
      }
    }

    iGeneralMeshSubMesh* submesh = factstate->AddSubMesh (indexbuffer, 0,
      node->GetAttributeValue ("name"), mixmode);
    submesh->SetZMode (zmode);
    submesh->SetRenderPriority (renderPrio);
    submesh->SetBack2Front (b2f);

    submeshWrapper.submesh = submesh;

    proto.submeshes.Push(submeshWrapper);

    return true;
  }

  bool ParseRenderBuffer(iDocumentNode *node, iGeneralFactoryState* state, iSyntaxService* synldr)
  {
    if(!node) return false;
    if(!state) return false;

    const char *name = node->GetAttributeValue("name");
    if ((name == 0) || (*name == 0))
    {
      synldr->ReportError ("crystalspace.genmeshfactoryloader.parse",
        node, "<renderbuffer>s must have names");
      return false;
    }
    csRef<iRenderBuffer> buf = synldr->ParseRenderBuffer (node);
    if (!buf.IsValid()) return false;

    bool checkElementCount = true;
    {
      const char* check = node->GetAttributeValue("checkelementcount");
      if (check && *check)
      {
        checkElementCount = ((strcmp (check, "no") != 0)
          && (strcmp (check, "false") != 0)
          && (strcmp (check, "off") != 0));
      }
    }

    size_t rbElem = buf->GetElementCount();
    if (checkElementCount && ((size_t)state->GetVertexCount() != rbElem))
    {
      synldr->ReportError ("crystalspace.genmeshfactoryloader.parse",
        node, "Render buffer vertex count(%zu) different from "
        "factory vertex count (%d)", rbElem, state->GetVertexCount());
      return false;
    }

    if (!state->AddRenderBuffer (name, buf))
    {
      synldr->ReportError ("crystalspace.genmeshfactoryloader.parse",
        node, "A <renderbuffer> of name '%s' was already provided",
        name);
      return false;
    }
    return true;
  }

  bool Parse (iDocumentNode* node, csObjectPrototype& proto, iSyntaxService* synldr)
  {
    InitTokenTable (xmltokens);

    csRef<iMeshObjectFactory> fact = scfQueryInterface<iMeshObjectFactory>(proto.object);
    if (!fact) return false;

    csRef<iGeneralFactoryState> state;
    state = scfQueryInterface<iGeneralFactoryState> (fact);

    bool num_tri_given = false;
    bool num_vt_given = false;
    int num_tri = 0;
    int num_nor = 0;
    int num_col = 0;
    int num_vt = 0;
    bool auto_normals = false;
    bool auto_normals_nocompress = false;
    bool compress = false;

    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
      case XMLTOKEN_MANUALCOLORS:
        {
          bool r;
          if (!synldr->ParseBool (child, r, true)) return false;
          state->SetManualColors (r);
        }
        break;
      case XMLTOKEN_NOSHADOWS:
        {
          state->SetShadowCasting (false);
        }
        break;
      case XMLTOKEN_LOCALSHADOWS:
        {
          state->SetShadowReceiving (true);
        }
        break;
      case XMLTOKEN_LIGHTING:
        {
          bool r;
          if (!synldr->ParseBool (child, r, true)) return false;
          state->SetLighting (r);
        }
        break;
      case XMLTOKEN_DEFAULTCOLOR:
        {
          csColor col;
          if (!synldr->ParseColor (child, col)) return false;
          state->SetColor (col);
        }
        break;
      case XMLTOKEN_MIXMODE:
        {
          uint mm;
          if (!synldr->ParseMixmode (child, mm)) return false;
          fact->SetMixMode (mm);
        }
        break;
      case XMLTOKEN_MATERIAL:
        {
          proto.materialName = child->GetContentsValue ();
        }
        break;

      case XMLTOKEN_BOX:
        {
          num_vt_given = true;
          num_tri_given = true;

          using namespace CS::Geometry;
          csBox3 box;
          if (!synldr->ParseBox (child, box))
            return false;
          csDirtyAccessArray<csVector3> mesh_vertices;
          csDirtyAccessArray<csVector2> mesh_texels;
          csDirtyAccessArray<csVector3> mesh_normals;
          csDirtyAccessArray<csTriangle> mesh_triangles;
          Primitives::GenerateBox (box, mesh_vertices, mesh_texels,
            mesh_normals, mesh_triangles, 0, 0);
          AppendOrSetData (state, mesh_vertices, mesh_texels,
            mesh_normals, mesh_triangles);

          num_vt = state->GetVertexCount ();
          num_tri = state->GetTriangleCount ();
        }
        break;
      case XMLTOKEN_SPHERE:
        {
          num_vt_given = true;
          num_tri_given = true;
          using namespace CS::Geometry;
          csVector3 center (0, 0, 0);
          int rim_vertices = 8;
          csEllipsoid ellips;
          csRef<iDocumentAttribute> attr;
          csRef<iDocumentNode> c = child->GetNode ("center");
          if (c)
            if (!synldr->ParseVector (c, ellips.GetCenter ()))
              return false;
          c = child->GetNode ("radius");
          if (c)
          {
            if (!synldr->ParseVector (c, ellips.GetRadius ()))
              return false;
          }
          else
          {
            attr = child->GetAttribute ("radius");
            float radius;
            if (attr) radius = attr->GetValueAsFloat ();
            else radius = 1.0f;
            ellips.SetRadius (csVector3 (radius, radius, radius));
          }
          attr = child->GetAttribute ("rimvertices");
          if (attr) rim_vertices = attr->GetValueAsInt ();
          bool cylmapping, toponly, reversed;
          if (!synldr->ParseBoolAttribute (child, "cylindrical", cylmapping,
            false, false))
            return false;
          if (!synldr->ParseBoolAttribute (child, "toponly", toponly,
            false, false))
            return false;
          if (!synldr->ParseBoolAttribute (child, "reversed", reversed,
            false, false))
            return false;
          csDirtyAccessArray<csVector3> mesh_vertices;
          csDirtyAccessArray<csVector2> mesh_texels;
          csDirtyAccessArray<csVector3> mesh_normals;
          csDirtyAccessArray<csTriangle> mesh_triangles;
          Primitives::GenerateSphere (ellips, rim_vertices,
            mesh_vertices, mesh_texels,
            mesh_normals, mesh_triangles, cylmapping,
            toponly, reversed, 0);
          AppendOrSetData (state, mesh_vertices, mesh_texels,
            mesh_normals, mesh_triangles);
          num_vt = state->GetVertexCount ();
          num_tri = state->GetTriangleCount ();
        }
        break;

      case XMLTOKEN_AUTONORMALS:
        if (!synldr->ParseBool (child, auto_normals, true)) return false;
        break;
      case XMLTOKEN_NORMALNOCOMPRESS:
        if (!synldr->ParseBool (child, auto_normals_nocompress, true)) return false;
        break;
      case XMLTOKEN_COMPRESS:
        if (!synldr->ParseBool (child, compress, true)) return false;
        break;

      case XMLTOKEN_NUMTRI:
        num_tri_given = true;
        state->SetTriangleCount (child->GetContentsValueAsInt ());
        break;
      case XMLTOKEN_NUMVT:
        num_vt_given = true;
        state->SetVertexCount (child->GetContentsValueAsInt ());
        break;

      case XMLTOKEN_BACK2FRONT:
        {
          bool b2f = false;
          if (!synldr->ParseBool (child, b2f, true)) return false;
          state->SetBack2Front (b2f);
        }
        break;
      case XMLTOKEN_RENDERBUFFER:
        ParseRenderBuffer(child, state, synldr);
        break;
      case XMLTOKEN_T:
        {
          if (num_tri_given)
          {
            csTriangle* tr = state->GetTriangles ();
            if (num_tri >= state->GetTriangleCount ())
            {
              synldr->ReportError (
                "crystalspace.genmeshfactoryloader.parse.frame.badformat",
                child, "Too many triangles for a general mesh factory!");
              return false;
            }
            tr[num_tri].a = child->GetAttributeValueAsInt ("v1");
            tr[num_tri].b = child->GetAttributeValueAsInt ("v2");
            tr[num_tri].c = child->GetAttributeValueAsInt ("v3");
            if (tr[num_tri].a >= state->GetVertexCount () ||
              tr[num_tri].b >= state->GetVertexCount () ||
              tr[num_tri].c >= state->GetVertexCount ())
            {
              synldr->ReportError (
                "crystalspace.genmeshfactoryloader.parse.frame.badvt",
                child, "Bad vertex index for triangle in genmesh factory!"
                );
              return false;
            }
            num_tri++;
          }
          else
          {
            csTriangle tri;
            tri.a = child->GetAttributeValueAsInt ("v1");
            tri.b = child->GetAttributeValueAsInt ("v2");
            tri.c = child->GetAttributeValueAsInt ("v3");
            if (tri.a >= state->GetVertexCount () ||
              tri.b >= state->GetVertexCount () ||
              tri.c >= state->GetVertexCount ())
            {
              synldr->ReportError (
                "crystalspace.genmeshfactoryloader.parse.frame.badvt",
                child, "Bad vertex index for triangle in genmesh factory!"
                );
              return false;
            }
            state->AddTriangle (tri);
          }
        }
        break;


      case XMLTOKEN_N:
        {
          if (!num_vt_given)
          {
            synldr->ReportError (
              "crystalspace.genmeshfactoryloader.parse.frame.badformat",
              child, "You didn't specify 'numvt'. You should add normal information to the vertex!");
            return false;
          }
          csVector3* no = state->GetNormals ();
          if (num_nor >= state->GetVertexCount ())
          {
            synldr->ReportError (
              "crystalspace.genmeshfactoryloader.parse.frame.badformat",
              child, "Too many normals for a general mesh factory!");
            return false;
          }
          float x, y, z;
          x = child->GetAttributeValueAsFloat ("x");
          y = child->GetAttributeValueAsFloat ("y");
          z = child->GetAttributeValueAsFloat ("z");
          no[num_nor].Set (x, y, z);
          num_nor++;
        }
        break;

      case XMLTOKEN_V:
        {
          if (num_vt_given)
          {
            csVector3* vt = state->GetVertices ();
            csVector2* te = state->GetTexels ();
            if (num_vt >= state->GetVertexCount ())
            {
              synldr->ReportError (
                "crystalspace.genmeshfactoryloader.parse.frame.badformat",
                child, "Too many vertices for a general mesh factory!");
              return false;
            }
            float x, y, z, u, v;
            x = child->GetAttributeValueAsFloat ("x");
            y = child->GetAttributeValueAsFloat ("y");
            z = child->GetAttributeValueAsFloat ("z");
            u = child->GetAttributeValueAsFloat ("u");
            v = child->GetAttributeValueAsFloat ("v");
            vt[num_vt].Set (x, y, z);
            te[num_vt].Set (u, v);
            num_vt++;
          }
          else
          {
            csVector3 v, n;
            csVector2 uv;
            csColor4 col;
            v.x = child->GetAttributeValueAsFloat ("x");
            v.y = child->GetAttributeValueAsFloat ("y");
            v.z = child->GetAttributeValueAsFloat ("z");
            uv.x = child->GetAttributeValueAsFloat ("u");
            uv.y = child->GetAttributeValueAsFloat ("v");
            n.x = GetDef (child, "nx", 0.0f);
            n.y = GetDef (child, "ny", 0.0f);
            n.z = GetDef (child, "nz", 0.0f);
            col.red = GetDef (child, "red", 0.0f);
            col.green = GetDef (child, "green", 0.0f);
            col.blue = GetDef (child, "blue", 0.0f);
            col.alpha = GetDef (child, "alpha", 1.0f);
            state->AddVertex (v, uv, n, col);
          }
        }
        break;

      case XMLTOKEN_COLOR:
        {
          if (!num_vt_given)
          {
            synldr->ReportError (
              "crystalspace.genmeshfactoryloader.parse.frame.badformat",
              child, "You didn't specify 'numvt'. You should add color information to the vertex!");
           return false;
          }
          csColor4* co = state->GetColors ();
          if (num_col >= state->GetVertexCount ())
          {
            synldr->ReportError (
              "crystalspace.genmeshfactoryloader.parse.frame.badformat",
              child, "Too many colors for a general mesh factory!");
            return false;
          }
          float r, g, b, alpha;
          r = child->GetAttributeValueAsFloat ("red");
          g = child->GetAttributeValueAsFloat ("green");
          b = child->GetAttributeValueAsFloat ("blue");
          csRef<iDocumentAttribute> attr_alpha = child->GetAttribute ("alpha");
          if (attr_alpha)
            alpha = child->GetAttributeValueAsFloat ("alpha");
          else
            alpha = 1.0f;
          co[num_col].Set (r, g, b, alpha);
          num_col++;
        }
        break;

      case XMLTOKEN_SUBMESH:
        if (!state)
        {
          synldr->ReportError ("crystalspace.genmeshloader.parse",
            node, "Submesh must be specified _after_ factory tag.");
          return false;
        }
        ParseSubMesh (proto, child, state, synldr);
        break;
      default:
        synldr->ReportBadToken (child);
        return false;
      } // end switch
    } // end while

    if (num_vt_given)
      if (num_vt != state->GetVertexCount ())
      {
        synldr->ReportError (
          "crystalspace.genmeshfactoryloader.parse", node,
          "Number of vertices (%d) doesn't match real number (%d)!",
          num_vt, state->GetVertexCount ());
        return false;
      }
      if (num_tri_given)
        if (num_tri != state->GetTriangleCount ())
        {
          synldr->ReportError (
            "crystalspace.genmeshfactoryloader.parse", node,
            "Number of triangles (%d) doesn't match real number (%d)!",
            num_tri, state->GetTriangleCount ());
          return false;
        }

    if (compress)
      state->Compress ();
    if (auto_normals)
      state->CalculateNormals (!auto_normals_nocompress);

    return true;
  }

} // end GenMesh


#endif // GENMESHLOADER_H
