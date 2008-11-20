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
#include <string>

#include "loader_mybody.h"

// Plugin stuff

CS_IMPLEMENT_PLUGIN

enum
{
  XMLTOKEN_MESHFACTORY,
  XMLTOKEN_ORBIT, 
  XMLTOKEN_MATERIAL,
  XMLTOKEN_BODY,
  XMLTOKEN_LIGHT,
  XMLTOKEN_COLOR,
  XMLTOKEN_RADIUS

};

SCF_IMPLEMENT_FACTORY(myLoaderBody)


//---------------------------------------------------------------------------

myLoaderBody::myLoaderBody (iBase *p) :
  scfImplementationType(this, p)
{
}

myLoaderBody::~myLoaderBody ()
{
}

bool myLoaderBody::Initialize(iObjectRegistry *object_reg)
{

  myLoaderBody::object_reg = object_reg;
  csRef<iReporter> reporter = csQueryRegistry<iReporter> (object_reg);
  synldr = csQueryRegistry<iSyntaxService> (object_reg);

//  xmltokens.Register ("name", XMLTOKEN_NAME);
  xmltokens.Register ("meshfactory",XMLTOKEN_MESHFACTORY );
  xmltokens.Register ("orbit",XMLTOKEN_ORBIT );
  xmltokens.Register ("material",XMLTOKEN_MATERIAL );
  xmltokens.Register ("body",XMLTOKEN_BODY );
  xmltokens.Register ("light",XMLTOKEN_LIGHT );
  xmltokens.Register ("radius",XMLTOKEN_RADIUS );
  xmltokens.Register ("color",XMLTOKEN_COLOR );

//  xmltokens.Register ("", );


  return true;
}
 
bool myLoaderBody::ParseXML ( iDocumentNode* node)
{
 
  return true;
}
bool myLoaderBody::ParseLight (iDocumentNode* node, csRef<iMyBody> obj)
{
//<light>
//  <radius>10</radius>
//  <color red='' green='' blue=''/>
//</Light>
  int r=0;
  csColor c;

  csRef<iDocumentNodeIterator> it = node->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    if (child->GetType () != CS_NODE_ELEMENT) continue;
    const char *value = child->GetValue ();
    csStringID id = xmltokens.Request (value);
    switch (id)
    {
      case XMLTOKEN_RADIUS:
        r = (int)child->GetAttribute("r")->GetValueAsFloat();
	printf("r=%i\n",r);
      break; 

      case XMLTOKEN_COLOR:
	  synldr->ParseColor(child, c);
//          c.Set(1,1,1);
	  printf("color=%2.4f,%2.4f,%2.4f \n" , c.red, c.green, c.blue);
        break;

      default:
        synldr->ReportBadToken (child);
        return false;
    }
  }
  printf ("adding light to body\n");
  obj->Add_Light(r,c);
  return true;
}

csPtr<iBase> myLoaderBody::Parse (iDocumentNode* node, 
  iStreamSource* ss, iLoaderContext* ldr_context, iBase* context)
{

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iMyBodyFactory> obj_fact;
  csRef<iMyBody> obj;

  if (!engine)
  {
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.mybodyfactory",
          node, "Can't find engine!");
      return 0;
    }
  }// end if(!engine)


  if (node)
  {
    csRef<iDocumentNodeIterator> itr = node->GetNodes ();
    while (itr->HasNext ())
    {
      csRef<iDocumentNode> child = itr->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;

      csStringID id = xmltokens.Request (child->GetValue ());
      printf("child->%s\n",child->GetValue ());

      switch (id)
      {

        case XMLTOKEN_BODY:
        {
	  printf ("%s\n\n\n",child->GetContentsValue());
          csRef<iMyBody> parent ;
          // Create body with Null as parent 
          obj = ParseBody ( child, ss, ldr_context, context , parent);
          
        }
        break;

	case XMLTOKEN_MESHFACTORY: break;
	case XMLTOKEN_ORBIT: break;
	case XMLTOKEN_MATERIAL: break;
	case XMLTOKEN_LIGHT: break;
	case XMLTOKEN_COLOR: break;
	case XMLTOKEN_RADIUS: break;

        default:
        {
          if (synldr) synldr->ReportBadToken (child);
          goto error;
        }
      } // end swith(id)

    } // end while(it->HasNext())
  } // end if(node)

 
  return csPtr<iBase> (obj);

error:

  return 0;
}


csRef<iMyBody> myLoaderBody::ParseBody (
                                         iDocumentNode* node, 
                                         iStreamSource* ss,
                                         iLoaderContext* ldr_context,
                                         iBase* context,
                                         csRef<iMyBody>& parent
                                       )

