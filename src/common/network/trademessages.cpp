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

#include "common/network/trademessages.h"
#include "deserialiser.h"
#include "serialiser.h"

void TradeRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt32(entityid);
}

void TradeRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  entityid = (unsigned int) serial.getInt32();
}

void TradeResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(error);
}

void TradeResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  error = serial.getString();
}

void TradeOffersListNpcMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(offerscount);
  for ( size_t i = 0; i < offerscount ; i++ )
  {
    serial.setInt32(offers[i].itemid);
    serial.setInt32(offers[i].price);
  };

}

void TradeOffersListNpcMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  offerscount = (unsigned char) serial.getInt8();
  setOffersCount(offerscount);
  for ( size_t i = 0; i < offerscount ; i++ )
  {
    offers[i].itemid = (unsigned int) serial.getInt32();
    offers[i].price = (unsigned int) serial.getInt32();
  };

}

void BuyItemRequestNpcMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
}

void BuyItemRequestNpcMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

void BuyItemResponseNpcMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(error);
}

void BuyItemResponseNpcMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  error = serial.getString();
}

void TradeOffersListPvpMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setInt8(offerscount);
  for ( size_t i = 0; i < offerscount ; i++ )
  {
    serial.setInt32(offers[i].itemid);
    serial.setInt32(offers[i].amount);
  };

}

void TradeOffersListPvpMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  offerscount = (unsigned char) serial.getInt8();
  setOffersCount(offerscount);
  for ( size_t i = 0; i < offerscount ; i++ )
  {
    offers[i].itemid = (unsigned int) serial.getInt32();
    offers[i].amount = (unsigned int) serial.getInt32();
  };

}

void TradeCancelMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
}

void TradeCancelMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

void TradeOfferAcceptMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
}

void TradeOfferAcceptMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

void TradeConfirmRequestMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
}

void TradeConfirmRequestMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
}

void TradeConfirmResponseMessage::serialise(ByteStream* bs)
{
  Serialiser serial(bs);
  serial.setInt8(type);
  serial.setInt8(id);
  serial.setString(error);
}

void TradeConfirmResponseMessage::deserialise(ByteStream* bs)
{
  Deserialiser serial(bs);
  type = serial.getInt8();
  id = serial.getInt8();
  error = serial.getString();
}

