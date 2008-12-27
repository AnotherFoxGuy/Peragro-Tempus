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

#ifndef PT_COMMON_ENTITY_H
#define PT_COMMON_ENTITY_H

#include <string>
#include "common/geom/ptvector3.h"

namespace PT
{
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
      protected:
        ///Unique ID of the entity.
        unsigned int id;
        ///Type of the entity.
        EntityType type;
        ///Name of the entity.
        std::string name;
        ///Name of the mesh used for this entity.
        std::string meshName;
        /// The file in which this entity's mesh resides.
        std::string fileName;
        ///Position of the entity in a sector (e.g. x='20', y='30', z='40').
        PtVector3 position;
        ///Name of the sector where the npc resides (e.g. 'room').
        std::string sectorName;
        ///Rotation of the entity.
        float rot;

      public:
        Entity() : id(-1), position(0.0f) {}

        /**
        * Virtual destructor.
        */
        virtual ~Entity() {}

        ///@return Entity's unique ID.
        unsigned int GetId () const { return id; }
        ///Set the entity's unique ID to a given value.
        ///@todo Should we really be allowed to do this?
        void SetId (unsigned int value) { id = value; }

        ///@return Entity's type.
        int GetType () const { return type; }
        ///Set the entity's type to a given value.
        ///@todo Should we really be allowed to do this?
        void SetType (EntityType value)  { type = value; }

        ///@return Entity's name.
        const std::string& GetName () const { return name; }
        ///Set the entity's name to a given value.
        void SetName (const std::string& value) { name = value; }

        ///@return Entity's mesh name.
        const std::string& GetMeshName () const { return meshName; }
        ///Set the entity's mesh name to a given value.
        void SetMeshName (const std::string& value) { meshName = value; }

        ///@return Entity's mesh name.
        const std::string& GetFileName () const { return fileName; }
        ///Set the entity's mesh name to a given value.
        void SetFileName (const std::string& value) { fileName = value; }

        ///@return Position of entity within a sector.
        const PtVector3& GetPosition() const { return position; }
        ///Set the position of entity within a sector to a given value.
        void SetPosition(float x, float y, float z)
        { position = PtVector3(x,y,z); }
        void SetPosition(const PtVector3& value) { position = value;}

        ///@return Name of the sector where entity resides.
        const std::string& GetSectorName() const { return sectorName; }
        ///Set the name of sector where the entity resides.
        void SetSectorName (std::string value) { sectorName = value; }

        ///@return Rotation of entity.
        const float GetRotation() const { return rot; }
        ///Set the rotation of entity to a given value.
        void SetRotation (float value) { rot = value; }

      };
    } // Entity namespace
  } // Common namespace
} // PT namespace

#endif // PT_COMMON_ENTITY_H
