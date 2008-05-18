/*
    Copyright (C) 2008 by Mogue Carpenter

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
// File: loader_template.cpp

#include <cssysdef.h>

#include <csutil/util.h>
#include <iutil/document.h>
#include <iutil/objreg.h>
#include <iutil/object.h>
#include <imap/services.h>
#include <imap/ldrctxt.h>
#include <ivaria/reporter.h>

#include "loader_starbox.h"

// Plugin stuff

CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_NAME,
  XMLTOKEN_CATALOGUE_FILE,
  XMLTOKEN_CURRENT_SYSTEM_ID,
  XMLTOKEN_CAMERA,
  XMLTOKEN_BODY,
  XMLTOKEN_ORBIT,
  XMLTOKEN_TEXTURE,
  XMLTOKEN_FILE,
  XMLTOKEN_MATERAL
};

SCF_IMPLEMENT_FACTORY(myLoaderStarbox)


//---------------------------------------------------------------------------

myLoaderStarbox::myLoaderStarbox (iBase *p) :
  scfImplementationType(this, p)
{
}

myLoaderStarbox::~myLoaderStarbox ()
{
}

bool myLoaderStarbox::Initialize(iObjectRegistry *object_reg)
{

  myLoaderStarbox::object_reg = object_reg;
  csRef<iReporter> reporter = csQueryRegistry<iReporter> (object_reg);
  synldr = csQueryRegistry<iSyntaxService> (object_reg);

  xmltokens.Register ("name", XMLTOKEN_NAME);
  xmltokens.Register ("star_catalogue", XMLTOKEN_CATALOGUE_FILE);
  xmltokens.Register ("current_system_id", XMLTOKEN_CURRENT_SYSTEM_ID);
  xmltokens.Register ("camera", XMLTOKEN_CAMERA);
  xmltokens.Register ("body", XMLTOKEN_BODY);
  xmltokens.Register ("orbit", XMLTOKEN_ORBIT);
  xmltokens.Register ("texture", XMLTOKEN_TEXTURE);
  xmltokens.Register ("file", XMLTOKEN_FILE);
  xmltokens.Register ("material", XMLTOKEN_MATERAL);

  return true;
}

bool myLoaderStarbox::ParseXML ( iDocumentNode* node)
{

  return true;
}

csPtr<iBase> myLoaderStarbox::Parse (iDocumentNode* node,
  iStreamSource*, iLoaderContext* ldr_context, iBase* /*context*/)
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iMyStarboxFactory> obj_fact;
  csRef<iMyStarbox> obj;

  if (!engine)
  {
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.starbox",
          node, "Can't find engine!");
      return 0;
    }
    return 0;
  }// end if(!engine)


  obj_fact = csQueryRegistry<iMyStarboxFactory> (object_reg);
  if (!obj_fact)
  {
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.starbox",
        node, "Can't find iMyStarboxFactory!");
      return 0;
    }
    return 0;
  }// end if(!obj_fact)

  obj = obj_fact->CreateObject ();
  if (!obj)
  {
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.starbox",
        node, "Faild to create iMyStarbox!");
      return 0;
    }
    return 0;
  }// end if(!obj_fact)



  if (node)
  {
    csRef<iDocumentNodeIterator> it = node->GetNodes ();

    while (it->HasNext ())
    {
      csRef<iDocumentNode> child = it->Next ();

      if (child->GetType () != CS_NODE_ELEMENT) continue;

      csStringID id = xmltokens.Request (child->GetValue ());
      switch (id)
      {
        case XMLTOKEN_NAME:
        {
          obj->SetName ( csStrNew (child->GetContentsValue() ) );
        }
        break;

        case XMLTOKEN_CATALOGUE_FILE:
        {
          obj->LoadStarCatalogue ( csStrNew (child->GetContentsValue() ) );
        }
        break;

        case XMLTOKEN_CURRENT_SYSTEM_ID:
        {
          obj->SetCurrentSystem ( child->GetContentsValueAsInt() );
        }
        break;

        case XMLTOKEN_CAMERA:{}
        break;

        case XMLTOKEN_BODY:{}
        break;

        case XMLTOKEN_ORBIT:{}
        break;

        case XMLTOKEN_TEXTURE:{}
        break;

        case XMLTOKEN_FILE:{}
        break;

        case XMLTOKEN_MATERAL:{}
        break;

        default:
        {
          if (synldr) synldr->ReportBadToken (child);
          goto error;
        }
      } // end swith(id)

    } // end while(it->HasNext())
  } // end if(node)

  starbox = obj; // Keep a referance to starbox in the loader
  return csPtr<iBase> (obj);

error:

  return 0;
}


