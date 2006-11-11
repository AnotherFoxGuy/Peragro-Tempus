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

#ifndef MOVEMENT_H
#define MOVEMENT_H

struct EquipData
{
  unsigned int entity_id;
  unsigned int item_id;
  unsigned int slot_id;
};

struct MountData
{
  unsigned int entity_id;
  unsigned int mount_id;
  unsigned char control;
};

struct UnMountData
{
  unsigned int entity_id;
  unsigned int mount_id;
};

struct TeleportData
{
  unsigned int entity_id;
  csVector3 position;
  csString sector;
};

struct MovementData
{
  unsigned int entity_id;
  float walk;
  float turn;
};

struct MoveToData
{
  unsigned int entity_id;
  bool walking;
  float dest_angle;
  float walk_speed;
  float turn_speed;
  float elapsed_time;
  float walk_duration;
};

struct UpdatePcPropData
{
  unsigned int entity_id;
  csString pcprop;
  celData value;
};

struct DrUpdateData
{
  unsigned int entity_id;
  csString sector;
  csVector3 pos;
  float rot;
};

#endif // PTENTITY_H
