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

  TimerEngine timerEngine;
  timerEngine.Begin();

  TestTimer* timer = new TestTimer();
  timer->setInterval(100);
  timer->start();

  JamTimer* jam = new JamTimer();
  jam->setInterval(500);
  jam->start();

  while (counter > 0) pt_sleep(1000);
  
  timer->stop();
  jam->stop();

  delete timer;
  delete jam;

  timerEngine.Kill();
  printf("End\n");
}
