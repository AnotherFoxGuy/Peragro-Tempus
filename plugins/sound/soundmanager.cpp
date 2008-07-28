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

#include <cssysdef.h>
#include <iutil/objreg.h>
#include <csutil/ref.h>
#include <iutil/document.h>
#include <iutil/vfs.h>
#include "soundmanager.h"

#include <csutil/eventnames.h>


//=======================================================

csEventID SoundEvent::GetEventId() const
{
  csRef<iEventNameRegistry> nameRegistry = csEventNameRegistry::GetRegistry(soundManager->object_reg);
  return nameRegistry->GetID(eventName.c_str());
} // end GetEventId()

bool SoundEvent::Play(iEvent& ev)
{
  ///The sound stream.
  csRef<iSndSysStream> sndstream;
  ///The sound source.
  csRef<iSndSysSource> sndsource;
  csRef<iSndSysSource3D> sndsource3d;

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (soundManager->object_reg);

  csRef<iDataBuffer> soundbuf = vfs->ReadFile (fileName.c_str());
  if (!soundbuf)
  {
    printf("E: Can't load file '%s'!\n", fileName.c_str());
    return false;
  }

  csRef<iSndSysData> snddata = soundManager->sndloader->LoadSound (soundbuf);
  if (!snddata)
  {
    printf("E: Can't load sound '%s'!\n", fileName.c_str());
    return false;
  }

  sndstream = soundManager->sndrenderer->CreateStream (snddata, CS_SND3D_ABSOLUTE);
  if (!sndstream)
  {
    printf("E: Can't create stream for '%s'!\n", fileName.c_str());
    return false;
  }

  sndsource = soundManager->sndrenderer->CreateSource (sndstream);
  if (!sndsource)
  {
    printf("E: Can't create source for '%s'!\n", fileName.c_str());
    return false;
  }

  sndsource3d = scfQueryInterface<iSndSysSource3D> (sndsource);

  sndsource3d->SetPosition (csVector3(0,0,0));
  sndsource->SetVolume (1.0f);

  sndstream->SetLoopState (CS_SNDSYS_STREAM_DONTLOOP);
  sndstream->Unpause ();

  sndstream->SetAutoUnregister(true);

  return true;
} // end GetEventId()

//=======================================================

csEventID SoundEventMaterial::GetEventId() const
{
  csRef<iEventNameRegistry> nameRegistry = csEventNameRegistry::GetRegistry(soundManager->object_reg);
  std::string name = eventName + ".";
  name += GetSourceMaterial() + ".";
  name += GetDestinationMaterial();
  return nameRegistry->GetID(name.c_str());
} // end GetEventId()

//=======================================================

CS_IMPLEMENT_PLUGIN
SCF_IMPLEMENT_FACTORY (SoundManager)

SoundManager::SoundManager(iBase* parent) :
scfImplementationType (this, parent), object_reg(0)
{
  soundDisabled = false;
}

SoundManager::~SoundManager()
{
}

bool SoundManager::HandleEvent (iEvent& ev)
{
  if (soundDisabled) return true;

  csArray<csRef<SoundEvent> > events = sounds.GetAll(ev.GetName());
  for (size_t i = 0; i < events.GetSize(); i++)
  {
    /// TODO: Sort by priority and only play x first sounds or something.
    events[i]->Play(ev);
  }

  return true;
} // end HandleEvent()

bool SoundManager::Initialize (iObjectRegistry* r)
{
  object_reg = r;

  sndrenderer = csQueryRegistry<iSndSysRenderer> (object_reg);
  if (!sndrenderer)
  {
    soundDisabled = true;
    printf("W: Failed to locate sound renderer!\n");
  }

  sndloader = csQueryRegistry<iSndSysLoader> (object_reg);
  if (!sndloader)
  {
    soundDisabled = true;
    printf("W: Failed to locate sound loader!\n");
  }

  return true;
} // end Initialize()

bool SoundManager::LoadSoundEvents(const std::string& fileName)
{
  if (soundDisabled) return true;

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);
  if (!vfs) printf("E: Failed to locate VFS!\n");

  csRef<iDataBuffer> xmlfile = vfs->ReadFile (fileName.c_str());
  if (!xmlfile)
  {
    printf("E: Can't load file '%s'!\n", fileName.c_str());
    return false;
  }

  csRef<iDocumentSystem> docsys (csQueryRegistry<iDocumentSystem> (object_reg));

  csRef<iDocument> doc (docsys->CreateDocument());

  const char* error = doc->Parse (xmlfile, true);
  if (error)
  {
    printf("E: '%s'!\n", error);
    return false;
  }

  csRef<iDocumentNode> xml = doc->GetRoot ();
  xml = xml->GetNode("sounds");

  if (!xml.IsValid()) return false;

  return LoadSoundEvents(xml, 0);
} // end LoadSoundEvents()

