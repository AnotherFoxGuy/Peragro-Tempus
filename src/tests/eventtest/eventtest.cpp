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

#include "common/event/eventmanager.h"
#include "common/event/eventdata.h"

#include <boost/enable_shared_from_this.hpp>
#include <sstream>

#include "common/thread/threadloop.h"
#include "common/thread/multithread.h"

boost::mutex ioMutex;
typedef boost::lock_guard<boost::mutex> Lock;
#define DEBUG_PRINT(s) do { Lock l(ioMutex); cout << s << endl; } while (false)
//#define DEBUG_PRINT(s)

using namespace std;
using namespace PT;
using namespace PT::Event;
using PT::Entity::EntityId;

class Handler;
typedef boost::shared_ptr<Handler> HandlerPtr;

iEventManager* evMgr;

const size_t N_SENDERS = 3;
const size_t N_HANDLERS = 10;
const size_t N_EVENT_THREADS = 3;
const size_t N_LOOPS = 100;

class Handler
{
public:
  static HandlerPtr Create(const EntityId& eId)
  {
    DEBUG_PRINT("Handler " << eId);
    HandlerPtr spThis(new Handler(eId));
    Handler* pThis = spThis.get();
    evMgr->AddHandler(event1Id, Slot(&Handler::Event1, pThis, _1).track(spThis));
    evMgr->AddHandler(event2Id, pThis->id, Slot(&Handler::Event2, pThis, _1).track(spThis));
    evMgr->AddHandler(event3Id, pThis->id, Slot(&Handler::Event3, pThis, _1).track(spThis));
    return spThis;
  }

  ~Handler() { DEBUG_PRINT("~Handler " << id); }

  bool Event1(const EventData& ev) const
  {
    DEBUG_PRINT("Event1 " << id);
    return true;
  }

  bool Event2(const EventData& ev)
  {
    boost::lock_guard<boost::mutex> l(m);
    attr1 = ev.Get<int32_t>(attr1Id);
    attr2 = ev.Get<string>(attr2Id);
    DEBUG_PRINT("Event2 " << id << " " << attr1 << " " << attr2);
    return true;
  }

  bool Event3(const EventData& ev)
  {
    int attr3 = ev.Get<int32_t>(attr3Id);
    bool attr4 = ev.Get<bool>(attr4Id);
    float attr5 = ev.Get<float>(attr5Id);
    string attr6 = ev.Get<string>(attr6Id);
    //WFMath::Point<3> attr7 = ev.Get<WFMath::Point<3> >(attr7Id);
    int attr8 = ev.Get<int32_t>(attr8Id);
    bool attr9 = ev.Get<bool>(attr9Id);
    float attr10 = ev.Get<float>(attr10Id);
    boost::lock_guard<boost::mutex> l(m);
    attr1 = ev.Get<int32_t>(attr1Id);
    attr2 = ev.Get<string>(attr2Id);
    DEBUG_PRINT("Event3 " << id << " " << attr1 << " " << attr2 << " "
      << attr3 << " " << attr4 << " " << attr5 << " " << attr6 << " "
      /*<< attr7*/ << " " << attr8 << " " << attr9 << " " << attr10);
    return true;
  }

private:
  Handler(const EntityId& eId) : id(eId) {}

public:
  const EntityId id;

  boost::mutex m;
  int attr1;
  string attr2;

  static const EventId event1Id;
  static const EventId event2Id;
  static const EventId event3Id;
  static const EventAttrId attr1Id;
  static const EventAttrId attr2Id;
  static const EventAttrId attr3Id;
  static const EventAttrId attr4Id;
  static const EventAttrId attr5Id;
  static const EventAttrId attr6Id;
  static const EventAttrId attr7Id;
  static const EventAttrId attr8Id;
  static const EventAttrId attr9Id;
  static const EventAttrId attr10Id;
};

