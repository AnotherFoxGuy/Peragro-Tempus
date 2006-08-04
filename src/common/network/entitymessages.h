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

#ifndef _ENTITIYMESSAGES_H_
#define _ENTITIYMESSAGES_H_

#include "netmessage.h"

namespace ENTITY
{
  enum MESSAGES
  {
    ADD=0,
    REMOVE=1,
    MOVE=2,
    MOVE_REQUEST=3,
    DRUPDATE_REQUEST=4,
    DRUPDATE=5,
    PICK_REQUEST=6,
    PICK_RESPONSE=7,
    DROP_REQUEST=8,
    DROP_RESPONSE=9,
    INV_ITEM_LIST=10,
    CHAR_STAT_LIST=11,
    UPDATE_STATS=12,
    MOVE_TO=13,
    MOVE_TO_REQUEST=14,
    CHAR_SKILL_LIST=15,
    EQUIP_REQUEST=16,
    EQUIP=17,
    OPEN_DOOR_REQUEST=18,
    OPEN_DOOR_RESPONSE=19,
    CLOSE_DOOR_REQUEST=20,
    CLOSE_DOOR_RESPONSE=21
  };
}

class AddEntityMessage : public NetMessage
{
  ptString name;
  ptString mesh;
  float pos[3];
  ptString sector;
  int ent_id;
  char ent_type;

public:
  AddEntityMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::ADD) 
  {
  }

  void serialise(ByteStream* bs)
  {
    assert(*name); assert(*mesh); assert(*sector);
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(ent_id);
    serial.setInt8(ent_type);
    serial.setString(name);
    serial.setString(mesh);
    serial.setFloat(pos[0]);
    serial.setFloat(pos[1]);
    serial.setFloat(pos[2]);
    serial.setString(sector);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    ent_id = serial.getInt32();
    ent_type = serial.getInt8();
    name = serial.getString();
    mesh = serial.getString();
    pos[0] = serial.getFloat();
    pos[1] = serial.getFloat();
    pos[2] = serial.getFloat();
    sector = serial.getString();
  }

  ptString& getName()
  {
    return name;
  }
  void setName(ptString n)
  {
    name = n;
  }

  int getId()
  {
    return ent_id;
  }
  void setId(int i)
  {
    ent_id = i;
  }

  char getType()
  {
    return ent_type;
  }
  void setType(char t)
  {
    ent_type = t;
  }

  ptString& getMesh()
  {
    return mesh;
  }
  void setMesh(ptString m)
  {
    mesh = m;
  }

  float* getPos()
  {
    return pos;
  }
  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  void setPos(float x,float y,float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }

  ptString& getSector()
  {
    return sector;
  }
  void setSector(ptString s)
  {
    sector = s;
  }
};

class RemoveEntityMessage : public NetMessage
{
  int ent_id;
  char ent_type;
  ptString name;

public:
  RemoveEntityMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::REMOVE) 
  {
  }

  ~RemoveEntityMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(ent_id);
    serial.setInt8(ent_type);
    serial.setString(name);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    ent_id = serial.getInt32();
    ent_type = serial.getInt8();
    name = serial.getString();
  }

  int getId()
  {
    return ent_id;
  }
  void setId(int i)
  {
    ent_id = i;
  }

  char getType()
  {
    return ent_type;
  }
  void setType(char t)
  {
    ent_type = t;
  }

  ptString& getName()
  {
    return name;
  }
  void setName(ptString n)
  {
    name = n;
  }
};

class MoveEntityMessage : public NetMessage
{
  float walk;
  float turn;
  int entity_id;

public:
  MoveEntityMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::MOVE) 
  {
  }


  ~MoveEntityMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setFloat(walk);
    serial.setFloat(turn);
    serial.setInt32(entity_id);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    walk = serial.getFloat();
    turn = serial.getFloat();
    entity_id = serial.getInt32();
  }

  void setRot(float direction)
  {
    turn = direction;
  }
  float getRot()
  {
    return turn;
  }

  void setWalk(float direction)
  {
    walk = direction;
  }
  float getWalk()
  {
    return walk;
  }

  int getId()
  {
    return entity_id;
  }
  void setId(int id)
  {
    entity_id = id;
  }
};

class MoveEntityRequestMessage : public NetMessage
{
  char walk;
  char turn;

public:
  MoveEntityRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::MOVE_REQUEST) 
  {
  }

  ~MoveEntityRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(walk);
    serial.setInt8(turn);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    walk = serial.getInt8();
    turn = serial.getInt8();
  }

  void setWalk(int direction)
  {
    walk = (char)(direction+1);
  }
  int getWalk()
  {
    return int(walk)-1;
  }

  void setRot(int direction)
  {
    turn = (char)(direction+1);
  }
  int getRot()
  {
    return int(turn)-1;
  }
};