bool SoundManager::LoadSoundEvents(iDocumentNode* node, const char* prefix)
{
  if (soundDisabled) return true;

  csRef<iDocumentNodeIterator> it (node->GetNodes ("sound"));

  while (it->HasNext())
  {
    csRef<iDocumentNode> node (it->Next());
    if (!LoadSoundEvent(node, prefix)) return false;
  }

  //Register events.
  csRef<iEventQueue> eventQueue = csQueryRegistry<iEventQueue> (object_reg);
  if (!eventQueue) return false;
  eventQueue->RegisterListener(this);
  // Not be able to do "csEventID events[sounds.GetSize()+1];" sucks.
  csEventID* events = new csEventID[sounds.GetSize()+1];
  csHash<csRef<SoundEvent>, csEventID>::GlobalIterator iter = sounds.GetIterator();
  size_t i = 0;
  while (iter.HasNext())
  {
    events[i] = iter.NextTuple().second;
    i++;
  }
  events[i] = CS_EVENTLIST_END;
  eventQueue->RegisterListener (this, events);

  delete [] events;

  return true;
} // end LoadSoundEvents()

bool SoundManager::LoadSoundEvent(iDocumentNode* node, const char* prefix)
{
  if (soundDisabled) return true;

  csRef<SoundEvent> soundEvent;
  if (node->GetNode("material"))
  {
    std::string src = node->GetNode("material")->GetAttributeValue("source");
    std::string dst = node->GetNode("material")->GetAttributeValue("destination");
    soundEvent.AttachNew(new SoundEventMaterial(this));
    SoundEventMaterial* ev = static_cast<SoundEventMaterial*> (&*soundEvent);
    ev->SetSourceMaterial(src);
    ev->SetDestinationMaterial(dst);
  }
  else if (node->GetNode("action"))
  {
    std::string act = node->GetNode("action")->GetAttributeValue("value");
    soundEvent.AttachNew(new SoundEventAction(this));
    SoundEventAction* ev = static_cast<SoundEventAction*> (&*soundEvent);
    ev->SetActionName(act);
  }
  else
  {
    soundEvent.AttachNew(new SoundEvent(this));
  }

  soundEvent->SetPriority(node->GetNode("priority")->GetAttributeValueAsInt("value"));
  soundEvent->SetFileName(node->GetNode("file")->GetAttributeValue("value"));
  soundEvent->SetEventName(node->GetNode("event")->GetAttributeValue("value"));

  sounds.Put(soundEvent->GetEventId(), soundEvent);

  return true;
} // end LoadSoundEvent()

bool SoundManager::PlayAmbient(const std::string& fileName)
{
  ///The sound source.
  csRef<iSndSysSource> sndsource;
  csRef<iSndSysSource3D> sndsource3d;

  csRef<iVFS> vfs = csQueryRegistry<iVFS> (object_reg);

  csRef<iDataBuffer> soundbuf = vfs->ReadFile (fileName.c_str());
  if (!soundbuf)
  {
    printf("E: Can't load file '%s'!\n", fileName.c_str());
    return false;
  }

  csRef<iSndSysData> snddata = sndloader->LoadSound (soundbuf);
  if (!snddata)
  {
    printf("E: Can't load sound '%s'!\n", fileName.c_str());
    return false;
  }

  ambientSndStream = sndrenderer->CreateStream (snddata, CS_SND3D_DISABLE);
  if (!ambientSndStream)
  {
    printf("E: Can't create stream for '%s'!\n", fileName.c_str());
    return false;
  }

  sndsource = sndrenderer->CreateSource (ambientSndStream);
  if (!sndsource)
  {
    printf("E: Can't create source for '%s'!\n", fileName.c_str());
    return false;
  }

  sndsource->SetVolume (1.0f);

  ambientSndStream->SetLoopState (CS_SNDSYS_STREAM_LOOP);
  ambientSndStream->Unpause ();

  return true;
} // end PlayAmbient()

bool SoundManager::PlayAmbient()
{
  if (ambientSndStream) ambientSndStream->Unpause ();

  return true;
}// end PlayAmbient()

bool SoundManager::StopAmbient()
{
  if (ambientSndStream) ambientSndStream->Pause();

  return true;
} // end StopAmbient()

bool SoundManager::RemoveSound(csEventID eventId)
{
  return true;
} // end RemoveSound()

bool SoundManager::RemoveSound(const std::string& fileName)
{
  return true;
} // end RemoveSound()

float SoundManager::GetVolume()
{
  if (sndrenderer) 
    return sndrenderer->GetVolume();
  else
    return 0.0f;
} // end GetVolume()

void SoundManager::SetVolume(float vol)
{
  if (sndrenderer) sndrenderer->SetVolume(vol);
} // end SetVolume()
