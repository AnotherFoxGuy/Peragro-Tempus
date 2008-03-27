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

#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "common/util/timer.h"
#include "common/util/sleep.h"

class Environment : public Timer
{
private:
  unsigned char daytime;

  // Timer implementation
  void timeOut()
  {
    broadcastTime();
  }

  void broadcastTime();

public:
  Environment()
  {
    daytime = 0;
    this->setInterval(12); // 12 sec * 24 = 4 min 48 sec
    this->start();
  }
};

#endif // ENVIRONMENT_H
