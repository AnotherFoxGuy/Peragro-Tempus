/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

/**
 * @file interaction.h
 *
 * @basic Server interaction manager.
 */

#ifndef INTERACTION
#define INTERACTION

#ifdef WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #define ssize_t SSIZE_T
#endif

#include "src/server/entity/pcentity.h"
#include "src/common/combat/combat.h"

/**
 * Server interaction manager.
 */
class Interaction
{
  public:
    /// The interaction ID associated with this interaction, 0 = unset.
    unsigned int interactionID;
    /// Holds the unlocked character that is associated with this action.
    boost::weak_ptr<PcEntity> character;
    /// The amount of extra stamina required for this interaction.
    float staminaRequired;
    /// The time to perform this interaction.
    ssize_t time;


    /// Constructor.
    Interaction();
    /// Destructor.
    ~Interaction();
};

#endif //INTERACTION
