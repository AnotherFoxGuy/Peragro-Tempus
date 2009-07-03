/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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
#include <stdio.h>

#include "common/util/sleep.h"
#include "common/util/timer.h"
#include "common/util/timerengine.h"

int counter = 100;

class TestTimer : public Timer
{
protected:
  void timeOut()
  {
    counter--;
    printf("Counter: %i (%d)\n", counter, (int)time(0));
  }
};

class JamTimer : public Timer
{
protected:
  void timeOut()
  {
    printf("Start delay ");
    int tmp = (rand() & 0x3) + 1;
    for (int i = 0; i < tmp * 10 ; i++)
    {
      pt_sleep(100);
      printf(".");
    }
    printf("done!\n");
  }
};

int main(int argc, char ** argv)
{
  printf("Start\n");

  PTTime time;
  time.Initialize();

  for (int i = 0; i < 10; i++)
  {
    printf("Sleeping... \t");
    pt_sleep(1000);
    printf("slept for %d ms\n", time.GetElapsedMS());
  }

  TimerEngine timerEngine;
  timerEngine.Begin();

  TestTimer* timer = new TestTimer();
  timer->setInterval(10); // interval in 100ms => 10 = 1 second.
  timer->start();

  JamTimer* jam = new JamTimer();
  jam->setInterval(50); // interval in 100ms => 50 = 5 seconds.
  jam->start();

  while (counter > 0) pt_sleep(1000);

  timer->stop();
  jam->stop();

  delete timer;
  delete jam;

  timerEngine.Kill();
  printf("End\n");
}
