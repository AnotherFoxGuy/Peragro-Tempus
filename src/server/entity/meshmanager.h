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
#include "meshlist.h"
#include "server/database/table-meshlist.h"

class MeshManager
{
private:
  std::vector<const Mesh*> list;
  unsigned int mesh_id;
  unsigned int revision;

  std::vector<Mesh*> updateList;

public:
  MeshManager() : ent_id(0) {}

  ~MeshManager()
  {
    for (size_t i = 0; i < list.size(); i++)
    {
      delete list[i];
    }
  }

  void addMeshUpdate(Mesh* mesh)
  {
    updateList.push_back(mesh);
  }

  void runUpdate(MeshListTable* mlt)
  {
    for (size_t i = 0; i < updateList.size(); i++)
    {
      Mesh* mesh = updateList[i];
      mesh.setRevision(revision + 1);
      mlt->insert(mesh);
    }
    loadFromDB(mlt);
  }

  const std::vector<const Mesh*>& getAllMesh()
  {
  }

  void findChangedMeshSince(unsigned int revision, std::vector<const Mesh*>& result)
  {
    for (size_t i = 0; i< list.size(); i ++)
    {
      if (list[i]->getRevision() > revision)
      {
        result.push_back(list[i]);
      }
    }
  }

  void loadFromDB(MeshListTable* mlt)
  {
    mesh_id = mlt->getMaxId();
    revision = mlt->getMaxRevision();

    //Load all Meshs from Database
    mlt->getAllMeshs(list);
  }
};

#endif // MESHMANAGER_H
