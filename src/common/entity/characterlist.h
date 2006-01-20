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

#ifndef _CHARACTERLIST_H_
#define _CHARACTERLIST_H_

#include <vector>

#include "character.h"
#include "server/database/table-characters.h"

class CharacterManager;

class CharacterList
{
private:
  std::vector<Character*> characters;

  friend CharacterManager;

public:
  CharacterList() {}

  size_t getCharacterCount()
  {
    return characters.size();
  }

  Character* getCharacter(size_t index)
  {
    return characters[index];
  }

  void addCharacter(Character* character)
  {
    characters.push_back(character);
  }

  void delCharacter(size_t index)
  {
    characters.erase(characters.begin() + index);
  }

  void delCharacter(Character* character)
  {
    if (!character) return;
    std::string name = character->getName();
    if (name == "") return;
    for (size_t i = 0; i < characters.size(); i++)
    {
      Character* _character = characters[i];
      if (_character->compare(character))
      {
        characters.erase(characters.begin() + i);
        return;
      }
    }
  }

  bool exists(Character* character)
  {
    if (!character) return false;
    std::string name = character->getName();
    if (name == "") return false;
    for (size_t i = 0; i < characters.size(); i++)
    {
      Character* _character = characters[i];

      if (_character->compare(character))
        return true;
    }
    return false;
  }

  Character* findByName(const std::string& name)
  {
    if (name == "") return 0;
    for (size_t i = 0; i < characters.size(); i++)
    {
      Character* character = characters[i];
      if (character->getName() == name)
      {
        return character;
      }
    }
    return 0;
  }

  Character* findById(int id)
  {
    for (size_t i = 0; i<characters.getCount(); i++)
    {
      Character* character = characters[i];
      if (character->getId() == id)
      {
        return character;
      }
    }
    return 0;
  }

  void clear()
  {
    characters.clear();
  }
};

#endif // _CHARACTERLIST_H_
