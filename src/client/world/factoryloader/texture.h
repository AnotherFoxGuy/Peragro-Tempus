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


namespace Texture
{

  csStringHash xmltokens;
#define CS_TOKEN_ITEM_FILE "src/client/world/factoryloader/csloader.tok"
#include <cstool/tokenlist.h>
#undef CS_TOKEN_ITEM_FILE


  bool ParseTexture (iDocumentNode* node, TexturePrototype& proto, iSyntaxService* SyntaxService)
  {
    InitTokenTable (xmltokens);

    const char* txtname = node->GetAttributeValue ("name");
    proto.name = txtname;

    // Init some stuff;
    proto.flags = 2;
    proto.Transparency = csColor(0, 0, 0);
    proto.keepImage = false;
    proto.alphaType = csAlphaMode::alphaNone;
    proto.alwaysAnimate = false;

    csRef<iDocumentNodeIterator> it = node->GetNodes ();
    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;
      const char* value = child->GetValue ();
      csStringID id = xmltokens.Request (value);
      switch (id)
      {
      case XMLTOKEN_KEY:
        proto.keyValuePairs.Push (child);
        break;
      case XMLTOKEN_FOR2D:
        {
          bool for2d;
          if (!SyntaxService->ParseBool (child, for2d, true))
            return false;
          if (for2d)
            proto.flags = proto.flags | CS_TEXTURE_2D;
          else
            proto.flags = proto.flags & ~CS_TEXTURE_2D;
        }
        break;
      case XMLTOKEN_FOR3D:
        {
          bool for3d;
          if (!SyntaxService->ParseBool (child, for3d, true))
            return false;
          if (for3d)
            proto.flags = proto.flags | CS_TEXTURE_3D;
          else
            proto.flags = proto.flags & ~CS_TEXTURE_3D;
        }
        break;
      case XMLTOKEN_TRANSPARENT:
        if (!SyntaxService->ParseColor (child, proto.Transparency))
          return false;
        break;
      case XMLTOKEN_FILE:
        {
          proto.fileName = child->GetContentsValue ();
        }
        break;
      case XMLTOKEN_MIPMAP:
        {
          bool mm;
          if (!SyntaxService->ParseBool (child, mm, true))
            return false;
          if (!mm)
            proto.flags = proto.flags | CS_TEXTURE_NOMIPMAPS;
          else
            proto.flags = proto.flags & ~CS_TEXTURE_NOMIPMAPS;
        }
        break;
      case XMLTOKEN_NPOTS:
        {
          bool npots;
          if (!SyntaxService->ParseBool (child, npots, true))
            return false;
          if (npots)
            proto.flags = proto.flags | CS_TEXTURE_NPOTS;
          else
            proto.flags = proto.flags & ~CS_TEXTURE_NPOTS;
        }
        break;
      case XMLTOKEN_KEEPIMAGE:
        {
          if (!SyntaxService->ParseBool (child, proto.keepImage, true))
            return false;
        }
        break;
      case XMLTOKEN_TYPE:
        proto.type = child->GetContentsValue ();
        if (proto.type.empty())
        {
          SyntaxService->ReportError (
            "crystalspace.maploader.parse.texture",
            child, "Expected plugin ID for <type>!");
          return false;
        }
        break;
      case XMLTOKEN_SIZE:
        {
          csRef<iDocumentAttribute> attr_w, attr_h;
          if ((attr_w = child->GetAttribute ("width")) &&
            (attr_h = child->GetAttribute ("height")))
          {/*
            context.SetSize (attr_w->GetValueAsInt(),
                             attr_h->GetValueAsInt());*/
            SyntaxService->Report("crystalspace.maploader.parse.texture", CS_REPORTER_SEVERITY_WARNING, child,
            "TODO: XMLTOKEN_SIZE for texture not implemented in threaded loader yet!");
          }
        }
        break;
      case XMLTOKEN_ALWAYSANIMATE:
        if (!SyntaxService->ParseBool (child, proto.alwaysAnimate, true))
          return false;
        break;
      case XMLTOKEN_CLAMP:
        {
          bool c;
          if (!SyntaxService->ParseBool (child, c, true))
            return false;
          if (c)
            proto.flags = proto.flags | CS_TEXTURE_CLAMP;
          else
            proto.flags = proto.flags & ~CS_TEXTURE_CLAMP;
        }
        break;
      case XMLTOKEN_FILTER:
        {
          bool c;
          if (!SyntaxService->ParseBool (child, c, true))
            return false;
          if (c)
            proto.flags = proto.flags & ~CS_TEXTURE_NOFILTER;
          else
            proto.flags = proto.flags | CS_TEXTURE_NOFILTER;
        }
        break;
      case XMLTOKEN_CLASS:
        {
          proto.textureClass = child->GetContentsValue ();
        }
        break;
      case XMLTOKEN_ALPHA:
        {
          csAlphaMode am;
          if (!SyntaxService->ParseAlphaMode (child, 0, am, false))
            return false;
          proto.alphaType = am.alphaType;
        }
        break;
      default:
        SyntaxService->ReportBadToken (child);
        return false;
      }
    }

    return true;
  }

} // namespace Texture
