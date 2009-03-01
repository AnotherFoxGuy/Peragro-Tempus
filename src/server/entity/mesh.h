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

#ifndef MESH_H
#define MESH_H

#include "common/util/stringstore.h"
#include "common/util/ptstring.h"

class Mesh
{
public:
  static const unsigned int NoMesh = 0;

private:

  unsigned int id;
  unsigned int revision;
  ptString name;
  ptString file;

public:
  Mesh() : id(-1)
  {
  }

  Mesh(const Mesh& mesh)
  {
    id = mesh.id;
    revision = mesh.revision;
    name = mesh.name;
    file = mesh.file;
  }

  ~Mesh()
  {
  }

  bool compare(Mesh* other)
  {
    if (this == other)
      return true;

    return (this->id == other->id);
  }

  void setId(unsigned int id) { this->id = id; }
  unsigned int GetId() const { return id; }

  void setRevision(unsigned int revision) { this->revision = revision; }
  unsigned int getRevision() const { return revision; }

  void setName(ptString name) { this->name = name; }
  const ptString& getName() const { return name; }

  void setFile(ptString file) { this->file = file; }
  const ptString& getFile() const { return file; }
};

#endif // MESH_H
