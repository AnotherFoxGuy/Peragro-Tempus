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

#include "client/entity/ptcelentity.h"


ptCelEntity::ptCelEntity (int id, int type, csString name, iCelEntity* celentity)
{
  ptCelEntity::id = id;
  ptCelEntity::type = type;
  ptCelEntity::name = name;
  ptCelEntity::celentity = celentity;
  //ptCelEntity::pclinmove = pclinmove;

  // Stats
  ptCelEntity::hp = 50;
  ptCelEntity::mp = 10;
  ptCelEntity::totalhp = 50;
  ptCelEntity::totalmp = 50;
}

ptCelEntity::~ptCelEntity()
{
}

