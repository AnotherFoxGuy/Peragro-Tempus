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

#ifndef TABLES_H
#define TABLES_H

class Database;

class UsersTable;
class CharacterTable;
class EntityTable;
class ItemTable;
class InventoryTable;
class StatTable;
class CharacterStatsTable;
class RaceTable;
class RaceStatsTable;
class RaceSkillsTable;
class SkillTable;
class CharacterSkillsTable;
class NpcAiSettingTable;
class NpcDialogsTable;
class NpcDialogAnswersTable;
class NpcEntitiesTable;
class SpawnPointsTable;
class BooksTable;
class DoorsTable;
class SectorsTable;
class PermissionsTable;
class MeshesTable;
class VerticesTable;
class TrianglesTable;
class ConfigTable;
class ZonesTable;
class ZonenodesTable;
class ReputationsTable;
class CharacterReputationsTable;

class Tables
{
private:
  UsersTable* userstable;
  CharacterTable* charactertable;
  EntityTable* entitytable;
  ItemTable* itemtable;
  InventoryTable* inventorytable;
  StatTable* stattable;
  CharacterStatsTable* characterstattable;
  RaceTable* racetable;
  RaceStatsTable* racestattable;
  RaceSkillsTable* raceskilltable;
  SkillTable* skilltable;
  CharacterSkillsTable* characterskillstable;
  NpcAiSettingTable* npcaisettingtable;
  NpcDialogsTable* npcdialogstable;
  NpcDialogAnswersTable* npcdialoganswerstable;
  NpcEntitiesTable* npcentitiestable;
  SpawnPointsTable* spawnpointstable;
  BooksTable* bookstable;
  DoorsTable* doorstable;
  SectorsTable* sectorstable;
  PermissionsTable* permissionstable;
  MeshesTable* meshestable;
  VerticesTable* verticestable;
  TrianglesTable* trianglestable;
  ConfigTable* configtable;
  ZonesTable* zonestable;
  ZonenodesTable* zonenodestable;
  ReputationsTable* reputationstable;
  CharacterReputationsTable* characterreputationstable;

public:
  Tables();
  virtual ~Tables();

  void init(Database* db);

  UsersTable* getUsersTable() { return userstable; }
  CharacterTable* getCharacterTable() { return charactertable; }
  EntityTable* getEntityTable() { return entitytable; }
  ItemTable* getItemTable() { return itemtable; }
  InventoryTable* getInventoryTable() { return inventorytable; }
  StatTable* getStatTable() { return stattable; }
  CharacterStatsTable* getCharacterStatTable() { return characterstattable; }
  RaceTable* getRaceTable() { return racetable; }
  RaceStatsTable* getRaceStatsTable() { return racestattable; }
  RaceSkillsTable* getRaceSkillsTable() { return raceskilltable; }
  SkillTable* getSkillTable() { return skilltable; }
  CharacterSkillsTable* getCharacterSkillsTable() { return characterskillstable; }
  NpcAiSettingTable* getNpcAiSettingTable() { return npcaisettingtable; }
  NpcDialogsTable* getNpcDialogsTable() { return npcdialogstable; }
  NpcDialogAnswersTable* getNpcDialogAnswersTable() { return npcdialoganswerstable; }
  NpcEntitiesTable* getNpcEntitiesTable() { return npcentitiestable; }
  SpawnPointsTable* getSpawnPointsTable() { return spawnpointstable; }
  BooksTable* getBooksTable() { return bookstable; }
  DoorsTable* getDoorsTable() { return doorstable; }
  SectorsTable* getSectorsTable() { return sectorstable; }
  PermissionsTable* getPermissionsTable() { return permissionstable; }
  MeshesTable* getMeshesTable() { return meshestable; }
  VerticesTable* getVerticesTable() { return verticestable; }
  TrianglesTable* getTrianglesTable() { return trianglestable; }
  ConfigTable* getConfigTable() { return configtable; }
  ZonesTable* getZonesTable() { return zonestable; }
  ZonenodesTable* getZonenodesTable() { return zonenodestable; }
  ReputationsTable* getReputationsTable() { return reputationstable; }
  CharacterReputationsTable* getCharacterReputationsTable() { return characterreputationstable; }

};

#endif // TABLES_H