class UpdateDREntityRequestMessage : public NetMessage
{
  bool on_ground;
  float speed, rot, avel;
  float pos[3], vel[3], wvel[3];
  ptString sector;

public:
  UpdateDREntityRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::DRUPDATE_REQUEST) 
  {
  }

  ~UpdateDREntityRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(on_ground);
    serial.setFloat(speed);
    serial.setFloat(rot);
    serial.setFloat(avel);
    serial.setFloat(pos[0]);
    serial.setFloat(pos[1]);
    serial.setFloat(pos[2]);
    serial.setFloat(vel[0]);
    serial.setFloat(vel[1]);
    serial.setFloat(vel[2]);
    serial.setFloat(wvel[0]);
    serial.setFloat(wvel[1]);
    serial.setFloat(wvel[2]);
    serial.setString(sector);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    on_ground = serial.getInt8() != 0;
    speed = serial.getFloat();
    rot = serial.getFloat();
    avel = serial.getFloat();
    pos[0] = serial.getFloat();
    pos[1] = serial.getFloat();
    pos[2] = serial.getFloat();
    vel[0] = serial.getFloat();
    vel[1] = serial.getFloat();
    vel[2] = serial.getFloat();
    wvel[0] = serial.getFloat();
    wvel[1] = serial.getFloat();
    wvel[2] = serial.getFloat();
    sector = serial.getString();
  }

  void setOnGround(bool state)
  {
    on_ground = state != 0;
  }
  bool getOnGround()
  {
    return on_ground;
  }

  void setSpeed(float s)
  {
    speed = s;
  }
  float getSpeed()
  {
    return speed;
  }

  void setRot(float r)
  {
    rot = r;
  }
  float getRot()
  {
    return rot;
  }

  void setAVel(float v)
  {
    avel = v;
  }
  float getAVel()
  {
    return avel;
  }

  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  void setPos(float x,float y,float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }
  float* getPos()
  {
    return pos;
  }

  void setVel(float p[3])
  {
    vel[0] = p[0];
    vel[1] = p[1];
    vel[2] = p[2];
  }
  void setVel(float x,float y,float z)
  {
    vel[0] = x;
    vel[1] = y;
    vel[2] = z;
  }
  float* getVel()
  {
    return vel;
  }

  void setWVel(float p[3])
  {
    wvel[0] = p[0];
    wvel[1] = p[1];
    wvel[2] = p[2];
  }
  void setWVel(float x,float y,float z)
  {
    wvel[0] = x;
    wvel[1] = y;
    wvel[2] = z;
  }
  float* getWVel()
  {
    return wvel;
  }

  void setSector(ptString s)
  {
    sector = s;
  }
  ptString& getSector()
  {
    return sector;
  }
};

class UpdateDREntityMessage : public NetMessage
{
  int entity_id;
  bool on_ground;
  float speed, rot, avel;
  float pos[3], vel[3], wvel[3];
  ptString sector;

public:
  UpdateDREntityMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::DRUPDATE) 
  {
  }

  ~UpdateDREntityMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(on_ground);
    serial.setFloat(speed);
    serial.setFloat(rot);
    serial.setFloat(avel);
    serial.setFloat(pos[0]);
    serial.setFloat(pos[1]);
    serial.setFloat(pos[2]);
    serial.setFloat(vel[0]);
    serial.setFloat(vel[1]);
    serial.setFloat(vel[2]);
    serial.setFloat(wvel[0]);
    serial.setFloat(wvel[1]);
    serial.setFloat(wvel[2]);
    serial.setInt32(entity_id);
    serial.setString(sector);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    on_ground = serial.getInt8() != 0;
    speed = serial.getFloat();
    rot = serial.getFloat();
    avel = serial.getFloat();
    pos[0] = serial.getFloat();
    pos[1] = serial.getFloat();
    pos[2] = serial.getFloat();
    vel[0] = serial.getFloat();
    vel[1] = serial.getFloat();
    vel[2] = serial.getFloat();
    wvel[0] = serial.getFloat();
    wvel[1] = serial.getFloat();
    wvel[2] = serial.getFloat();
    entity_id = serial.getInt32();
    sector = serial.getString();
  }

  int getId()
  {
    return entity_id;
  }
  void setId(int id)
  {
    entity_id = id;
  }

  void setOnGround(bool state)
  {
    on_ground = state != 0;
  }
  bool getOnGround()
  {
    return on_ground;
  }

  void setSpeed(float s)
  {
    speed = s;
  }
  float getSpeed()
  {
    return speed;
  }

  void setRot(float r)
  {
    rot = r;
  }
  float getRot()
  {
    return rot;
  }

  void setAVel(float v)
  {
    avel = v;
  }
  float getAVel()
  {
    return avel;
  }

  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  float* getPos()
  {
    return pos;
  }

  void setVel(float p[3])
  {
    vel[0] = p[0];
    vel[1] = p[1];
    vel[2] = p[2];
  }
  float* getVel()
  {
    return vel;
  }

  void setWVel(float p[3])
  {
    wvel[0] = p[0];
    wvel[1] = p[1];
    wvel[2] = p[2];
  }
  float* getWVel()
  {
    return wvel;
  }

  void setSector(ptString s)
  {
    sector = s;
  }
  ptString& getSector()
  {
    return sector;
  }
};