{

  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  csRef<iMyBodyFactory> obj_fact;
  csRef<iMyBody> obj;

  if (!engine)
  {
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.mybodyfactory",
          node, "Can't find engine!");
      return 0;
    }
  }// end if(!engine)


  obj_fact  = csQueryRegistry<iMyBodyFactory> (object_reg);
  if (!obj_fact)
  { 
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.mybodyfactory",
        node, "Can't find iMyBodyFactory!");
      return 0;
    }
  }// end if(!obj_fact)

  obj = obj_fact->CreateObject ();
  if (!obj)
  { 
    if (synldr)
    {
      synldr->ReportError (
        "recon69.loader.mybodyfactory",
        node, "Faild to create iMyBody!");
      return 0;
    }
  }// end if(!obj)    


  // set the bodys sector 
  if(!context)
  {
    synldr->ReportError (
      "recon69.loader.mybodyfactory",
      node, "Can't find context!");
    return 0;
  } else {
    csRef<iSector> sector = scfQueryInterface<iSector>(context);
    if (!sector) 
    {
    synldr->ReportError (
      "recon69.loader.mybodyfactory",
      node, "Can't find sector!");
    return 0;
    }

    obj->Set_sector(sector); // needs to change to using the pointer not the name 

  }// end if context






  if (node)
  {
    printf("Parseing node obj\n");
    // Create the new imybody object
    std::string name = node->GetAttribute("name")->GetValue ();
    float radius =  node->GetAttribute("r")->GetValueAsFloat();
    int verts = node->GetAttribute("verts")->GetValueAsInt();
    double day =  static_cast<double>(node->GetAttribute("day")->GetValueAsFloat());
    double i =  static_cast<double>(node->GetAttribute("i")->GetValueAsFloat());

    obj->Set_Name(name.c_str());

    csVector3 r(radius,0,0);
    obj->Create_Body_Mesh( radius, verts, day, i );

//    printf("Adding %s ", obj->Get_Name() );

    if (parent) printf(" child of %s\n", parent->Get_Name() );

    if (parent)
    {
	printf("Has Parent\n");
//       obj->Set_Parent(parent);
//       printf("Adding %s as child of %s\n", parent->Get_Name() , obj->Get_Name() );
//       parent->Add_Child(obj);
    }



    csRef<iDocumentNodeIterator> itr = node->GetNodes ();
    while (itr->HasNext ())
    {
      csRef<iDocumentNode> child = itr->Next ();
      if (child->GetType () != CS_NODE_ELEMENT) continue;

      csStringID id = xmltokens.Request (child->GetValue ());
      switch (id)
      {

        case XMLTOKEN_MESHFACTORY:
        {
        }
        break;


        case XMLTOKEN_ORBIT:
        {// <Orbit i='5.145' n='0' w='0' e='0.0549' a='0.00257' period='27.321582' scale='100000' >
          obj->Set_Orbit(  
                         (long double)child->GetAttribute("i")->GetValueAsFloat(),
                         (long double)child->GetAttribute("n")->GetValueAsFloat(),
                         (long double)child->GetAttribute("w")->GetValueAsFloat(),
                         (long double)child->GetAttribute("e")->GetValueAsFloat(),
                         (long double)child->GetAttribute("a")->GetValueAsFloat(),
                         (long double)child->GetAttribute("period")->GetValueAsFloat(),
                         (long double)child->GetAttribute("scale")->GetValueAsFloat()
                        );
        }
        break;

        case XMLTOKEN_MATERIAL:
        { 
          csRef<iMaterialWrapper> mat = ldr_context->FindMaterial(child->GetContentsValue()); 
          if (!mat) 
          {
            printf("myLoaderBody::ParseBody %s materal %s not found!\n", obj->Get_Name() , child->GetContentsValue() );
            goto error;
          }
          obj->Set_Material(mat);
        }
        break;

        case XMLTOKEN_LIGHT:
        {                 
          if (!ParseLight(child,obj)) return false;
        }
        break;

        case XMLTOKEN_BODY:
        {

          // Parse Next body
          //Create another body with this one as parent 
          // Create body with Null as parent 
          ParseBody ( child, ss, ldr_context, context , obj);


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
  printf("Regersting Obj %s \n\n", obj->Get_Name()); 
  if (!object_reg->Register (obj, obj->Get_Name()) )
  {
    printf ("myLoaderBody::Parse: failed to register iMyBody object in registery\n");
    return false;
  }

  if (parent) 
  {
    parent->Add_Child(obj);
    obj->Set_Parent(parent);
    printf("return parent\n");
    return parent;
  } else
  {
    printf("return root pointer\n");
    return obj;
  }

error:

  return 0;

}

