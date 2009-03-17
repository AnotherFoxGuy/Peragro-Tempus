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

#ifndef PCENTITY_H
#define PCENTITY_H

#include "common/util/monitorable.h"

#include "entity.h"
#include "character/character.h"
#include "character/tradepeer.h"

#include "server/quest/npcdialogstate.h"

class User;
class MountEntity;
class CharChats;

class PcEntity : public Character
{
private:
  User* user;
  ptMonitor<CharChats> charchats;

  TradePeer tradepeer;

  NPCDialogState dialog_state;

  unsigned char pose_id;

  // Moving to the destination instantly!
  bool flashStep;

public:
  PcEntity() : Character(Common::Entity::PCEntityType), user(0)
  {
    tradepeer.setEntity(this);
    dialog_state.setCharacter(this);

    pose_id = 0;

    flashStep = false;
  }

  ~PcEntity()
  {
  }

  void SetUser(User* user);
  User* GetUser() const { return user; }

  void setCharChats(const CharChats* cchats);
  const CharChats* getCharChats() const { return charchats.get(); }

  TradePeer* getTradePeer() { return &tradepeer; }
  NPCDialogState* getNPCDialogState() { return &dialog_state; }

  void setPoseId(unsigned char pose_id) { this->pose_id = pose_id; }
  const unsigned char getPoseId() const { return pose_id; }

  void toggleFlashStep() { flashStep = !flashStep; }
  bool usesFlashStep() const { return flashStep; }

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // PCENTITY_H
