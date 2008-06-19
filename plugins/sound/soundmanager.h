/*
    Copyright (C) 2001 by Jorrit Tyberghein

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef SOUNDMANAGER_H_
#define SOUNDMANAGER_H_

#include <csutil/scf_implementation.h>
#include <iutil/comp.h>
#include <csutil/csstring.h>

#include "include/soundmanager.h"

struct iObjectRegistry;
struct iSoundManager;

class SoundEvent : public virtual iBase
{
protected:
  /// low, norm, high, or always.
  int priority;
  /// Filename of the sound to play.
  std::string fileName;
  /// Eventname.
  std::string eventId;

public:
  SoundEvent ();
  virtual ~SoundEvent ();

  virtual bool Play (iEvent& ev);
};

class SoundEventMaterial : public SoundEvent
{
private:
  std::string sourceMaterial;
  std::string destinationMaterial;

public:
  SoundEventMaterial ();
  virtual ~SoundEventMaterial ();

  virtual bool Play (iEvent& ev);
};

class SoundEventAction : public SoundEvent
{
private:
  std::string actionName;

public:
  SoundEventAction ();
  virtual ~SoundEventAction ();

  virtual bool Play (iEvent& ev);
};

class SoundManager : public scfImplementation2<SoundManager,iSoundManager,iComponent>
{
private:
  iObjectRegistry* object_reg;

  std::hashmap<std::string, iSoundEvent> sounds;

  virtual bool HandleEvent(iEvent& ev);

public:
    SoundManager (iBase* parent);
    virtual ~SoundManager();

    // From iComponent.
    virtual bool Initialize (iObjectRegistry*);

    virtual bool LoadSoundEvents(const char* fileName);
    virtual bool LoadSoundEvents(iDocumentNode* node, const char* prefix = 0);

    virtual bool RemoveSound(const char* eventId);
    virtual bool RemoveSound(const char* fileName);
};

#endif // SOUNDMANAGER_H_

/*
<!-- "interface.button.optionswindow.ok" -->
<sound>
  <event value="interface.button" />
  <file value="/foob/click.wav"/>
  <priority value='Normal'/>
</sound>

<!-- "combat.attack.hit.glance.SteelSword.Leather" -->
<sound>
  <event value="combat.attack.hit.glance" />
  <material source='SteelSword' destination='Leather' />
  <file value="/foob/bar.wav"/>
  <priority value='Normal'/>
</sound>

<!-- Meant to be loaded with a prefix full event becomes: "prefix.action" 
      ex. npc.Bob.Die or item.SteelSword.Drop -->
<sound>
  <action value="Die" /> 
  <file value="/foob/bar.wav"/>
  <priority value='Normal'/>
</sound>
*/


/*
Source material is found by a check for material done first on the source object's ammo, 
then on the contents (if any) of the weapon hand, and finally on the actor template itself. 
With this in mind, an archer could have a source of "arrow", a spellcaster could have a source of "fireball", 
a sword-wielding fighter could have a source of "steelsword", 
and an unarmed krug scout could have a source of "flesh" (fists).

Destination material is found by a check for material done first on the destination object's shield, 
followed by a check for material on the torso armor, and finally on the actor's template itself. 
For example, if a warrior wearing plate armor is holding a wooden buckler, 
the warrior's destination material is "wood" (the material specification of the shield). 
However, if that same warrior then dropped the shield, 
the destination material becomes "steel" (the material specification of the plate armor).
*/