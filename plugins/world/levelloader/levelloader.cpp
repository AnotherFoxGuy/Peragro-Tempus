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

#include "levelloader.h"
#include "../utils.h"

//---------------------------------------------------------------------------

LevelLoader::LoaderJob::LoaderJob (const std::string& path,
                                   const std::string& fileName)
  : scfImplementationType (this)
{
  LoaderJob::path = path;
  LoaderJob::fileName = fileName;
  finished = false;
} // end LoaderJob()

LevelLoader::LoaderJob::~LoaderJob()
{
  factories.DeleteAll();
  instances.DeleteAll();
} // end ~LoaderJob()

void LevelLoader::LoaderJob::Run()
{
  std::string pathFile = path + fileName;

  if (!vfs->Exists(pathFile.c_str()))
  {
    printf("E: No such file '%s'!\n", pathFile.c_str());
    finished = true;
    return;
  }

  csRef<iDataBuffer> buf = vfs->ReadFile(pathFile.c_str());
  if (!buf.IsValid())
  {
    printf("E: Failed to open file '%s'!\n", pathFile.c_str());
    finished = true;
    return;
  }

  csRef<iDocument> doc;
  csRef<iDocumentSystem> docsys;
  if (!docsys) docsys = csPtr<iDocumentSystem> (new csTinyDocumentSystem ());
  doc = docsys->CreateDocument ();
  const char* error = doc->Parse (buf, true);
  if (error != 0)
  {
    printf("E: Error while parsing file '%s'\n!", pathFile.c_str());
    finished = true;
    return;
  }

  csRef<iDocumentNode> root = doc->GetRoot();
  csRef<iDocumentNode> parentNode = root->GetNode("tile");
  if (!parentNode.IsValid()) parentNode = root->GetNode("interior");
  if (!parentNode.IsValid())
  {
    printf("E: Failed to get parent node for file '%s'\n!", pathFile.c_str());
    finished = true;
    return;
  }

  // Parse factories (libraries).
  csRef<iDocumentNodeIterator> nodes = parentNode->GetNodes("library");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    std::string fileName = current->GetContentsValue();
    factories.Push(fileName);
  }

  // Parse instances (meshobjs).
  /*csRef<iDocumentNodeIterator>*/ nodes = parentNode->GetNodes("meshobj");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    // A little hack, CS expects the root to be given but we only got the meshobj node.
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    instances.Push(newNode);
  }

  // Parse instances (meshrefs).
  /*csRef<iDocumentNodeIterator>*/ nodes = parentNode->GetNodes("meshref");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    instances.Push(newNode);
  }

  // Parse portals.
  /*csRef<iDocumentNodeIterator>*/ nodes = parentNode->GetNodes("portals");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    instances.Push(newNode);
  }

  // Parse lights.
  /*csRef<iDocumentNodeIterator>*/ nodes = parentNode->GetNodes("light");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    instances.Push(newNode);
  }

  // Parse interiors.
  /*csRef<iDocumentNodeIterator>*/ nodes = parentNode->GetNodes("interior");
  while (nodes->HasNext())
  {
    csRef<iDocumentNode> current = nodes->Next();
    csRef<iDocumentNode> newNode = PT::Utils::WrapNode(current);
    interiors.Push(newNode);
  }

  finished = true;

} // end Run()

//---------------------------------------------------------------------------

LevelLoader::LevelLoader (iObjectRegistry* object_reg)
{
  LevelLoader::object_reg = object_reg;

  static const char queueTag[] = "peragro.jobqueue.levelparser";
  jobQueue = csQueryRegistryTagInterface<iJobQueue> (object_reg, queueTag);
  if (!jobQueue.IsValid())
  {
    jobQueue.AttachNew (new CS::Threading::ThreadedJobQueue (1, CS::Threading::THREAD_PRIO_LOW));
    object_reg->Register (jobQueue, queueTag);
  }
} // end LevelLoader()

LevelLoader::~LevelLoader ()
{
  if (loadJob.IsValid())
    jobQueue->Unqueue (loadJob);
} // end ~LevelLoader()

bool LevelLoader::Load (const std::string& path, const std::string& fileName)
{
  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  // Create the loadjob.
  loadJob.AttachNew (new LoaderJob (path, fileName));

  // Add the plugins.
  loadJob->vfs = vfs;

  // Add to the queue.
  jobQueue->Enqueue (loadJob);

  return true;
} // end Load()

void LevelLoader::WaitForJob() const
{
  jobQueue->PullAndRun (loadJob);
} // end WaitForJob()

bool LevelLoader::IsReady() const
{
  return loadJob->finished;
} // end IsReady()
