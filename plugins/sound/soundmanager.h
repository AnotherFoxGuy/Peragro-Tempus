/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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
#include <iutil/eventnames.h>
#include <iutil/event.h>
#include <iutil/eventh.h>
#include <iutil/eventq.h>

#include <csutil/hash.h>

#include <csutil/refcount.h>

#include <isndsys/ss_renderer.h>
#include <isndsys/ss_loader.h>
#include <isndsys/ss_source.h>
#include <isndsys/ss_stream.h>
#include <isndsys/ss_data.h>
#include <isndsys/ss_listener.h>

#include "include/soundmanager.h"

struct iObjectRegistry;
struct iSoundManager;

struct iEvent;

class SoundManager;

class SoundEvent : public csRefCount
{
protected:
  SoundManager* soundManager;

protected:
  /// low, norm, high, or always.
  int priority;
  /// Filename of the sound to play.
  std::string fileName;
  /// Eventname.
  std::string eventName;
  /// Event ID.
  csEventID eventId;

public:
  SoundEvent (SoundManager* sndmgr) : soundManager(sndmgr) {}
  virtual ~SoundEvent () {}

  void SetPriority(unsigned int value) { priority = value; }
  unsigned int GetPriority() const { return priority; }

  void SetFileName(const std::string& value) { fileName = value; }
  const std::string& GetFileName() const { return fileName; }

  void SetEventName(const std::string& value) { eventName = value; }
  const std::string& GetEventName() const { return eventName; }

  virtual csEventID GetEventId() const;

  virtual bool Play (iEvent& ev);
};

class SoundEventMaterial : public SoundEvent
{
private:
  std::string sourceMaterial;
  std::string destinationMaterial;

public:
  SoundEventMaterial (SoundManager* sndmgr) : SoundEvent(sndmgr) {}
  virtual ~SoundEventMaterial () {}

  void SetSourceMaterial(const std::string& value) { sourceMaterial = value; }
  const std::string& GetSourceMaterial() const { return sourceMaterial; }

  void SetDestinationMaterial(const std::string& value) { destinationMaterial = value; }
  const std::string& GetDestinationMaterial() const { return destinationMaterial; }

  virtual csEventID GetEventId() const;

  //virtual bool Play (iEvent& ev) { return false; }
};

class SoundEventAction : public SoundEvent
{
private:
  std::string actionName;

public:
  SoundEventAction (SoundManager* sndmgr) : SoundEvent(sndmgr) {}
  virtual ~SoundEventAction () {}

  void SetActionName(const std::string& value) { actionName = value; }
  const std::string& GetActionName() const { return actionName; }

  //virtual bool Play (iEvent& ev) { return false; }
};

class SoundManager : public scfImplementation3<SoundManager, iSoundManager, iEventHandler, iComponent>
{
private:
  friend class SoundEvent;
  friend class SoundEventMaterial;
  friend class SoundEventAction;

  iObjectRegistry* object_reg;
  csHash<csRef<SoundEvent>, csEventID> sounds;

  bool soundDisabled;

  ///The sound renderer.
  csRef<iSndSysRenderer> sndrenderer;

  ///The sound loader.
  csRef<iSndSysLoader> sndloader;

private:
  ///The ambient sound stream.
  csRef<iSndSysStream> ambientSndStream;

private:
  virtual bool HandleEvent(iEvent& ev);

public:
  SoundManager (iBase* parent);
  virtual ~SoundManager();

  virtual bool Initialize (iObjectRegistry*);

  virtual bool LoadSoundEvents(const std::string& fileName);
  virtual bool LoadSoundEvents(iDocumentNode* node, const char* prefix = 0);
  virtual bool LoadSoundEvent(iDocumentNode* node, const char* prefix);

  virtual bool RemoveSound(csEventID eventId);
  virtual bool RemoveSound(const std::string& fileName);

  virtual bool PlayAmbient(const std::string& fileName);
  virtual bool PlayAmbient();
  virtual bool StopAmbient();

  virtual float GetVolume();
  virtual void SetVolume(float vol);

  CS_EVENTHANDLER_NAMES ("peragro.soundmanager")
  CS_EVENTHANDLER_NIL_CONSTRAINTS
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

