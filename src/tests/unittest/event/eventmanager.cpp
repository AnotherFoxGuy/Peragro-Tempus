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

#include <boost/test/unit_test.hpp>

#include "common/thread/threadloop.h"

#include "common/event/eventdata.h"
#include "common/event/eventmanager.h"

BOOST_AUTO_TEST_SUITE(Event_System)

using namespace PT::Event;
using PT::Entity::EntityId;

struct Receiver;
typedef boost::shared_ptr<Receiver> ReceiverPtr;

struct Receiver
{
public:
  static ReceiverPtr Create(const EntityId& id, iEventManager* evMgr)
  {
    ReceiverPtr sp(new Receiver);
    Receiver* p = sp.get();
    evMgr->AddHandler("event1", Slot(&Receiver::Event1, p, _1).track(sp));
    evMgr->AddHandler("event2", id, Slot(&Receiver::Event2, p, _1).track(sp));
    return sp;
  }

#define PT_RECIEVER_HANDLER(n) \
  bool Event##n(const EventData& ev) \
  { \
    Lock lock(mutex); \
    event##n##Received = true; \
    eventReceived.notify_all(); \
    return true; \
  }
  PT_RECIEVER_HANDLER(1)
  PT_RECIEVER_HANDLER(2)

#define PT_RECIEVER_CHECK(n) \
  bool WaitEvent##n##Received() \
  { \
    Lock lock(mutex); \
    if (!event##n##Received) \
     eventReceived.timed_wait(lock, boost::posix_time::seconds(1)); \
    const bool retVal = event##n##Received; \
    event##n##Received = false; \
    return retVal; \
  } \
  bool CheckEvent##n##Received() \
  { \
    Lock lock(mutex); \
    return event##n##Received; \
  }
  PT_RECIEVER_CHECK(1)
  PT_RECIEVER_CHECK(2)

private:
  Receiver() : event1Received(false), event2Received(false) {}

  typedef boost::unique_lock<boost::mutex> Lock;
  boost::mutex mutex;
  boost::condition_variable eventReceived;
  bool event1Received;
  bool event2Received;
};

struct EventManagerFixture
{
  EventManagerFixture()
    : eventManager(&iEventManager::SendEvents, new EventManager) {}
  PT::Thread::ThreadLoop<iEventManager> eventManager;
};

struct EventManagerReceiver1Fixture : virtual EventManagerFixture
{
  EventManagerReceiver1Fixture() : receiver1Id(38),
    receiver1(Receiver::Create(receiver1Id, eventManager.Get())) {}
  EntityId receiver1Id;
  ReceiverPtr receiver1;
};

struct EventManagerReceiver2Fixture : virtual EventManagerFixture
{
  EventManagerReceiver2Fixture() : receiver2Id(44),
    receiver2(Receiver::Create(receiver2Id, eventManager.Get())) {}
  EntityId receiver2Id;
  ReceiverPtr receiver2;
};

struct EventManagerData1Fixture
{
  EventManagerData1Fixture() : event1Id("event1"),
    event1(new EventData(event1Id)) {}
  EventId event1Id;
  EventDataPtr event1;
};

struct EventManagerData2Fixture
{
  EventManagerData2Fixture() : event2Id("event2"), event2EntityId(38),
    event2(new EventData(event2Id, event2EntityId)) {}
  EventId event2Id;
  EntityId event2EntityId;
  EventDataPtr event2;
};

struct EventManagerPostFixture : EventManagerReceiver1Fixture,
  EventManagerData1Fixture {};
BOOST_FIXTURE_TEST_CASE(Event_Manager_Post, EventManagerPostFixture)
{
  eventManager.Get()->Post(event1);
  BOOST_CHECK(receiver1->WaitEvent1Received());
}

struct EventManagerPost2Fixture : EventManagerReceiver1Fixture,
  EventManagerData1Fixture, EventManagerData2Fixture {};
BOOST_FIXTURE_TEST_CASE(Event_Manager_Post2, EventManagerPost2Fixture)
{
  eventManager.Get()->Post(event1);
  BOOST_CHECK(receiver1->WaitEvent1Received());
  BOOST_CHECK(!receiver1->CheckEvent1Received());
  eventManager.Get()->Post(event2);
  BOOST_CHECK(receiver1->WaitEvent2Received());
}

struct EventManagerReceive2Fixture : EventManagerReceiver1Fixture,
  EventManagerReceiver2Fixture, EventManagerData1Fixture,
  EventManagerData2Fixture {};
BOOST_FIXTURE_TEST_CASE(Event_Manager_Receive2, EventManagerReceive2Fixture)
{
  eventManager.Get()->Post(event2);
  BOOST_CHECK(receiver1->WaitEvent2Received());
  BOOST_CHECK(!receiver2->CheckEvent2Received());
  eventManager.Get()->Post(event1);
  BOOST_CHECK(receiver1->WaitEvent1Received());
  BOOST_CHECK(receiver2->WaitEvent1Received());
}

BOOST_AUTO_TEST_SUITE_END()
