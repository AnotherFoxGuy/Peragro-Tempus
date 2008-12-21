/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

#include <cssysdef.h>
#include <csutil/ref.h>
#include <iengine/texture.h>
#include <iengine/engine.h>
#include <csutil/scf_implementation.h>
#include <iengine/collection.h>
#include <iutil/objreg.h>
#include <iutil/object.h>
#include <ivideo/texture.h>

#include "cacheentry.h"

#define INT       sizeof(int)
#define FLOAT     sizeof(float)
#define CSVECTOR3 sizeof(csVector3)
#define CSCOLOR4  sizeof(csColor4)
#define TRIANGLE  sizeof(csTriangle)

namespace PT
{
  void CacheEntry::Report(int severity, const char* msg, ...)
  {
    va_list arg;
    va_start (arg, msg);
    csReportV(object_reg, severity, "peragro.cacheentry", msg, arg);
    va_end (arg);
  }

  CacheEntry::CacheEntry(const std::string& fileName, iObjectRegistry* object_reg)
    : scfImplementationType (this)
  {
    this->fileName = fileName;
    this->object_reg = object_reg;
    fileLoader = 0;
    isPrecached = false;
    cachedSize = 0;

    // Create our region.
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    collection = engine->CreateCollection(fileName.c_str());

    // Seperate the path of the filename.
    size_t p = fileName.find_last_of("/");
    std::string path = fileName.substr(0,p+1);
    std::string file = fileName.substr(p+1,fileName.length());

    csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
    if (!loader) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate Loader!");
    csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
    if (!vfs) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate VFS!");

    vfs->SetSyncDir(path.c_str());
    threadReturn = loader->LoadLibraryFile(file.c_str(), collection);
  } // end CacheEntry()

  CacheEntry::~CacheEntry()
  {
    // Stab Xordan for this!!
    csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
    engine->RemoveCollection(fileName.c_str());
  } // end ~CacheEntry()

  bool CacheEntry::IsFinished()
  {
    return threadReturn->IsFinished();
  } // end IsFinished()

  bool CacheEntry::WasSuccessful()
  {
    return threadReturn->WasSuccessful();
  } // end WasSuccessful()

  void CacheEntry::Precache()
  {
    if (!IsFinished()) return;
    if (!collection.IsValid()) return;
    if (IsPrecached()) return;

    // Precaches "one" texture and then returns, so you have to call
    // this repeately and check IsPrecached() whether 'everything' is done.
    csRef<iObjectIterator> iter = collection->QueryObject()->GetIterator();
    while (iter->HasNext())
    {
      csRef<iTextureWrapper> csth(
        scfQueryInterfaceSafe<iTextureWrapper> (iter->Next()) );
      if (csth)
      {
        if (csth->GetTextureHandle ())
        {
          if (csth->GetTextureHandle ()->IsPrecached())
            continue;
          else
          {
            csth->GetTextureHandle()->Precache();
            Report("I: Precached texture '%s'\n",
              csth->QueryObject()->GetName());
            return;
          }
        }
      }
    }

    isPrecached = true;
  } // end Precache()

  iMeshWrapper* CacheEntry::Create(const std::string& meshName,
                                   const std::string& factoryName)
  {
    return 0;
  } // end Create()

  size_t GetSize(iTextureWrapper* w)
  {
    if (w->GetTextureHandle())
    {
      int w, h, d; w = h = d = 0;
      w->GetTextureHandle()->GetOriginalDimensions(w, h, d);
      return (w * h * d) * INT; ///@todo what about mipmap size??
    }
    
    return 1;
  } // end GetSize()

  size_t GetSize(iMeshFactoryWrapper* w)
  {
    size_t size = 0;

    csRef<iMeshFactoryList> children = w->GetChildren ();
    for (size_t i = 0; i < children->GetCount(); i++)
    {
       csRef<iMeshFactoryWrapper> c = Get(i);
       size += GetSize(c);
    }

    if (w->GetMeshObjectFactory())
    {
      iObjectModel* o = w->GetMeshObjectFactory()->GetObjectModel();
      if (o)
      {
         iTriangleMesh* m =	o->GetTriangleData(csStringID id);
         if (m)
         {
           size += m->GetTriangleCount() * TRIANGLE;
           size += m->GetVertexCount() * (CSVECTOR3 + CSVECTOR3 + CSCOLOR4); // vertex + normal + color.
           return size;
         }
      }
    }

    return 1;
  } // end GetSize()

  size_t CacheEntry::GetSize()
  {
    if (cachedSize)
      return cachedSize;
    else
    {
      csRef<iObjectIterator> iter = collection->QueryObject()->GetIterator();
      while (iter->HasNext())
      {
        csRef<iObject> obj = iter->Next();
        {
          csRef<iTextureWrapper> w(scfQueryInterfaceSafe<iTextureWrapper>(obj));
          if (w) { cachedSize += GetSize(w); continue;}
        }
        {
          csRef<iMeshFactoryWrapper> w(scfQueryInterfaceSafe<iMeshFactoryWrapper>(obj));
          if (w) { cachedSize += GetSize(w); continue;}
        }

      } // end while
    } // end if
  } // end GetSize()


} // PT namespace

