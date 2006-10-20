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
    TRADECANCEL=6,
    TRADEOFFERACCEPT=7,
    TRADECONFIRMREQUEST=8,
    TRADECONFIRMRESPONSE=9
  };
}

class TradeRequestMessage : public NetMessage
{
  unsigned int entityid;

public:
  TradeRequestMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEREQUEST)
  {
  }

  ~TradeRequestMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getEntityId() { return entityid; }
  void setEntityId(unsigned int x) { entityid = x; }

};

class TradeResponseMessage : public NetMessage
{
  ptString error;

public:
  TradeResponseMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADERESPONSE)
  {
  }

  ~TradeResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() { return error; }
  void setError(ptString x) { error = x; }

};

class TradeOffersListNpcMessage : public NetMessage
{
  class ListOffers
  {
  public:
    unsigned int itemid;
    unsigned int price;
  };

  unsigned char offerscount;
  ListOffers* offers;


public:
  TradeOffersListNpcMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEOFFERSLISTNPC)
  {
    offers = 0;
  }

  ~TradeOffersListNpcMessage()
  {
    delete [] offers;
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getOffersCount() { return offerscount; }
  void setOffersCount(unsigned char ic)
  {
    offerscount = ic;
    delete [] offers;
    offers = new ListOffers[ic];
  }

  // --- begin ListOffers Getter and Setter ---

  unsigned int getItemId(size_t i) { return offers[i].itemid; }
  void setItemId(size_t i, unsigned int x) { offers[i].itemid = x; }

  unsigned int getPrice(size_t i) { return offers[i].price; }
  void setPrice(size_t i, unsigned int x) { offers[i].price = x; }

  // --- end ListOffers Getter and Setter ---

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
  ptString error;

public:
  BuyItemResponseNpcMessage() : NetMessage(MESSAGES::TRADE,TRADE::BUYITEMRESPONSENPC)
  {
  }

  ~BuyItemResponseNpcMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() { return error; }
  void setError(ptString x) { error = x; }

};

class TradeOffersListPvpMessage : public NetMessage
{
  class ListOffers
  {
  public:
    unsigned int itemid;
    unsigned int amount;
    unsigned char slotid;
  };

  unsigned char offerscount;
  ListOffers* offers;


public:
  TradeOffersListPvpMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADEOFFERSLISTPVP)
  {
    offers = 0;
  }

  ~TradeOffersListPvpMessage()
  {
    delete [] offers;
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned char getOffersCount() { return offerscount; }
  void setOffersCount(unsigned char ic)
  {
    offerscount = ic;
    delete [] offers;
    offers = new ListOffers[ic];
  }

  // --- begin ListOffers Getter and Setter ---

  unsigned int getItemId(size_t i) { return offers[i].itemid; }
  void setItemId(size_t i, unsigned int x) { offers[i].itemid = x; }

  unsigned int getAmount(size_t i) { return offers[i].amount; }
  void setAmount(size_t i, unsigned int x) { offers[i].amount = x; }

  unsigned char getSlotId(size_t i) { return offers[i].slotid; }
  void setSlotId(size_t i, unsigned char x) { offers[i].slotid = x; }

  // --- end ListOffers Getter and Setter ---

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
  ptString error;

public:
  TradeConfirmResponseMessage() : NetMessage(MESSAGES::TRADE,TRADE::TRADECONFIRMRESPONSE)
  {
  }

  ~TradeConfirmResponseMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  ptString getError() { return error; }
  void setError(ptString x) { error = x; }

};

#endif // _TRADEMESSAGES_H_
