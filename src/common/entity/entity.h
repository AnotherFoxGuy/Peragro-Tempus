/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef COMMON_ENTITY_H
#define COMMON_ENTITY_H

#include <string>

#include "entitymanager.h"

#include <wfmath/point.h>
#include <wfmath/stream.h>

namespace Common
{
  namespace Entity
  {
    /**
    * @ingroup common_entity
    * Enum representing the possible entity types that are handled by the game
    * engine.
    */
    enum EntityType
    {
      PCEntityType      =0,
      NPCEntityType     =1,
      DoorEntityType    =2,
      ItemEntityType    =3,
      MountEntityType   =4,
      PlayerEntityType  =6
    };

    /**
    * @ingroup common_entity
    * Helper class that contains basic information about an entity.
    * Set/Get methods to access the entity properties.
    */
    class Entity
    {
    public:
      ///Name of the entity.
      std::string name; // TODO: Remove after tree debug printfs are removed, uncomment the lower one.
    protected:
      ///Unique ID of the entity.
      unsigned int id;
      ///Type of the entity.
      EntityType type;
      ///Name of the entity.
      //std::string name;
      ///Name of the mesh used for this entity.
      std::string meshName;
      /// The file in which this entity's mesh resides.
      std::string fileName;
      ///Position of the entity in a sector (e.g. x='20', y='30', z='40').
      Octree::Shape position;
      ///Name of the sector where the npc resides (e.g. 'room').
      std::string sectorName;
      ///Rotation of the entity.
      float rotation;

      friend class EntityManager;

    public:
#pragma warning( push)
#pragma warning( disable : 4355 )
      Entity(EntityType type) : id(0), type(type), position(this), rotation(0.0f) {}
#pragma warning( pop )

      /**
      * Virtual destructor.
      */
      virtual ~Entity() { Reset(); }

      ///@return Entity's unique ID.
      virtual unsigned int GetId () const { return id; }
      ///Set the entity's unique ID to a given value.
      ///@todo Should we really be allowed to do this?
      virtual void SetId (unsigned int value) { id = value; }

      ///@return Entity's type.
      virtual int GetType () const { return type; }

      ///@return Entity's name.
      virtual const std::string& GetName () const { return name; }
      ///Set the entity's name to a given value.
      virtual void SetName (const std::string& value) { name = value; }

      ///@return Entity's mesh name.
      virtual const std::string& GetMeshName () const { return meshName; }
      ///Set the entity's mesh name to a given value.
      virtual void SetMeshName (const std::string& value) { meshName = value; }

      ///@return Entity's mesh name.
      virtual const std::string& GetFileName () const { return fileName; }
      ///Set the entity's mesh name to a given value.
      virtual void SetFileName (const std::string& value) { fileName = value; }

      ///@return Position of entity within a sector.
      virtual const WFMath::Point<3>& GetPosition() const { return position.Get(); }
      ///Set the position of entity within a sector to a given value.
      virtual void SetPosition(float x, float y, float z)
      { position = WFMath::Point<3>(x,y,z); }
      virtual void SetPosition(const WFMath::Point<3>& value) { position = value;}

      ///@return Position of entity within a sector.
      virtual std::string GetPositionStr() const { return WFMath::ToString(position.Get()); }

      ///@return Name of the sector where entity resides.
      virtual const std::string& GetSectorName() const { return sectorName; }
      ///Set the name of sector where the entity resides.
      virtual void SetSectorName (const std::string& value) { sectorName = value; }

      ///@return Rotation of entity.
      virtual float GetRotation() const { return rotation; }
      ///Set the rotation of entity to a given value.
      virtual void SetRotation (float value) { rotation = value; }

      ///Ensure that the Entity's position is a valid position in the world
      virtual void SetFullPosition() { SetFullPosition(GetPosition(), GetRotation(), GetSectorName()); }
      ///Set the Entity's position, ensuring that the position is valid
      virtual void SetFullPosition(const WFMath::Point<3>& pos, 
                                   float rot,
                                   const std::string& sector)
      { position = pos;  rotation = rotation;  sectorName = sector; }

      ///Reset the entity do its default state
      virtual void Reset() {}

    };
  } // Entity namespace
} // Common namespace

#endif // COMMON_ENTITY_H
