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

#include <iutil/plugin.h>

#include "loader_solarsys.h"

#include "imybody.h"
#include "imystarbox.h"
// Plugin stuff


#include "csutil/scf.h"



CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_NAME,
  XMLTOKEN_ROOTBODY,
  XMLTOKEN_SELECTBODY,
  XMLTOKEN_STARBOX
};

SCF_IMPLEMENT_FACTORY(LoaderSolarsys)

//---------------------------------------------------------------------------

LoaderSolarsys::LoaderSolarsys (iBase *p) :
  scfImplementationType(this, p)
{
}

LoaderSolarsys::~LoaderSolarsys ()
{
}

bool LoaderSolarsys::Initialize(iObjectRegistry *object_reg)
{

  LoaderSolarsys::object_reg = object_reg;
  csRef<iReporter> reporter = csQueryRegistry<iReporter> (object_reg);
  synldr = csQueryRegistry<iSyntaxService> (object_reg);


  xmltokens.Register ("name", XMLTOKEN_NAME);
  xmltokens.Register ("rootbody", XMLTOKEN_ROOTBODY);
  xmltokens.Register ("selectbody", XMLTOKEN_SELECTBODY);
  xmltokens.Register ("starbox",XMLTOKEN_STARBOX );

  return true;
}
 
bool LoaderSolarsys::ParseXML ( iDocumentNode* node)
{
 
  return true;
}

csPtr<iBase> LoaderSolarsys::Parse (iDocumentNode* node, 
  iStreamSource* ss, iLoaderContext* ldr_context, iBase* context, iStringArray* failed)
{



  csRef<iSolarsysFactory> obj_fact;
  csRef<iSolarsys> obj;
  
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  if (!engine)
  {
    if (synldr) synldr->ReportError ("recon69.loader.solarsys", node, "Can't find engine!");
    return false;
  }// end if(!engine)

  obj_fact  = csQueryRegistry<iSolarsysFactory> (object_reg);
  if (!obj_fact)
  { 
    if (synldr) synldr->ReportError ("recon69.loader.solarsys", node, "Can't find iSolarsysFactory!");
    return false;
  }// end if(!obj_fact)

  obj = obj_fact->CreateObject ();
  if (!obj)
  { 
    if (synldr) synldr->ReportError ("recon69.loader.solarsys", node, "Faild to create iSolarsys!");
    return false;
  }// end if(!obj)

  // set the bodys sector 
  if(!context)
  {
    synldr->ReportError ("recon69.loader.solarsys", node, "Can't find context!");
    return false;
  }else 
  {
    csRef<iSector> sector = scfQueryInterface<iSector>(context);
    if (!sector) 
    {
    synldr->ReportError ("recon69.loader.solarsys", node, "Can't find sector!");
    return false;
    }
    obj->SetSector(sector);
  }// end if context

  if (node)
  {
    printf("Parse Node solarsys\n");
    csRef<iDocumentNodeIterator> itr = node->GetNodes ();

    while (itr->HasNext ())
    {
      csRef<iDocumentNode> child = itr->Next ();

      if (child->GetType () != CS_NODE_ELEMENT) continue;
	
      csStringID id = xmltokens.Request (child->GetValue ()); 
      switch (id)
      {
        case XMLTOKEN_NAME:
        {
          obj->SetName (child->GetContentsValue ());
        }
        break;

        case XMLTOKEN_ROOTBODY:
        {
          csRef<iMyBody> rootbody = csQueryRegistryTagInterface<iMyBody>(object_reg, child->GetContentsValue ());
          if (!rootbody) 
          {
            printf ("recon69.loader.solarsys: Uable to set rootbody:%s\n",child->GetContentsValue ()); 
            goto error;
          }
          obj->SetRootbody(rootbody);
        }
        break;

        case XMLTOKEN_SELECTBODY:
        {

          csRef<iMyBody>surfbody = csQueryRegistryTagInterface<iMyBody>(object_reg, child->GetContentsValue ());
          if (!surfbody) 
          {
            printf ("recon69.loader.solarsys: Uable to set selectbody:%s\n",child->GetContentsValue ()); 
            goto error;
          }
          obj->SetSurfaceBody(surfbody);
          obj->SetLongitude ( child->GetAttribute("longitude")->GetValueAsFloat() );
          obj->SetLatitude ( child->GetAttribute("latitude")->GetValueAsFloat() );

        }
        break;

        case XMLTOKEN_STARBOX:
        {
          csRef<iMyStarbox> starbox = csQueryRegistryTagInterface<iMyStarbox>(object_reg, child->GetContentsValue ());
          if (!starbox) 
          {
            printf ("recon69.loader.solarsys: Uable to set starbox :%s\n",child->GetContentsValue ()); 
            goto error;
          }
          obj->SetStarbox(starbox);
        }
        break;

        default:
        {
          if (synldr) synldr->ReportBadToken (child);
          goto error;
        }
      } // end swith(id)

    } // end while(it->HasNext())
  } // end if(node)


  // add the object to the object registory
  // so a pointer can be retrived from the main app 
  printf ("registering solarsys plugin %s\n", obj->GetName() );
  if (!object_reg->Register (obj, obj->GetName()) )
  {
    printf ("LoaderSolarsys::Parse: failed to register plugin %s\n", obj->GetName() );
    goto error;
  }

  printf("End Load solarsys\n");
  return csPtr<iBase> (obj);

error:

  return 0;
}




// Leftovers

/*

Demo* DemoSequenceManager::demo;
DemoSequenceManager* DemoSequenceManager::demoseq;

DemoSequenceManager::DemoSequenceManager (Demo* demo)
{
  DemoSequenceManager::demo = demo;
  demoseq = this;
  loader = 0;
  iObjectRegistry* object_reg = demo->object_reg;
  csRef<iPluginManager> plugin_mgr (
  	csQueryRegistry<iPluginManager> (object_reg));
  seqmgr = csLoadPlugin<iSequenceManager> (plugin_mgr,
    "crystalspace.utilities.sequence");


*/

/*
  csRef<iPluginManager> plg_mgr ( csQueryRegistry<iPluginManager> (object_reg)) ;
  if (!plg_mgr)
  {
    if (synldr) synldr->ReportError ("recon69.loader.solarsys", node, "Can't find plugin manager!");
    return false;
  }// end if(!engine)
  
  // Get loaders for other plugin loaders
  myLoaderBody* ldr_body = csLoadPlugin<myLoaderBody> ( plg_mgr ,  "recon69.loader.mybodyfactory");

  

 // csRef<myLoaderStarbox> ldr_starbox = csQueryRegistry<myLoaderStarbox> (object_reg);

*/
