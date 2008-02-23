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

#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <crystalspace.h>
#include <iutil/job.h>

#include <string>

struct iEngine;
struct iVFS;
struct iObjectRegistry;
struct iLoader;
struct iRegion;

//---------------------------------------------------------------------------

class LevelLoader
{
protected:

  class LoaderJob : public scfImplementation1<LoaderJob, iJob>
  {
  private:
    csPtr<iDocumentNode> WrapNode(iDocumentNode* node);

  public:
    //Plugins.
    csRef<iVFS> vfs;

    //Data
    bool finished;

    std::string path;
    std::string fileName;

    csArray<std::string> factories;
    csRefArray<iDocumentNode> instances;
    csRefArray<iDocumentNode> interiors;

    // Create new instance.
    LoaderJob (const std::string& path, const std::string& fileName);
    virtual ~LoaderJob();

    virtual void Run();
  };

protected:
  /// Reference to the job for loading this file.
  mutable csRef<LoaderJob> loadJob;
  /// Reference to job queue.
  mutable csRef<iJobQueue> jobQueue;

  iObjectRegistry* object_reg;

public:
  LevelLoader (iObjectRegistry* object_reg);
  virtual ~LevelLoader ();

  virtual bool Load (const std::string& path, const std::string& fileName);

  void WaitForJob() const;

  bool IsReady() const;

  csArray<std::string>* GetFactories() 
  { 
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->factories; 
  }

  csRefArray<iDocumentNode>* GetInstances() 
  { 
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->instances; 
  }

  csRefArray<iDocumentNode>* GetInteriors() 
  { 
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->interiors; 
  }
};

#endif // LEVELLOADER_H
