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

#include "ai.h"

#include "idle-ai.h"
#include "stray-ai.h"
#include "guard-ai.h"

ptString AI::idle("idle", 4);
ptString AI::stray("stray", 5);
ptString AI::guard("guard", 5);

AI* AI::createAI(ptString ai_name)
{
  if (ai_name == idle)
  {
    return new IdleAI();
  }
  else if (ai_name == stray)
  {
    return new StrayAI();
  }
  else if (ai_name == guard)
  {
    return new GuardAI();
  }
  return 0;
}
