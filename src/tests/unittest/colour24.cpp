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
#include <boost/test/output_test_stream.hpp>

#include "common/utility/colour24.h"

BOOST_AUTO_TEST_SUITE(Colour_24)

using boost::test_tools::output_test_stream;

using PT::Colour24;

BOOST_AUTO_TEST_CASE(Colour_24_Construct)
{
  Colour24 colour1;
  BOOST_CHECK_EQUAL(colour1.Red(), 0);
  BOOST_CHECK_EQUAL(colour1.Green(), 0);
  BOOST_CHECK_EQUAL(colour1.Blue(), 0);

  Colour24 colour2(12, 34, 56);
  BOOST_CHECK_EQUAL(colour2.Red(), 12);
  BOOST_CHECK_EQUAL(colour2.Green(), 34);
  BOOST_CHECK_EQUAL(colour2.Blue(), 56);
  BOOST_CHECK_EQUAL(colour2.red, static_cast<Colour24::ColourType>(12));
  BOOST_CHECK_EQUAL(colour2.green, static_cast<Colour24::ColourType>(34));
  BOOST_CHECK_EQUAL(colour2.blue, static_cast<Colour24::ColourType>(56));
}

BOOST_AUTO_TEST_CASE(Colour_24_Output_Stream)
{
  output_test_stream output;
  Colour24 colour(12, 34, 56);
  output << colour;
  BOOST_CHECK(!output.is_empty(false));
  BOOST_CHECK(output.is_equal("12,34,56"));
}

BOOST_AUTO_TEST_CASE(Colour_24_Input_Stream)
{
  std::stringstream ss("12,34,56");
  Colour24 colour;
  ss >> colour;
  BOOST_CHECK_EQUAL(colour.red, 12);
  BOOST_CHECK_EQUAL(colour.green, 34);
  BOOST_CHECK_EQUAL(colour.blue, 56);
}

BOOST_AUTO_TEST_SUITE_END()
