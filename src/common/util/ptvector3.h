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

#ifndef PTVECTOR3_H
#define PTVECTOR3_H

/**
  Simple convenience class representing 3D vector. Access the coordinates
  directly via members called x, y, and z.
  @author Branko Majic <branko.majic@gmail.com>
*/

class PtVector3
{
public:
  float x,y,z;

  PtVector3(float xv=0, float yv=0, float zv=0) : x(xv), y(yv), z(zv) {}

  bool operator==(const PtVector3 v);

};

#endif
