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

#ifndef MESHMANAGER_H
#define MESHMANAGER_H

#include <vector>

#include "mesh.h"
#include "server/database/table-meshlist.h"
#include "common/util/mutex.h"

class MeshManager
{
private:
  std::vector<const Mesh*> list;
  unsigned int mesh_id;
  unsigned int revision;

  bool hasRevisionChanges;

  Mutex mutex;

  MeshListTable* mlt;

  void loadFromDB()
  {
    if (!list.empty())
    {
      for (size_t i = 0; i < list.size(); i++)
      {
        delete list[i];
      }
      list.clear();
    }

    mesh_id = mlt->getMaxId();
    revision = mlt->getMaxRevision();

    //Load all Meshs from Database
    mlt->getAll(list);
    hasRevisionChanges = false;
  }

  const Mesh* findByName(const ptString& name, bool doUpdate)
  {
    ptScopedMutex scopedMutex(mutex);

    if (hasRevisionChanges && doUpdate) loadFromDB();

    for (size_t i = 0; i < list.size(); i++)
    {
      const Mesh* mesh = list[i];
      if (mesh != 0 && mesh->getName() == name)
      {
        return mesh;
      }
    }
    return 0;
  }

public:
  MeshManager(MeshListTable* mlt) : mesh_id(0), revision(0), mlt(mlt)
  {
    hasRevisionChanges = true;
  }

  ~MeshManager()
  {
    for (size_t i = 0; i < list.size(); i++)
    {
      delete list[i];
    }
  }

  const Mesh* addMeshUpdate(ptString name, ptString file)
  {
    const Mesh* mesh = findByName(name, false);

    ptScopedMutex scopedMutex(mutex);

    if (mesh != 0 && file == mesh->getFile())
    {
      return mesh;
    }

    if (mesh == 0)
    {
      Mesh* newMesh = new Mesh();
      newMesh->setId(++mesh_id);
      newMesh->setRevision(revision + 1);
      newMesh->setName(name);
      newMesh->setFile(file);

      list.push_back(newMesh);
      mesh = newMesh;
    }
    else if (file != mesh->getFile())
    {
      Mesh* updateMesh = (Mesh*) mesh;
      updateMesh->setRevision(revision + 1);
      updateMesh->setFile(file);
      mlt->remove(mesh->getId());
    }

    mlt->insert(mesh);

    return mesh;
  }

  const Mesh* findById(unsigned int id)
  {
    ptScopedMutex scopedMutex(mutex);

    if (hasRevisionChanges) loadFromDB();

    if (id < list.size())
    {
      const Mesh* mesh = list[id];
      if (mesh != 0 && mesh->getId() == id)
      {
        return mesh;
      }
    }
    return 0;
  }

  const Mesh* findByName(const ptString& name)
  {
    ptScopedMutex scopedMutex(mutex);

    if (hasRevisionChanges) loadFromDB();

    return findByName(name, true);
  }

  void findChangedMeshSince(unsigned int revision, std::vector<const Mesh*>& result)
  {
    ptScopedMutex scopedMutex(mutex);

    if (hasRevisionChanges) loadFromDB();

    for (size_t i = 0; i< list.size(); i ++)
    {
      if (list[i]->getRevision() > revision)
      {
        result.push_back(list[i]);
      }
    }
  }
};

#endif // MESHMANAGER_H
