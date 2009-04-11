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

#include "common/entity/entitymanager.h"
#include "common/entity/entity.h"

#include "common/util/thread.h"
#include "common/util/sleep.h"

using namespace Common::Entity;
using namespace WFMath;

boost::mutex io_mutex; // The iostreams are not guaranteed to be thread-safe!

class Test : public Thread
{
  EntityManager* entityManager;
  size_t NR;
  size_t offset;

  bool firstRun;

public:
  Test(EntityManager* e, size_t NR, size_t offset=0) 
    : entityManager(e), NR(NR), offset(offset) { firstRun = true; Begin(); }
  ~Test() { }

  bool HadfirstRun() { return !firstRun; }

  virtual void Run()
  {
    if (firstRun)
    {
      ptScopedMutex p(io_mutex);
      std::cout << "Thread:" << offset << std::endl;
    }

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
      assert(ents.first.owns_lock());
      Entities::const_iterator it;
      for (size_t i = 0; i < NR; i++)
      {
        for (it = ents.second.begin(); it != ents.second.end(); it++)
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
      ptScopedMutex p(io_mutex);
      //std::cout << "Normal query: Found " << result.size() 
      //  << " out of " << entities.size() << "." << std::endl;
    }

    firstRun = false;
    this->Yield();
  }
};

int main()
{
  std::cout << "Testing EntityManager:" << std::endl;
  EntityManager* entityManager = new EntityManager();

  const size_t NR = 1000;

  const size_t TNR = 10;
  std::vector<Test*> threads;
  for (size_t i = 0; i < TNR; i++)
  {
    threads.push_back(new Test(entityManager, NR/TNR, (NR/TNR)*i));
  }

  bool running = true;
  while (running)
  {
    pt_sleep(100);
    running = false;
    for (size_t i = 0; i < TNR; i++)
      if(!threads[i]->HadfirstRun()) {running = true; break; }
  }

  for (size_t i = 0; i < TNR; i++)
  {
    threads[i]->Kill();
    delete threads[i];
  }

  assert(NR == entityManager->GetCount());

  delete entityManager;

  return 0;
}