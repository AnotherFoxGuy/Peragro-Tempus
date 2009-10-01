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

#include "common/utility/stringid.h"

BOOST_AUTO_TEST_SUITE(String_Id)

struct TestTag {};
typedef PT::StringId<TestTag> TestStringId;

struct StringIdFixture
{
  StringIdFixture() : testCString("test string"), testString(testCString),
    testString2("test string 2"), testStringId1(), testStringId2(testString),
    testStringId3(testCString), testStringId4(testStringId2),
    testStringId5(testString2) {}
  const char* const testCString;
  const std::string testString;
  const std::string testString2;
  TestStringId testStringId1;
  TestStringId testStringId2;
  TestStringId testStringId3;
  TestStringId testStringId4;
  TestStringId testStringId5;
};

BOOST_FIXTURE_TEST_CASE(String_Id_Construct, StringIdFixture)
{
  BOOST_CHECK_EQUAL(testStringId1.get(), std::string());
  BOOST_CHECK_EQUAL(testStringId2.get(), testString);
  BOOST_CHECK_EQUAL(testStringId3.get(), testString);
  BOOST_CHECK_EQUAL(testStringId4.get(), testString);
}

BOOST_FIXTURE_TEST_CASE(String_Id_Operators, StringIdFixture)
{
  BOOST_CHECK_NE(testStringId1, testStringId2);
  BOOST_CHECK_EQUAL(testStringId2, testStringId3);
  BOOST_CHECK_EQUAL(testStringId2, testStringId4);
  BOOST_CHECK(testStringId2 < testStringId5 || testStringId5 < testStringId2);
  BOOST_CHECK(testStringId2 > testStringId5 || testStringId5 > testStringId2);
}

BOOST_FIXTURE_TEST_CASE(String_Id_String_Compatibility, StringIdFixture)
{
  BOOST_CHECK(testStringId1.empty());
  BOOST_CHECK(!testStringId2.empty());
  BOOST_CHECK_EQUAL(testStringId2.c_str(), testCString);
  BOOST_CHECK_EQUAL(testStringId3.c_str(), testCString);
  BOOST_CHECK_EQUAL(testStringId4.c_str(), testCString);
}

BOOST_FIXTURE_TEST_CASE(String_Id_Hash, StringIdFixture)
{
  BOOST_CHECK_NE(hash_value(testStringId2), hash_value(testStringId1));
  BOOST_CHECK_EQUAL(hash_value(testStringId2), hash_value(testStringId3));
  BOOST_CHECK_EQUAL(hash_value(testStringId2), hash_value(testStringId4));
  BOOST_CHECK_NE(hash_value(testStringId2), hash_value(testStringId5));
}

BOOST_AUTO_TEST_SUITE_END()
