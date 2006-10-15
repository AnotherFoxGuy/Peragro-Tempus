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

struct MovementData
{
  int entity_id;
  float walk;
  float turn;
};

struct MoveToData
{
  int entity_id;
  bool walking;
  float dest_angle;
  float walk_speed;
  float turn_speed;
  float elapsed_time;
  float walk_duration;
};

struct UpdatePcPropData
{
  int entity_id;
  csString pcprop;
  celData value;
};

struct DrUpdateData
{
  int entity_id;
  csString sector;
  csVector3 pos, vel, wvel;
  float rot, speed, avel;
  bool on_ground;
};

#endif // PTENTITY_H
