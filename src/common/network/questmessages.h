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
    NPCDIALOGANSWER=1,
    NPCSTARTDIALOG=2,
    NPCENDDIALOG=3
  };
}

class NpcDialogMessage : public NetMessage
{
  unsigned int dialogid;
  const char* dialogtext;
  class ListAnswers
  {
  public:
    unsigned int answerid;
    const char* answertext;
  };

  unsigned char answerscount;
  ListAnswers* answers;


public:
  NpcDialogMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCDIALOG)
  {
    answers = 0;
  }

  ~NpcDialogMessage()
  {
    delete [] answers;
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getDialogId() { return dialogid; }
  void setDialogId(unsigned int x) { dialogid = x; }

  const char* getDialogText() { return dialogtext; }
  void setDialogText(const char* x) { dialogtext = x; }

  unsigned char getAnswersCount() { return answerscount; }
  void setAnswersCount(unsigned char ic)
  {
    answerscount = ic;
    delete [] answers;
    answers = new ListAnswers[ic];
  }

  // --- begin ListAnswers Getter and Setter ---

  unsigned int getAnswerId(size_t i) { return answers[i].answerid; }
  void setAnswerId(size_t i, unsigned int x) { answers[i].answerid = x; }

  const char* getAnswerText(size_t i) { return answers[i].answertext; }
  void setAnswerText(size_t i, const char* x) { answers[i].answertext = x; }

  // --- end ListAnswers Getter and Setter ---

};

class NpcDialogAnswerMessage : public NetMessage
{
  unsigned int dialogid;
  unsigned int answerid;

public:
  NpcDialogAnswerMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCDIALOGANSWER)
  {
  }

  ~NpcDialogAnswerMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getDialogId() { return dialogid; }
  void setDialogId(unsigned int x) { dialogid = x; }

  unsigned int getAnswerId() { return answerid; }
  void setAnswerId(unsigned int x) { answerid = x; }

};

class NpcStartDialogMessage : public NetMessage
{
  unsigned int npcid;

public:
  NpcStartDialogMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCSTARTDIALOG)
  {
  }

  ~NpcStartDialogMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getNpcId() { return npcid; }
  void setNpcId(unsigned int x) { npcid = x; }

};

class NpcEndDialogMessage : public NetMessage
{
  unsigned int npcid;

public:
  NpcEndDialogMessage() : NetMessage(MESSAGES::QUEST,QUEST::NPCENDDIALOG)
  {
  }

  ~NpcEndDialogMessage()
  {
  }

  void serialise(ByteStream* bs);
  void deserialise(ByteStream* bs);

  unsigned int getNpcId() { return npcid; }
  void setNpcId(unsigned int x) { npcid = x; }

};

#endif // _QUESTMESSAGES_H_
