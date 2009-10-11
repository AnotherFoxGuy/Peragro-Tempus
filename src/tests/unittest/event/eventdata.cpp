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

#include <algorithm>
#include <sstream>

#include <boost/test/unit_test.hpp>

#include "common/event/eventdata.h"

BOOST_AUTO_TEST_SUITE(Event_System)

using namespace PT;
using namespace PT::Event;
using PT::Entity::EntityId;

struct EventData1Fixture
{
  EventData1Fixture() : event1Id("event.one"), invalidEntityId(),
    event1(event1Id) {}
  const EventId event1Id;
  const EntityId invalidEntityId;
  EventData event1;
};

struct EventData2Fixture
{
  EventData2Fixture() : event2Id("event.two"), entity40Id(40),
    event2(event2Id, entity40Id) {}
  const EventId event2Id;
  const EntityId entity40Id;
  EventData event2;
};

struct EventDataAttributesFixture
{
  EventDataAttributesFixture() : length(67), hasFailed(true),
    lengthId("Length"), hasFailedId("HasFailed") {}
  const int32_t length;
  const bool hasFailed;
  const EventAttrId lengthId;
  const EventAttrId hasFailedId;
};

struct EventData2AttributesFixture : EventData2Fixture, EventDataAttributesFixture
{
  EventData2AttributesFixture()
  {
    event2.Add<int32_t>(lengthId, length);
    event2.Add<bool>(hasFailedId, hasFailed);
  }
};

struct EventDataConstructFixture : EventData1Fixture, EventData2Fixture {};
BOOST_FIXTURE_TEST_CASE(Event_Data_Construct, EventDataConstructFixture)
{
  BOOST_CHECK_EQUAL(event1.GetId(), event1Id);
  BOOST_CHECK_EQUAL(event1.GetEntityId(), invalidEntityId);
  BOOST_CHECK_EQUAL(event1.IsEntitySpecific(), false);

  BOOST_CHECK_EQUAL(event2.GetId(), event2Id);
  BOOST_CHECK_EQUAL(event2.GetEntityId(), entity40Id);
  BOOST_CHECK(event2.IsEntitySpecific());
}

BOOST_FIXTURE_TEST_CASE(Event_Data_Copy_Construct, EventData2AttributesFixture)
{
  EventData event2Copy(event2);
  BOOST_CHECK_EQUAL(event2.GetId(), event2Copy.GetId());
  BOOST_CHECK_EQUAL(event2.GetEntityId(), event2Copy.GetEntityId());
  BOOST_CHECK_EQUAL(event2.IsEntitySpecific(), event2Copy.IsEntitySpecific());

  std::stringstream event2Stream;
  event2Stream << event2;
  std::stringstream event2CopyStream;
  event2CopyStream << event2Copy;
  BOOST_CHECK(!event2Stream.str().empty());
  BOOST_CHECK(!event2CopyStream.str().empty());
  BOOST_CHECK_EQUAL(event2Stream.str(), event2CopyStream.str());
}

struct EventDataAddAttributesFixture : EventData1Fixture, EventDataAttributesFixture {};
BOOST_FIXTURE_TEST_CASE(Event_Data_Add_Attributes, EventDataAddAttributesFixture)
{
  BOOST_CHECK(event1.Add<int32_t>(lengthId, length));
  BOOST_CHECK(event1.Add<int32_t>(lengthId, length) == false);
  BOOST_CHECK(event1.Add<bool>(hasFailedId, hasFailed));
}

BOOST_FIXTURE_TEST_CASE(Event_Data_Get_Attributes, EventData2AttributesFixture)
{
  int32_t lengthResult = 0;
  BOOST_CHECK_THROW(lengthResult = event2.Get<int32_t>("Langth"),
    AttributeNotFound);
  BOOST_CHECK_THROW(lengthResult = event2.Get<float>(lengthId),
    AttributeTypeMismatch);
  BOOST_CHECK_NO_THROW(lengthResult = event2.Get<int>(lengthId));
  BOOST_CHECK_EQUAL(lengthResult, length);
  BOOST_CHECK_EQUAL(event2.Get<bool>(hasFailedId), hasFailed);

  boost::optional<int32_t> lengthResult2 = event2.TryGet<int32_t>("Langth");
  BOOST_CHECK(!lengthResult2.is_initialized());
  lengthResult2 = event2.TryGet<int32_t>(lengthId);
  BOOST_CHECK(lengthResult2.is_initialized());
}

BOOST_FIXTURE_TEST_CASE(Event_Data_Ostream, EventData2AttributesFixture)
{
  std::stringstream stream;
  stream << event2;
  const char* const output1 = "event.two EntityId:40 Length:67 HasFailed:true";
  const char* const output2 = "event.two EntityId:40 HasFailed:true Length:67";
  BOOST_CHECK_MESSAGE(stream.str() == output1 || stream.str() == output2,
    "check stream.str() == \"" << output1 << "\" || stream.str() == \"" <<
    output2 << "\" failed [\"" << stream.str() << "\"]");
}

BOOST_AUTO_TEST_SUITE_END()
