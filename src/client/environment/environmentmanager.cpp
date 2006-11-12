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

#include "client/environment/environmentmanager.h"

#include <iutil/objreg.h>
#include <csutil/xmltiny.h>


EnvironmentMGR::EnvironmentMGR ()
{
}

EnvironmentMGR::~EnvironmentMGR ()
{
}

bool EnvironmentMGR::Initialize ()
{
  iObjectRegistry* obj_reg = PointerLibrary::getInstance()->getObjectRegistry();

  engine = CS_QUERY_REGISTRY(obj_reg, iEngine);
  if(!engine) return false;

  vfs = CS_QUERY_REGISTRY(obj_reg, iVFS);
  if(!vfs) return false;

  stringset = CS_QUERY_REGISTRY_TAG_INTERFACE (obj_reg,
    "crystalspace.shared.stringset", iStringSet);
  if(!stringset) return false;

//=============================================================================
  csRef<iDocumentSystem> docsys = CS_QUERY_REGISTRY(obj_reg, iDocumentSystem);

  csRef<iFile> buf = vfs->Open("/peragro/xml/environment/sun.xml", VFS_FILE_READ);

  if (!buf)
  {
    printf("EnvironmentMGR: ERROR Couldn't open sun file!\n");
    return false;
  }

  csRef<iDocument> doc;
  csRef<iDocumentNode> node;

  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    printf("EnvironmentMGR: ERROR Couldn't open sun file!\n");
    return false;
  }

  if (doc)
  {
    node = doc->GetRoot()->GetNode("lights");
    if (!node)
    {
      printf("EnvironmentMGR: ERROR Couldn't open sun file!\n");
      return false;
    }
  }

  printf("\n==Loading suncolors==========================\n");
  csRef<iDocumentNode> light      = node->GetNode ("light");
  sunname = (csString)light->GetAttributeValue("name");
  csRef<iDocumentNode> times      = light->GetNode ("times");
  csRef<iDocumentNodeIterator> it = times->GetNodes ();
  while (it->HasNext ())
  {
    csRef<iDocumentNode> child = it->Next ();
    csRef<iDocumentNode> color      = child->GetNode ("color");
    csRef<iDocumentNode> pos        = child->GetNode ("position");

    if (!color || !pos)
    {
      printf("EnvironmentMGR: ERROR Missing token(s)!\n");
      return false;
    }

    Sun* sun = new Sun();

    sun->id = child->GetAttributeValueAsInt("id");

    sun->color.red = (color->GetAttributeValueAsFloat("r"));
    sun->color.green = (color->GetAttributeValueAsFloat("g")); 
    sun->color.blue = (color->GetAttributeValueAsFloat("b")); 

    sun->position.x = (pos->GetAttributeValueAsFloat("x"));
    sun->position.y = (pos->GetAttributeValueAsFloat("y"));
    sun->position.z = (pos->GetAttributeValueAsFloat("z"));

    if (sun->id >= sunarray.Capacity())
      sunarray.SetCapacity(sun->id+1);

    sunarray.Put(sun->id, sun);
    printf("%d : %s\n", sun->id, sun->position.Description().GetData());
  }
  printf("================================= %d sun color(s)\n\n", sunarray.Length()-1);

//=============================================================================

  return true;
}

void EnvironmentMGR::Handle ()
{
  Time();
}

void EnvironmentMGR::Time(unsigned int hour)
{
  mutex.lock();
  printf("EnvironmentMGR: Adding time '%d'\n", hour);
  TimeData* timedata = new TimeData();
  timedata->hour = hour;
  timearray.Push(timedata);
  mutex.unlock();
}

void EnvironmentMGR::Time()
{
  if (!timearray.GetSize()) return;
  mutex.lock();
  TimeData* timedata = timearray.Pop();

  if(timedata->hour < sunarray.Length())
  {
    Sun* sun = sunarray[timedata->hour];
    if(!sun) return;
    engine->SetAmbientLight(sun->color);
  }
  else
    printf("EnvironmentMGR: Failed to update sun!\n");

  delete timedata;
  mutex.unlock();
}