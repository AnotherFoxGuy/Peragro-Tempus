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

#ifndef DATA_DOOR_H
#define DATA_DOOR_H


#include <cssysdef.h>

#include <iutil/string.h>
#include <iutil/vfs.h>
#include <csutil/ref.h>
#include <csutil/csstring.h>

namespace PT
{
  namespace Data
  {
    class Door
    {
    private:
      unsigned int id;
      csString name;
      csString meshname;
      csVector3 position;
      csString sectorname;
      unsigned int keyId;

    public:
      Door() {}
      ~Door() {}
      unsigned int GetId() { return id; }
      csString GetName() { return name; }
      csString GetMeshName() { return meshname;}
      csVector3 GetPosition() { return position;}
      csString GetSectorName() { return sectorname;}
      unsigned int GetKeyId() { return keyId;} 

      void SetId(unsigned int value) { this->id = value; }
      void SetName(csString value) { this->name = value; }
      void SetMeshName(csString value) { this->meshname = value; }
      void SetPosition(csVector3 value) { this->position = value; }
      void SetSectorName(csString value) { this->sectorname = value; }
      void SetKeyId(unsigned int value) { this->keyId = value; }
    };

  } // Data namespace 
} // PT namespace 

#endif // DATA_DOOR_H
