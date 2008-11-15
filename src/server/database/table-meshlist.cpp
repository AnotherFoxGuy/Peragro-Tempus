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

#include "common/database/database.h"
#include "table-meshlist.h"

#include "server/entity/mesh.h"

MeshListTable::MeshListTable(Database* db) : db(db)
{
}

const Mesh* MeshListTable::parseSingleResultSet(ResultSet* rs, size_t row)
{
  Mesh* vo = new Mesh();
  vo->setId(atoi(rs->GetData(row,0).c_str()));
  vo->setRevision(atoi(rs->GetData(row,1).c_str()));
  vo->setName(ptString(rs->GetData(row,2).c_str(), rs->GetData(row,2).length()));
  vo->setFile(ptString(rs->GetData(row,3).c_str(), rs->GetData(row,3).length()));
  return vo;
}

void MeshListTable::parseMultiResultSet(ResultSet* rs, std::vector<const Mesh*>& list)
{
  for (size_t i = 0; i < rs->GetRowCount(); i++)
  {
    const Mesh* obj = parseSingleResultSet(rs, i);
    list.push_back(obj);
  }
}

void MeshListTable::createTable()
{
  db->update("create table meshlist ("
             "id INTEGER,"
             "revision INTEGER,"
             "name TEXT,"
             "file TEXT"
             "PRIMARY KEY (Id) );");
}

void MeshListTable::insert(const Mesh* vo)
{
  const char* query = { "insert into meshlist(id, revision, name, file) values (%d, %d, '%s', '%s');" };
  db->update(query, vo->getId(), vo->getRevision(), *vo->getName(), *vo->getFile());
}

void MeshListTable::remove(int id)
{
  db->update("delete from meshlist where id = %d");
}

unsigned int MeshListTable::getMaxId()
{
  ResultSet* rs = db->query("select max(id) from meshlist");
  if (!rs || rs->GetRowCount() == 0)
    return 0;

  unsigned int id = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return id;
}

unsigned int MeshListTable::getMaxRevision()
{
  ResultSet* rs = db->query("select max(revision) from meshlist");
  if (!rs || rs->GetRowCount() == 0)
    return 0;

  unsigned int revision = atoi(rs->GetData(0,0).c_str());

  delete rs;
  return revision;
}

void MeshListTable::getAll(std::vector<const Mesh*>& list)
{
  ResultSet* rs = db->query("select * from meshlist;");
  parseMultiResultSet(rs, list);
  delete rs;
}

