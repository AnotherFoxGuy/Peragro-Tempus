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
#include <iengine/mesh.h>
#include <csutil/scf_implementation.h>
#include <iengine/collection.h>
#include <iutil/objreg.h>
#include <iutil/object.h>
#include <ivideo/texture.h>
#include <igeom/trimesh.h>
#include <imesh/objmodel.h>

#include <iutil/vfs.h>
#include <imap/loader.h>

#include <ivaria/reporter.h>

#include <imesh/object.h>

#include <csutil/cscolor.h>
#include <csgeom/tri.h>

#include "cacheentry.h"

#define INT       sizeof(int)
#define FLOAT     sizeof(float)
#define CSVECTOR3 sizeof(csVector3)
#define CSCOLOR4  sizeof(csColor4)
#define TRIANGLE  sizeof(csTriangle)

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
  isPrecached = false;
  cachedSize = 0;

  // Create our region.
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  collection = engine->CreateCollection(fileName.c_str());

  csRef<iThreadedLoader> loader = csQueryRegistry<iThreadedLoader> (object_reg);
  if (!loader) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate Loader!");
  //csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  //if (!vfs) Report(CS_REPORTER_SEVERITY_ERROR, "Failed to locate VFS!");

  // TODO: What if its not a full path?
  // just vfs->PushDir() won't work since its being loaded in another thread.
  threadReturn = loader->LoadLibraryFile(fileName.c_str(), collection);
} // end CacheEntry()

CacheEntry::~CacheEntry()
{
  // Stab Xordan for this!!
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);
  engine->RemoveCollection(fileName.c_str());
} // end ~CacheEntry()

bool CacheEntry::IsFinished() const
{
  return threadReturn->IsFinished();
} // end IsFinished()

bool CacheEntry::WasSuccessful() const
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
          Report(CS_REPORTER_SEVERITY_NOTIFY, "Precached texture '%s'\n", csth->QueryObject()->GetName());
          return;
        }
      }
    }
  }

  isPrecached = true;
} // end Precache()

csPtr<iMeshWrapper> CacheEntry::Create(const std::string& meshName,
                                 const std::string& factoryName)
{
  csRef<iEngine> engine = csQueryRegistry<iEngine> (object_reg);

  // Find the factory and turn it into a factorywrapper.
  csRef<iMeshFactoryWrapper> fmw = engine->FindMeshFactory(factoryName.c_str(), collection);
  if (!fmw)
  {
    Report(CS_REPORTER_SEVERITY_ERROR, "CacheEntry: Couldn't find factory: '%s'!", factoryName.c_str());
    return 0;
  }

  return engine->CreateMeshWrapper(fmw, meshName.c_str(), 0, csVector3(), false);
} // end Create()

size_t GetSize(iTextureWrapper* wrapper, iObjectRegistry* object_reg)
{
  if (wrapper->GetTextureHandle())
  {
    int w, h, d; w = h = d = 0;
    wrapper->GetTextureHandle()->GetOriginalDimensions(w, h, d);
    return (w * h * d) * INT; ///@todo what about mipmap size??
  }

  return 1;
} // end GetSize()

size_t GetSize(iMeshFactoryWrapper* w, iObjectRegistry* object_reg)
{
  size_t size = 0;

  csRef<iMeshFactoryList> children = w->GetChildren ();
  for (size_t i = 0; i < (size_t)children->GetCount(); i++)
  {
    csRef<iMeshFactoryWrapper> c = children->Get((int)i);
    size += GetSize(c, object_reg);
  }

  if (w->GetMeshObjectFactory())
  {
    iObjectModel* o = w->GetMeshObjectFactory()->GetObjectModel();
    if (o)
    {
      csRef<iStringSet> strings = csQueryRegistryTagInterface<iStringSet> (object_reg, "crystalspace.shared.stringset");
      csStringID id = strings->Request ("base");
      iTriangleMesh* m = o->GetTriangleData(id);
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

size_t CacheEntry::GetSize() const
{
  if (!cachedSize)
  {
    csRef<iObjectIterator> iter = collection->QueryObject()->GetIterator();
    while (iter->HasNext())
    {
      csRef<iObject> obj = iter->Next();
      {
        csRef<iTextureWrapper> w(scfQueryInterfaceSafe<iTextureWrapper>(obj));
        if (w) { cachedSize += ::GetSize(w, object_reg); continue;}
      }
      {
        csRef<iMeshFactoryWrapper> w(scfQueryInterfaceSafe<iMeshFactoryWrapper>(obj));
        if (w) { cachedSize += ::GetSize(w, object_reg); continue;}
      }

    } // end while
  } // end if

  return cachedSize;
} // end GetSize()

