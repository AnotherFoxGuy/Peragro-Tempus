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

#ifndef MOUNTENTITY_H
#define MOUNTENTITY_H

#include <list>
#include <boost/weak_ptr.hpp>

#include "entity.h"
#include "character/character.h"

#include "server/ai/ai.h"


class MountEntity : public Character
{
private:
  class Equal
  {
    boost::shared_ptr<Character> e;
  public:
    Equal(boost::shared_ptr<Character> e) : e(e) {}
    Equal() {}
    bool operator() (const boost::weak_ptr<Character>& c)
    { return (c.expired() || c.lock()==e); }
  };

private:
  std::list<boost::weak_ptr<Character> > passengers;
  size_t max_passengers;

  size_t t_stop;
  size_t t_org;

public:
  MountEntity() : Character(Common::Entity::MountEntityType)
  {
    max_passengers = 1;
  }

  ~MountEntity()
  {
    for (size_t i = 0; i < max_passengers; i++)
    {
      //passengers[i]->UnMount(this);
    }
  }

  virtual void SetPosition(const WFMath::Point<3>& p);

  void AddPassenger(boost::shared_ptr<Character> e);
  void RemovePassenger(boost::shared_ptr<Character> e);
  size_t GetPassengerCount() { passengers.remove_if(Equal()); return passengers.size(); }
  size_t GetMaxPassengers() const { return max_passengers; }

  virtual void LoadFromDB();
  virtual void SaveToDB();
};

#endif // MOUNTENTITY_H