class PickEntityRequestMessage : public NetMessage
{
  int target;

public:
  PickEntityRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::PICK_REQUEST) 
  {
  }

  ~PickEntityRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(target);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    target = serial.getInt32();
  }

  void setTargetId(int target)
  {
    this->target = target;
  }
  int getTargetId()
  {
    return target;
  }

};

class PickEntityResponseMessage : public NetMessage
{
  ptString name;
  int itemId;
  ptString target;
  ptString error;

public:
  PickEntityResponseMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::PICK_RESPONSE) 
  {
  }

  ~PickEntityResponseMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(name);
    serial.setInt32(itemId);
    serial.setString(target);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    name = serial.getString();
    itemId = serial.getInt32();
    target = serial.getString();
    error = serial.getString();
  }

  void setName(ptString name)
  {
    this->name = name;
  }
  ptString& getName()
  {
    return name;
  }

  void setItemId(int itemId)
  {
    this->itemId = itemId;
  }
  int getItemId()
  {
    return itemId;
  }

  void setTarget(ptString target)
  {
    this->target = target;
  }
  ptString& getTarget()
  {
    return target;
  }

  void setError(ptString error)
  {
    this->error = error;
  }
  ptString& getError()
  {
    return error;
  }
};

class DropEntityRequestMessage : public NetMessage
{
  int target;

public:
  DropEntityRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::DROP_REQUEST) 
  {
  }

  ~DropEntityRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(target);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    target = serial.getInt32();
  }

  void setTargetId(int target)
  {
    this->target = target;
  }
  int getTargetId()
  {
    return target;
  }

};

class DropEntityResponseMessage : public NetMessage
{
  ptString name;
  ptString target;
  ptString error;

public:
  DropEntityResponseMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::DROP_RESPONSE) 
  {
  }

  ~DropEntityResponseMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setString(name);
    serial.setString(target);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    name = serial.getString();
    target = serial.getString();
    error = serial.getString();
  }

  void setName(ptString name)
  {
    this->name = name;
  }
  ptString& getName()
  {
    return name;
  }

  void setTarget(ptString target)
  {
    this->target = target;
  }
  ptString& getTarget()
  {
    return target;
  }

  void setError(ptString error)
  {
    this->error = error;
  }
  ptString& getError()
  {
    return error;
  }
};

class InventoryItemListMessage : public NetMessage
{
  char itemCount;

  class nwItems
  {
  public:
    int id;
    ptString name;
    int amount;
  };

  nwItems* items;

public:
  InventoryItemListMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::INV_ITEM_LIST), items(0) {}
  ~InventoryItemListMessage() { delete [] items; }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(itemCount);
    for (int i=0; i<itemCount; i++)
    {
      serial.setInt32(items[i].id);
      serial.setString(items[i].name);
      serial.setInt32(items[i].amount);
    }
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    setItemCount(serial.getInt8());
    for (int i=0; i<itemCount; i++)
    {
      items[i].id = serial.getInt32();
      items[i].name = serial.getString();
      items[i].amount = serial.getInt32();
    }
  }

  char getItemCount() { return itemCount; }
  void setItemCount(char ic) 
  {
    itemCount = ic; 
    delete [] items; 
    items = new nwItems[ic];
  }

  int getItemId(int idx) { return items[idx].id; }
  void setItemId(int idx, int item_id) { items[idx].id = item_id; }

  int getItemAmount(int idx) { return items[idx].amount; }
  void setItemAmount(int idx, int amount) { items[idx].amount = amount; }

  void setName(int idx, ptString name)
  {
    items[idx].name = name;
  }
  ptString& getName(int idx)
  {
    return items[idx].name;
  }
};

