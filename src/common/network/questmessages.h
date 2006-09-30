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

#ifndef _QUESTMESSAGES_H_
#define _QUESTMESSAGES_H_

#include "netmessage.h"

namespace QUEST
{
  enum MESSAGES
  {
    NPCDIALOG=0,
    NPCDIALOGANSWER=1
  };
}

class NpcDialogMessage : public NetMessage
{
  const char* chatdialog;
  class Listunnamed1
  {
  public:
  };

  unsigned char unnamed1count;
  Listunnamed1* unnamed1;


public:
  NpcDialogMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCDIALOG)
  {
    unnamed1 = 0;
  }

  ~NpcDialogMessage()
  {
    delete [] unnamed1;
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  const char* getChatDialog() { return chatdialog; }
  void setChatDialog(const char* x) { chatdialog = x; }

  unsigned char getunnamed1Count() { return unnamed1count; }
  void setunnamed1Count(unsigned char ic)
  {
    unnamed1count = ic;
    delete [] unnamed1;
    unnamed1 = new Listunnamed1[ic];
  }

  // --- begin Listunnamed1 Getter and Setter ---

  // --- end Listunnamed1 Getter and Setter ---

};

class NpcDialogAnswerMessage : public NetMessage
{

public:
  NpcDialogAnswerMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCDIALOGANSWER)
  {
  }

  ~NpcDialogAnswerMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

};

#endif // _QUESTMESSAGES_H_
