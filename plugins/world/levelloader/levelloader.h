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
/**
 * @file levelloader.h
 *
 * @basic Loads game levels.
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

/**
 * Loads a game level.
 */
class LevelLoader
{
protected:

  /**
   * Loads one file.
   */
  class LoaderJob : public scfImplementation1<LoaderJob, iJob>
  {
  private:
    /**
     * Put a document node inside another node.
     * @param node The node to wrap.
     * @return A new node with the original inside.
     */
    csPtr<iDocumentNode> WrapNode(iDocumentNode* node);

  public:
    /// The virtual file system.
    csRef<iVFS> vfs;

    /// Whether the loading has finished.
    bool finished;

    /// The path to the file.
    std::string path;
    /// The file name to load.
    std::string fileName;

    /// The names of the loaded factories.
    csArray<std::string> factories;
    /// The document nodes of the loaded instances.
    /// (meshes/portals/lights/...)
    csRefArray<iDocumentNode> instances;
    /// The document nodes of the loaded interiors.
    csRefArray<iDocumentNode> interiors;

    /**
     * Create a new job to load from a file.
     * @param path The path to the file.
     * @param fileName The file name to load.
     */
    LoaderJob (const std::string& path, const std::string& fileName);

    /// Destructor.
    virtual ~LoaderJob();

    /**
     * Start loading from the file.
     */
    virtual void Run();
  };

protected:
  /// Reference to the job for loading the current file.
  mutable csRef<LoaderJob> loadJob;
  /// Reference to the job queue.
  mutable csRef<iJobQueue> jobQueue;

  /// The object registry.
  iObjectRegistry* object_reg;

public:
  /// Constructor.
  LevelLoader (iObjectRegistry* object_reg);
  /// Destructor.
  virtual ~LevelLoader ();

  /**
   * Start loading from a file.
   * @param path The path to the file.
   * @param fileName The file name to load.
   */
  virtual bool Load (const std::string& path, const std::string& fileName);

  /**
   * Check if a job is still in the queue.
   * If yes, remove it from the queue and run it immediately.
   */
  void WaitForJob() const;

  /// Return true if all enqueued jobs are finished.
  bool IsReady() const;

  /**
   * Get an array of factory names from the current file.
   * @return The factory names.
   */
  csArray<std::string>* GetFactories()
  {
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->factories;
  }

  /**
   * Get an array of instance names from the current file.
   * @return The instance names. (meshes/portals/lights/...)
   */
  csRefArray<iDocumentNode>* GetInstances()
  {
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->instances;
  }

  /**
   * Get an array of interior names from the current file.
   * @return The interior names.
   */
  csRefArray<iDocumentNode>* GetInteriors()
  {
    if (!loadJob) return 0;
    WaitForJob();
    return &loadJob->interiors;
  }
};

#endif // LEVELLOADER_H
