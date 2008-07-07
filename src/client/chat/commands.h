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

#ifndef COMMANDS_H
#define COMMANDS_H

#include <cssysdef.h>

#include "commanddefault.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/entity/entitymanager.h"

namespace PT
{
  namespace Chat
  {
    //--------------------------------------------------------------------------
    class cmdGreet : public Command
    {
    public:
      cmdGreet () { }
      virtual ~cmdGreet () { }
      virtual const char* GetCommand () { return "greet"; }
      virtual const char* GetDescription () { return "Wave at someone."; }
      virtual const char* Help () { return "Usage: '/greet <target>'"; }
      virtual void Execute (const StringArray& args)
      {
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;
        PT::Entity::EntityManager* entmgr = PointerLibrary::getInstance()->getEntityManager();
        if(!entmgr) return;

        // Element 0 is '/', 1 is 'greet'
        if (args.size() < 3 || args.size() > 3) throw BadUsage();
        else
        {
          std::string target = args[2];
          std::string text = "/greet " + target;

          // TODO: Turn and wave to target.
          /*
          PtEntity* targetent = entmgr->findPtEntByName(target);
          PtEntity* ownent = entmgr->getOwnPtEntity();
          ownent->Target(targetent);
          ownent->PlayAction("wave");
          */

          Report(PT::Debug, "waving at %s", target.c_str());
          ChatMessage msg;
          msg.setVolume(2);
          msg.setMessage(text.c_str());
          network->send(&msg);

          //We want to send a pose request to server as well.
          //TODO: If we decide to introduce more poses, replacing this by some
          //"PoseManager" might be nice (ie we wouldn't need to hard-code those
          //commands like now).
          PoseRequestMessage poseMsg;
          poseMsg.setPoseId(1); //Magic number - bad developer!
          network->send(&poseMsg);

          return;
        }
      }
    };
    //--------------------------------------------------------------------------
    class cmdSit : public Command
    {
    public:
      cmdSit () { }
      virtual ~cmdSit () { }
      virtual const char* GetCommand () { return "sit"; }
      virtual const char* GetDescription () { return "Sit down."; }
      virtual const char* Help () { return "Usage: '/sit'"; }
      virtual void Execute (const StringArray& args)
      {
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;
        PT::Entity::EntityManager* entmgr = PointerLibrary::getInstance()->getEntityManager();
        if(!entmgr) return;

        // Element 0 is '/', 1 is 'sit'
        if (args.size() < 2 || args.size() > 3) throw BadUsage();
        else
        {
          //We want to send a pose request to server as well.
          //TODO: If we decide to introduce more poses, replacing this by some
          //"PoseManager" might be nice (ie we wouldn't need to hard-code those
          //commands like now).
          PoseRequestMessage poseMsg;
          poseMsg.setPoseId(2); //Magic number - bad developer!
          network->send(&poseMsg);

          return;
        }
      }
    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // COMMANDS_H
