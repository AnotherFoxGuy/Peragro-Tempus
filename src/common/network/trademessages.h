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

#ifndef _TRADEMESSAGES_H_
#define _TRADEMESSAGES_H_

#include "netmessage.h"

namespace TRADE
{
  enum MESSAGES
  {
    TRADEREQUEST=0,
    TRADERESPONSE=1,
    TRADEOFFERSLISTNPC=2,
    BUYITEMREQUESTNPC=3,
    BUYITEMRESPONSENPC=4,
    TRADEOFFERSLISTPVP=5,
    TRADECANCELREQUEST=6,
    TRADECANCEL=7,
    TRADEOFFERACCEPT=8,
    TRADECONFIRMREQUEST=9,
    TRADECONFIRMRESPONSE=10
  };
}

class TradeRequestMessage : public NetMessage
{

public:
  TradeRequestMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEREQUEST)
  {
  }

  ~TradeRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeResponseMessage : public NetMessage
{

public:
  TradeResponseMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADERESPONSE)
  {
  }

  ~TradeResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeOffersListNpcMessage : public NetMessage
{

public:
  TradeOffersListNpcMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEOFFERSLISTNPC)
  {
  }

  ~TradeOffersListNpcMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class BuyItemRequestNpcMessage : public NetMessage
{

public:
  BuyItemRequestNpcMessage() : NetMessage(MESSAGES::TRADE,TRADE::BUYITEMREQUESTNPC)
  {
  }

  ~BuyItemRequestNpcMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class BuyItemResponseNpcMessage : public NetMessage
{

public:
  BuyItemResponseNpcMessage() : NetMessage(MESSAGES::TRADE,TRADE::BUYITEMRESPONSENPC)
  {
  }

  ~BuyItemResponseNpcMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeOffersListPvpMessage : public NetMessage
{

public:
  TradeOffersListPvpMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEOFFERSLISTPVP)
  {
  }

  ~TradeOffersListPvpMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeCancelRequestMessage : public NetMessage
{

public:
  TradeCancelRequestMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADECANCELREQUEST)
  {
  }

  ~TradeCancelRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeCancelMessage : public NetMessage
{

public:
  TradeCancelMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADECANCEL)
  {
  }

  ~TradeCancelMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeOfferAcceptMessage : public NetMessage
{

public:
  TradeOfferAcceptMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEOFFERACCEPT)
  {
  }

  ~TradeOfferAcceptMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeConfirmRequestMessage : public NetMessage
{

public:
  TradeConfirmRequestMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADECONFIRMREQUEST)
  {
  }

  ~TradeConfirmRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

class TradeConfirmResponseMessage : public NetMessage
{

public:
  TradeConfirmResponseMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADECONFIRMRESPONSE)
  {
  }

  ~TradeConfirmResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

#endif // _TRADEMESSAGES_H_
