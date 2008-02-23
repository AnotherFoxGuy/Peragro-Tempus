/*
    Copyright (C) 2000-2001 by Jorrit Tyberghein
    Copyright (C) 1998-2000 by Ivan Avramovic <ivan@avramovic.com>

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

#include "cssysdef.h"
#include "csqint.h"

#include "csgfx/imagecubemapmaker.h"
#include "csgfx/rgbpixel.h"
#include "cstool/proctex.h"
#include "csutil/array.h"
#include "csutil/cscolor.h"
#include "csutil/scanstr.h"
#include "iengine/engine.h"
#include "iengine/material.h"
#include "iengine/region.h"
#include "iengine/texture.h"
#include "igraphic/animimg.h"
#include "imap/ldrctxt.h"
#include "imap/reader.h"
#include "iutil/document.h"
#include "iutil/objreg.h"
#include "iutil/strset.h"
#include "ivaria/keyval.h"
#include "ivaria/reporter.h"
#include "ivideo/graph3d.h"
#include "ivideo/material.h"
#include "ivideo/shader/shader.h"
#include "ivideo/texture.h"

namespace Material
{

  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "src/client/world/factoryloader/csloader.tok"
#include <cstool/tokenlist.h>
#undef CS_TOKEN_ITEM_FILE 

  bool ParseMaterial (iDocumentNode* node, MaterialPrototype& mat, iSyntaxService* SyntaxService)
  {
    InitTokenTable (xmltokens);

    const char* matname = node->GetAttributeValue ("name");
    mat.name = matname;

    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
      case XMLTOKEN_TEXTURE:
        {
          mat.textureName = child->GetContentsValue ();
        }
        break;
      case XMLTOKEN_COLOR:
        {
          //if (!SyntaxService->ParseColor (child, col))
            //return false;
          printf("E: Threaded loader doesn't support COLOR in materials yet!\n");
        }
        break;
      case XMLTOKEN_SHADER:
        {
          const char* shadername = child->GetContentsValue ();
          const char* shadertype = child->GetAttributeValue ("type");

          mat.shadertypes.Push (shadertype);
          mat.shaders.Push (shadername);
        }
        break;
      case XMLTOKEN_SHADERVAR:
        {
          mat.shaderVariables.Push(child);
        }
        break;
      default:
        SyntaxService->ReportBadToken (child);
        return false;
      }
    }

    return true;
  }

} // namespace Material
