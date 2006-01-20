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

#include <string>

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
    DROP_RESPONSE=9
  };
};

class AddEntityMessage : public NetMessage
{
  std::string name;
  std::string mesh;
  float pos[3];
  std::string sector;
  int ent_id;
  char ent_type;

public:
  AddEntityMessage() : NetMessage(MESSAGES::ENTITY,ENTITY::ADD) 
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

  std::string getName() const
  {
    return name;
  }
  void setName(const std::string& n)
  {
    name = n;
  }

  int getId() const
  {
    return ent_id;
  }
  void setId(int i)
  {
    ent_id = i;
  }

  char getType() const
  {
    return ent_type;
  }
  void setType(char t)
  {
    ent_type = t;
  }

  std::string getMesh() const
  {
    return mesh;
  }
  void setMesh(const std::string& m)
  {
    mesh = m;
  }

  const float* getPos() const
  {
    return pos;
  }
  void setPos(const float p[3])
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

  std::string getSector() const
  {
    return sector;
  }
  void setSector(const std::string& s)
  {
    sector = s;
  }
};

class RemoveEntityMessage : public NetMessage
{
  int ent_id;
  char ent_type;
  std::string name;

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

  int getId() const
  {
    return ent_id;
  }
  void setId(int i)
  {
    ent_id = i;
  }

  char getType() const
  {
    return ent_type;
  }
  void setType(char t)
  {
    ent_type = t;
  }

  std::string getName() const
  {
    return name;
  }
  void setName(const std::string& n)
  {
    name = n;
  }
};

class MoveEntityMessage : public NetMessage
{
  float walk;
  float turn;
  std::string name;

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
    serial.setString(name);
  }

  void deserialise(ByteStream* bs)
  {
    Deserialiser serial(bs);
    type = serial.getInt8();
    id = serial.getInt8();
    walk = serial.getFloat();
    turn = serial.getFloat();
    name = serial.getString();
  }

  void setRot(float direction)
  {
    turn = direction;
  }
  float getRot() const
  {
    return turn;
  }

  void setWalk(float direction)
  {
    walk = direction;
  }
  float getWalk() const
  {
    return walk;
  }

  std::string getName() const
  {
    return name;
  }
  void setName(const std::string& n)
  {
    name = n;
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
    walk = direction+1;
  }
  int getWalk() const
  {
    return int(walk)-1;
  }

  void setRot(int direction)
  {
    turn = direction+1;
  }
  int getRot() const
  {
    return int(turn)-1;
  }
};

class UpdateDREntityRequestMessage : public NetMessage
{
  bool on_ground;
  float speed, rot, avel;
  float pos[3], vel[3], wvel[3];
  std::string sector;

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

  void setOnGround(char state)
  {
    on_ground = state != 0;
  }
  char getOnGround() const
  {
    return on_ground;
  }

  void setSpeed(float s)
  {
    speed = s;
  }
  float getSpeed() const
  {
    return speed;
  }

  void setRot(float r)
  {
    rot = r;
  }
  float getRot() const
  {
    return rot;
  }

  void setAVel(float v)
  {
    avel = v;
  }
  float getAVel() const
  {
    return avel;
  }

  void setPos(const float p[3])
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
  const float* getPos() const
  {
    return pos;
  }

  void setVel(const float p[3])
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
  const float* getVel() const
  {
    return vel;
  }

  void setWVel(const float p[3])
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
  const float* getWVel() const
  {
    return wvel;
  }

  void setSector(const std::string& s)
  {
    sector = s;
  }
  std::string getSector() const
  {
    return sector;
  }
};

class UpdateDREntityMessage : public NetMessage
{
  std::string name;
  bool on_ground;
  float speed, rot, avel;
  float pos[3], vel[3], wvel[3];
  std::string sector;

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
    serial.setString(name);
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
    name = serial.getString();
    sector = serial.getString();
  }

  std::string getName() const
  {
    return name;
  }
  void setName(const std::string& n)
  {
    name = n;
  }

  void setOnGround(char state)
  {
    on_ground = state != 0;
  }
  char getOnGround() const
  {
    return on_ground;
  }

  void setSpeed(float s)
  {
    speed = s;
  }
  float getSpeed() const
  {
    return speed;
  }

  void setRot(float r)
  {
    rot = r;
  }
  float getRot() const
  {
    return rot;
  }

  void setAVel(float v)
  {
    avel = v;
  }
  float getAVel() const
  {
    return avel;
  }

  void setPos(const float p[3])
  {
    pos[0] = p[0];
    pos[1] = p[1];
    pos[2] = p[2];
  }
  const float* getPos() const
  {
    return pos;
  }

  void setVel(const float p[3])
  {
    vel[0] = p[0];
    vel[1] = p[1];
    vel[2] = p[2];
  }
  const float* getVel() const
  {
    return vel;
  }

  void setWVel(const float p[3])
  {
    wvel[0] = p[0];
    wvel[1] = p[1];
    wvel[2] = p[2];
  }
  const float* getWVel() const
  {
    return wvel;
  }

  void setSector(const std::string& s)
  {
    sector = s;
  }
  std::string getSector() const
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
  int getTargetId() const
  {
    return target;
  }

};

class PickEntityResponseMessage : public NetMessage
{
  std::string name;
  std::string target;
  std::string error;

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

  void setName(const std::string& name)
  {
    this->name = name;
  }
  std::string getName() const
  {
    return name;
  }

  void setTarget(const std::string& target)
  {
    this->target = target;
  }
  std::string getTarget() const
  {
    return target;
  }

  void setError(const std::string& error)
  {
    this->error = error;
  }
  std::string getError() const
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
  int getTargetId() const
  {
    return target;
  }

};

class DropEntityResponseMessage : public NetMessage
{
  std::string name;
  std::string target;
  std::string error;

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

  void setName(const std::string& name)
  {
    this->name = name;
  }
  std::string getName() const
  {
    return name;
  }

  void setTarget(const std::string& target)
  {
    this->target = target;
  }
  std::string getTarget() const
  {
    return target;
  }

  void setError(const std::string& error)
  {
    this->error = error;
  }
  std::string getError() const
  {
    return error;
  }
};

#endif // _ENTITIYMESSAGES_H_