class CharacterStatListMessage : public NetMessage
{
  char statCount;

  class nwStats
  {
  public:
    short id;
    ptString name;
    short level;
  };

  nwStats* stats;

public:
  CharacterStatListMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::CHAR_STAT_LIST), stats(0) {}
  ~CharacterStatListMessage() { delete [] stats; }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(statCount);
    for (int i=0; i<statCount; i++)
    {
      serial.setInt16(stats[i].id);
      serial.setString(stats[i].name);
      serial.setInt16(stats[i].level);
    }
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    setStatCount(serial.getInt8());
    for (int i=0; i<statCount; i++)
    {
      stats[i].id = serial.getInt16();
      stats[i].name = serial.getString();
      stats[i].level = serial.getInt16();
    }
  }

  char getStatCount() { return statCount; }
  void setStatCount(char ic) 
  {
    statCount = ic; 
    delete [] stats; 
    stats = new nwStats[ic];
  }

  int getStatId(short idx) { return stats[idx].id; }
  void setStatId(size_t idx, short stat_id) { stats[idx].id = stat_id; }

  int getStatLevel(short idx) { return stats[idx].level; }
  void setStatLevel(size_t idx, short level) { stats[idx].level = level; }

  void setName(int idx, ptString name)
  {
    stats[idx].name = name;
  }
  ptString& getName(int idx)
  {
    return stats[idx].name;
  }
};

class UpdateStatsMessage : public NetMessage
{
  short stat_id;
  short level;

public:
  UpdateStatsMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::UPDATE_STATS) {}
  ~UpdateStatsMessage() { }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt16(stat_id);
    serial.setInt16(level);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    stat_id = serial.getInt16();
    level = serial.getInt16();
  }

  int getStatId() { return stat_id; }
  void setStatId(short stat_id) { this->stat_id = stat_id; }

  int getStatLevel() { return level; }
  void setStatLevel(short level) { this->level = level; }
};

class MoveEntityToMessage : public NetMessage
{
  float from[3];
  float to[3];
  float speed;
  int entity_id;

public:
  MoveEntityToMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::MOVE_TO) 
  {
  }


  ~MoveEntityToMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setFloat(from[0]);
    serial.setFloat(from[1]);
    serial.setFloat(from[2]);
    serial.setFloat(to[0]);
    serial.setFloat(to[1]);
    serial.setFloat(to[2]);
    serial.setFloat(speed);
    serial.setInt32(entity_id);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    from[0] = serial.getFloat();
    from[1] = serial.getFloat();
    from[2] = serial.getFloat();
    to[0] = serial.getFloat();
    to[1] = serial.getFloat();
    to[2] = serial.getFloat();
    speed = serial.getFloat();
    entity_id = serial.getInt32();
  }

  void setFromPos(float p[3])
  {
    from[0] = p[0];
    from[1] = p[1];
    from[2] = p[2];
  }
  void setFromPos(float x,float y,float z)
  {
    from[0] = x;
    from[1] = y;
    from[2] = z;
  }
  float* getFromPos()
  {
    return from;
  }

  void setToPos(float p[3])
  {
    to[0] = p[0];
    to[1] = p[1];
    to[2] = p[2];
  }
  void setToPos(float x,float y,float z)
  {
    to[0] = x;
    to[1] = y;
    to[2] = z;
  }
  float* getToPos()
  {
    return to;
  }

  void setSpeed(float s)
  {
    speed = s;
  }
  float getSpeed()
  {
    return speed;
  }

  int getId()
  {
    return entity_id;
  }
  void setId(int id)
  {
    entity_id = id;
  }
};

class MoveEntityToRequestMessage : public NetMessage
{
  float pos[3];

public:
  MoveEntityToRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::MOVE_TO_REQUEST) 
  {
  }

  ~MoveEntityToRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setFloat(pos[0]);
    serial.setFloat(pos[1]);
    serial.setFloat(pos[2]);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    pos[0] = serial.getFloat();
    pos[1] = serial.getFloat();
    pos[2] = serial.getFloat();
  }

  void setPos(float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  void setPos(float x,float y,float z)
  {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
  }
  float* getPos()
  {
    return pos;
  }
};


class CharacterSkillListMessage : public NetMessage
{
  char skillCount;

  class nwSkills
  {
  public:
    short id;
    ptString name;
  };

