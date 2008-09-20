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

#include <algorithm>

#include <cssysdef.h>

#include "cmdemotes.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/entity/entitymanager.h"

namespace PT
{
  namespace Chat
  {

    struct Emotes 
    {
      const char* emote;
      const char* synopsis;
      int pose;
      bool echoOnChat;
    };

    // TODO put this in an XML file
    struct Emotes emotelist[] =
    { 
      {"greet", "Wave at someone.", 1, true},
      {"sit", "Sit down.", 2, false},
      {"tentacle", "Flex a tentacle.", 3, true}
    };
 
    #define EMOTELISTSIZE ( sizeof(emotelist) / sizeof (struct Emotes) )
    
    cmdEmote::cmdEmote () : emotes()
    {
      for (unsigned i = 0;  i < EMOTELISTSIZE;  i++)
        emotes.push_back(emotelist[i].emote); 
    }

    cmdEmote::~cmdEmote ()
    {
    }

    bool cmdEmote::CommandHandled (const char* cmd) const
    {
      for (unsigned i = 0;  i < emotes.size();  i++)
      {
        if (emotes[i].compare(cmd) == 0)
          return true;
      }
      return false;
    }

    StringArray cmdEmote::GetAllCommands () const
    { return emotes; }

    std::string cmdEmote::HelpSynopsis (const char* cmd) const
    {
      std::string scmd = cmd;
      for (unsigned i = 0;  i < EMOTELISTSIZE;  i++)
        if ( emotelist[i].emote == scmd )
          return emotelist[i].synopsis;
      return "";
    }

    std::string cmdEmote::HelpUsage (const char* cmd) const
    {
      std::string s = "'/";
      s += cmd;
      s += " [<target>]'";
      return s;
    }

    std::string cmdEmote::HelpFull (const char* cmd) const
    {  
      return  "Lets your character emote.  You may specify a target, which"
        " would direct the emote at a target, or leave out the target, which"
        " would allow you to emote at the world at large.";
    }


    void cmdEmote::Execute (const StringArray& args)
    {
      Network* network = PointerLibrary::getInstance()->getNetwork();
      if(!network) return;
      PT::Entity::EntityManager* entmgr = PointerLibrary::getInstance()->getEntityManager();
      if(!entmgr) return;

      // Element 0 is '/', 1 is emote
      if (args.size() < 2 || args.size() > 3) throw BadUsage();
      
      std::string emote = args[1];
      std::string target = "";
      std::string text = "/" + emote;
      if (args.size() == 3)
      {
        target = args[2];
        text += " " + target; 
      }

      //Magic number - bad developer!
      int poseid = 0;
      bool echoOnChat = false;
      for (unsigned i = 0;  i < EMOTELISTSIZE;  i++)
        if ( emotelist[i].emote == emote )
        {
          poseid = emotelist[i].pose;
          echoOnChat = emotelist[i].echoOnChat;
          break;
        }

      // TODO: Turn and wave to target.
      /*
        PtEntity* targetent = entmgr->findPtEntByName(target);
        PtEntity* ownent = entmgr->getOwnPtEntity();
        ownent->Target(targetent);
        ownent->PlayAction("wave");
      */

      Report(PT::Debug, "emoting at %s", target.c_str());

      if (echoOnChat)
      {
        ChatMessage msg;
        msg.setVolume(2);
        msg.setMessage(text.c_str());
        network->send(&msg);
      }

      //We want to send a pose request to server as well.
      //TODO: If we decide to introduce more poses, replacing this by some
      //"PoseManager" might be nice (ie we wouldn't need to hard-code those
      //commands like now).
      PoseRequestMessage poseMsg;
      poseMsg.setPoseId(poseid); 
      network->send(&poseMsg);
    }

  } // Chat namespace
} // PT namespace