const EventId Handler::event1Id("event1");
const EventId Handler::event2Id("event2");
const EventId Handler::event3Id("event3");
const EventAttrId Handler::attr1Id("attr1");
const EventAttrId Handler::attr2Id("attr2");
const EventAttrId Handler::attr3Id("attr3");
const EventAttrId Handler::attr4Id("attr4");
const EventAttrId Handler::attr5Id("attr5");
const EventAttrId Handler::attr6Id("attr6");
const EventAttrId Handler::attr7Id("attr7");
const EventAttrId Handler::attr8Id("attr8");
const EventAttrId Handler::attr9Id("attr9");
const EventAttrId Handler::attr10Id("attr10");

class Sender
{
public:
  Sender(const string& n) : name(n) { DEBUG_PRINT("Sender " << name); }
  ~Sender() { DEBUG_PRINT("~Sender " << name); }
  void Send()
  {
    for (size_t i = 0; i < N_LOOPS; ++i)
    {
      for (size_t idCounter = 0, attr1Counter = 0; idCounter < N_HANDLERS; ++idCounter, ++attr1Counter)
      {
        EventDataPtr event(new EventData(Handler::event1Id, idCounter));
        evMgr->Post(event);

        event.reset(new EventData(Handler::event2Id, idCounter));
        event->Add<int32_t>(Handler::attr1Id, ++attr1Counter);
        event->Add<string>(Handler::attr2Id, name);
        evMgr->Post(event);

        event.reset(new EventData(Handler::event3Id, idCounter));
        event->Add<int32_t>(Handler::attr1Id, ++attr1Counter);
        event->Add<string>(Handler::attr2Id, name);
        event->Add<int32_t>(Handler::attr3Id, 289);
        event->Add<bool>(Handler::attr4Id, true);
        event->Add<float>(Handler::attr5Id, 13.432f);
        event->Add<string>(Handler::attr6Id, "goat");
        //event->Add<WFMath::Point<3> >(Handler::attr7Id, WFMath::Point<3>(1,2,3));
        event->Add<int32_t>(Handler::attr8Id, 124);
        event->Add<bool>(Handler::attr9Id, false);
        event->Add<float>(Handler::attr10Id, 54.23f);
        evMgr->Post(event);

        boost::this_thread::yield();
      }
    }
  }
private:
  string name;
};

struct Timer
{
  static boost::posix_time::ptime GetTime() { return boost::posix_time::microsec_clock::local_time(); }
  Timer() : init(GetTime()) {}
  size_t Elapsed() const { return (GetTime() - init).total_microseconds(); }
  void Reset() { init = GetTime(); }
  boost::posix_time::ptime init;
};

int main()
{
  Thread::ThreadLoop<iEventManager, Thread::OwnedStorage, Thread::MultiThread>
    eventThread(&iEventManager::SendEvents);
  eventThread.Set(new EventManager);
  evMgr = eventThread.Get();
  eventThread.Start(N_EVENT_THREADS);

  vector<HandlerPtr> handlers(N_HANDLERS);
  for (size_t id = 0; id < N_HANDLERS; ++id)
  {
    handlers.at(id) = Handler::Create(id);
  }

  vector<boost::thread*> senderThreads(N_SENDERS);
  vector<Sender*> senders(N_SENDERS);
  Timer timer;
  for (size_t i = 0; i < N_SENDERS; ++i)
  {
    stringstream name;
    name << "sender ";
    name << i;
    Sender* sender = new Sender(name.str());
    senderThreads.at(i) = new boost::thread(boost::bind(&Sender::Send, sender));
    senders.at(i) = sender;
  }

  for (size_t i = 0; i < N_SENDERS; ++i)
  {
    senderThreads.at(i)->join();
    delete senderThreads.at(i);
    delete senders.at(i);
  }

  const size_t sentTime = timer.Elapsed();

  eventThread.Stop();

  const size_t recievedTime = timer.Elapsed();
  cout << "Posted " << N_SENDERS * N_LOOPS * 3 << " events in " << sentTime
    << " microseconds, finished handling in " << recievedTime
    << " microseconds" << endl;

  return 0;
}