  nwSkills* skills;

public:
  CharacterSkillListMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::CHAR_SKILL_LIST), skills(0) {}
  ~CharacterSkillListMessage() { delete [] skills; }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt8(skillCount);
    for (int i=0; i<skillCount; i++)
    {
      serial.setInt16(skills[i].id);
      serial.setString(skills[i].name);
    }
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    setSkillCount(serial.getInt8());
    for (int i=0; i<skillCount; i++)
    {
      skills[i].id = serial.getInt16();
      skills[i].name = serial.getString();
    }
  }

  char getSkillCount() { return skillCount; }
  void setSkillCount(char ic) 
  {
    skillCount = ic; 
    delete [] skills; 
    skills = new nwSkills[ic];
  }

  int getSkillId(short idx) { return skills[idx].id; }
  void setSkillId(size_t idx, short skill_id) { skills[idx].id = skill_id; }

  void setName(int idx, ptString name)
  {
    skills[idx].name = name;
  }
  ptString& getName(int idx)
  {
    return skills[idx].name;
  }
};

class EquipRequestMessage : public NetMessage
{
  int itemId;
  int slotId;

public:
  EquipRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::EQUIP_REQUEST) 
  {
  }

  ~EquipRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(itemId);
    serial.setInt32(slotId);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    itemId = serial.getInt32();
    slotId = serial.getInt32();
  }

  void setItemID(int i) { itemId = i; }
  int getItemID() { return itemId; }

  void setSlotID(int s) { slotId = s; }
  int getSlotID() { return slotId; }
};

class EquipMessage : public NetMessage
{
  int entityId;
  int itemId;
  int slotId;

  ptString error;

public:
  EquipMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::EQUIP) 
  {
  }

  ~EquipMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(entityId);
    serial.setInt32(itemId);
    serial.setInt32(slotId);
    serial.setString(error);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    entityId = serial.getInt32();
    itemId = serial.getInt32();
    slotId = serial.getInt32();
    error = serial.getString();
  }

  void setEntityID(int e) { entityId = e; }
  int getEntityID() { return entityId; }

  void setItemID(int i) { itemId = i; }
  int getItemID() { return itemId; }

  void setSlotID(int s) { slotId = s; }
  int getSlotID() { return slotId; }

  void setError(ptString error)
  {
    this->error = error;
  }
  ptString& getError()
  {
    return error;
  }
};

class OpenDoorRequestMessage : public NetMessage
{
  int doorID;

public:
  OpenDoorRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::OPEN_DOOR_REQUEST) 
  {
  }

  ~OpenDoorRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(doorID);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    doorID = serial.getInt32();
  }
  void setDoorId(int doorid)
  {
    this->doorID = doorid;
  }
  int getDoorId()
  {
    return doorID;
  }

};
class OpenDoorResponseMessage : public NetMessage
{
  int doorID;
  ptString error;
public:
  OpenDoorResponseMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::OPEN_DOOR_RESPONSE) 
  {
  }

  ~OpenDoorResponseMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(doorID);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    doorID = serial.getInt32();
  }

  void setDoorId(int doorid)
  {
    this->doorID = doorid;
  }
  int getDoorId()
  {
    return doorID;
  }
  void setError(ptString error)
  {
    this->error = error;
  }
  ptString& getError()
  {
    return error;
  }

};

class CloseDoorRequestMessage : public NetMessage
{
  int doorID;

public:
  CloseDoorRequestMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::CLOSE_DOOR_REQUEST) 
  {
  }

  ~CloseDoorRequestMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(doorID);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    doorID = serial.getInt32();
  }
  void setDoorId(int doorid)
  {
    this->doorID = doorid;
  }
  int getDoorId()
  {
    return doorID;
  }
};
class CloseDoorResponseMessage : public NetMessage
{
  int doorID;
  ptString error;

public:
  CloseDoorResponseMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::CLOSE_DOOR_RESPONSE) 
  {
  }

  ~CloseDoorResponseMessage()
  {
  }

  void serialise(ByteStream* bs)
  {
    Serialiser serial(bs);
    serial.setInt8(type);
    serial.setInt8(id);
    serial.setInt32(doorID);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    doorID = serial.getInt32();
  }
  void setDoorId(int doorid)
  {
    this->doorID = doorid;
  }
  int getDoorId()
  {
    return doorID;
  }
  void setError(ptString error)
  {
    this->error = error;
  }
  ptString& getError()
  {
    return error;
  }

};

#endif // _ENTITIYMESSAGES_H_
