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

#ifndef MESHOBJECTFACTORY_H
#define MESHOBJECTFACTORY_H

#include "crystalspace.h"


namespace MeshObjectFactory
{
  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "src/client/world/factoryloader/csloader.tok"
#include "cstool/tokenlist.h"
#undef CS_TOKEN_ITEM_FILE 

  bool LoadMeshObjectFactory (iDocumentNode* node, csObjectPrototype& proto, iSyntaxService* SyntaxService)
  {
    InitTokenTable (xmltokens);

    csRef<iMeshObjectFactory> meshObjfact = scfQueryInterface<iMeshObjectFactory>(proto.object);
    if (!meshObjfact) return false;

    bool staticshape = false;
    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
      case XMLTOKEN_STATICSHAPE:
        if (!SyntaxService->ParseBool (child, staticshape, true))
          return false;
        break;
      case XMLTOKEN_TRIMESH:
        {
          iObjectModel* objmodel = meshObjfact->GetObjectModel ();
          if (!objmodel)
          {
            SyntaxService->ReportError (
              "crystalspace.maploader.parse.meshfactory", child,
              "This factory doesn't support setting of other 'trimesh'!");
            return false;
          }
          SyntaxService->Report("crystalspace.maploader.parse.meshfactory",CS_REPORTER_SEVERITY_WARNING,child,
            "TODO: Trimesh not implemented in threaded loader yet!");
          /*
          if (!ParseTriMesh (child, objmodel))
          {
            // Error already reported.
            return false;
          }*/
        }
        break;
      case XMLTOKEN_CLOSED:
        {
          iObjectModel* objmodel = meshObjfact->GetObjectModel ();
          csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
          while (it->HasNext ())
          {
            csStringID id;
            iTriangleMesh* trimesh = it->Next (id);
            if (trimesh) trimesh->GetFlags ().Set (
              CS_TRIMESH_CLOSED | CS_TRIMESH_NOTCLOSED, CS_TRIMESH_CLOSED);
          }
        }
        break;
      case XMLTOKEN_CONVEX:
        {
          iObjectModel* objmodel = meshObjfact->GetObjectModel ();
          csRef<iTriangleMeshIterator> it = objmodel->GetTriangleDataIterator ();
          while (it->HasNext ())
          {
            csStringID id;
            iTriangleMesh* trimesh = it->Next (id);
            if (trimesh) trimesh->GetFlags ().Set (
              CS_TRIMESH_CONVEX | CS_TRIMESH_NOTCONVEX, CS_TRIMESH_CONVEX);
          }
        }
        break;
      case XMLTOKEN_MATERIAL:
        {
          proto.materialName = child->GetContentsValue ();
        }
        break;

      case XMLTOKEN_MOVE:
        {
          csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
          if (matrix_node)
          {
            csMatrix3 m;
            if (!SyntaxService->ParseMatrix (matrix_node, m))
              return false;
            proto.move.SetO2T (m);
          }
          csRef<iDocumentNode> vector_node = child->GetNode ("v");
          if (vector_node)
          {
            csVector3 v;
            if (!SyntaxService->ParseVector (vector_node, v))
              return false;
            proto.move.SetO2TTranslation (v);
          }
        }
        break;
      case XMLTOKEN_HARDMOVE:
        {
          if (!meshObjfact->SupportsHardTransform ())
          {
            SyntaxService->ReportError (
              "crystalspace.maploader.parse.meshfactory",
              child, "This factory doesn't support 'hardmove'!");
            return false;
          }
          csReversibleTransform tr;
          csRef<iDocumentNode> matrix_node = child->GetNode ("matrix");
          if (matrix_node)
          {
            csMatrix3 m;
            if (!SyntaxService->ParseMatrix (matrix_node, m))
              return false;
            tr.SetT2O (m);
          }
          csRef<iDocumentNode> vector_node = child->GetNode ("v");
          if (vector_node)
          {
            csVector3 v;
            if (!SyntaxService->ParseVector (vector_node, v))
              return false;
            tr.SetOrigin (v);
          }
          proto.hardMove = tr;
        }
        break;

      case XMLTOKEN_ZUSE:
        proto.zBuffMode = CS_ZBUF_USE;
        break;
      case XMLTOKEN_ZFILL:
        proto.zBuffMode = CS_ZBUF_FILL;
        break;
      case XMLTOKEN_ZNONE:
        proto.zBuffMode = CS_ZBUF_NONE;
        break;
      case XMLTOKEN_ZTEST:
        proto.zBuffMode = CS_ZBUF_TEST;
        break;
      case XMLTOKEN_PRIORITY:
        {
          proto.priority = child->GetContentsValue ();
        }
        break;
      case XMLTOKEN_SHADERVAR:
        {
          proto.shaderVariables.Push(child);
        }
        break;
      case XMLTOKEN_NOLIGHTING:
        proto.flags.Set (CS_ENTITY_NOLIGHTING, CS_ENTITY_NOLIGHTING);
        break;
      case XMLTOKEN_NOSHADOWS:
        proto.flags.Set (CS_ENTITY_NOSHADOWS, CS_ENTITY_NOSHADOWS);
        break;
      case XMLTOKEN_NOCLIP:
        proto.flags.Set (CS_ENTITY_NOCLIP, CS_ENTITY_NOCLIP);
        break;
      case XMLTOKEN_NOHITBEAM:
        proto.flags.Set (CS_ENTITY_NOHITBEAM, CS_ENTITY_NOHITBEAM);
        break;
      case XMLTOKEN_INVISIBLEMESH:
        proto.flags.Set (CS_ENTITY_INVISIBLEMESH, CS_ENTITY_INVISIBLEMESH);
        break;
      case XMLTOKEN_INVISIBLE:
        proto.flags.Set (CS_ENTITY_INVISIBLE, CS_ENTITY_INVISIBLE);
        break;
      case XMLTOKEN_DETAIL:
        proto.flags.Set (CS_ENTITY_DETAIL, CS_ENTITY_DETAIL);
        break;
      case XMLTOKEN_PLUGIN:
        //TODO?
        break;
      case XMLTOKEN_PARAMS:
        // Doing this seperatly.
        break;
      default:
        SyntaxService->ReportBadToken (child);
        return false;
      } // end switch
    } // end while

    meshObjfact->GetFlags ().SetBool (CS_FACTORY_STATICSHAPE, staticshape);


    return true;
  }

  
} // end MeshObjectFactory


#endif // MESHOBJECTFACTORY_H
