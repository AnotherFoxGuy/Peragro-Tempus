/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef PTENTITY_H
#define PTENTITY_H

#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"
#include "csutil/ref.h"
#include "csutil/csstring.h"
#include "physicallayer/entity.h"

class PtEntity 
{
public:
  enum EntityType
  {
    PlayerEntity=0,
    NPCEntity=1,
    DoorEntity=2,
    ItemEntity=3
  };

private:
  int id;
  EntityType type;
  csString name;
  iCelEntity* celentity;

public:
  PtEntity(EntityType type) : id(-1), type(type) {}
  virtual ~PtEntity() {}

  int GetId () const { return id; }
  int GetType () const { return type; }
  csString GetName () const { return name; }
  iCelEntity* GetCelEntity () const { return celentity; }

  void SetId (int value) { this->id = value; }
  void SetType (EntityType value)  { this->type = value; }
  void SetName (csString value) { this->name = value; }
  void SetCelEntity (iCelEntity* value) { this->celentity = value; }

  virtual void Create();
  virtual void Destroy();

};

#endif // PTENTITY_H
