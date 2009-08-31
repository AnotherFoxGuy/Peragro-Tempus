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

#ifdef NDEBUG
#undef NDEBUG
#endif
#ifndef DEBUG
#define DEBUG
#endif

#include <string>
#include <iostream>
#include <sstream>
#include <istream>

//#include <wfmath/ball.h>

#include <boost/thread.hpp>

#include "common/entity/entitymanager.h"
#include "common/entity/entity.h"

using namespace Common::Entity;
using namespace WFMath;

typedef boost::lock_guard<boost::mutex> LockType;
boost::mutex ioMutex; // The iostreams are not guaranteed to be thread-safe!

class Test
{
public:
  Test(EntityManager* e, size_t NR, size_t offset = 0)
    : entityManager(e), NR(NR), offset(offset), firstRun(true) {}

  void operator()()
  {
    {
      LockType lock(ioMutex);
      std::cout << "Thread:" << offset << std::endl;
    }
    while (true)
    {
      Run();
      boost::this_thread::interruption_point();
      boost::this_thread::yield();
    }
  }

  void Run()
  {
    std::list<Entityp> entities;
    for (size_t i = offset; i < NR+offset; i++)
    {
      std::stringstream name; name << "Entity_" << i;
      Entityp e(new Entity(Common::Entity::PCEntityType));
      e->SetId(i);
      e->SetName(name.str());
      entities.push_back(e);
    }

    if (firstRun)
    {
      // Add entities.
      std::list<Entityp>::const_iterator it;
      for (it = entities.begin(); it != entities.end(); it++)
      {
        assert(entityManager->Add(*it));
      }
    }

    {
      // Add double entities.
      std::list<Entityp>::const_iterator it;
      for (it = entities.begin(); it != entities.end(); it++)
      {
        assert(!entityManager->Add(*it));
      }
    }

    {
      // Move entities.
      std::list<Entityp>::const_iterator it;
      for (it = entities.begin(); it != entities.end(); it++)
      {
        (*it)->SetPosition(Point<3>(0));
      }
    }

    {
      // Move all entities.
      EntitiesSafe ents = entityManager->GetEntitiesSafe();
      Entities::const_iterator it;
      for (it = ents.get().begin(); it != ents.get().end(); it++)
      {
        for (size_t i = 0; i < NR; i++)
        {
          it->second->SetPosition(Point<3>(0));
        }
      }
    }

    Ball<3> ball(Point<3>(0), 100.0f);

    {
      // Query.
      std::list<Entityp> result;
      result = entityManager->Query(ball);
      //LockType lock(ioMutex);
      //std::cout << "Normal query: Found " << result.size()
      //  << " out of " << entities.size() << "." << std::endl;
    }

    firstRun = false;
  }

private:
  EntityManager* const entityManager;
  const size_t NR;
  const size_t offset;
  bool firstRun;
};

int main()
{
  std::cout << "Testing EntityManager:" << std::endl;
  EntityManager entityManager;

  const size_t NR = 1000;
  const size_t TNR = 10;
  std::vector<boost::thread*> threads;
  for (size_t i = 0; i < TNR; i++)
  {
    threads.push_back(new boost::thread(Test(&entityManager, NR/TNR, (NR/TNR)*i)));
  }

  for (size_t i = 0; i < TNR; i++)
  {
    threads[i]->interrupt();
    threads[i]->join();
    delete threads[i];
  }

  assert(NR == entityManager.GetCount());

  return 0;
}

