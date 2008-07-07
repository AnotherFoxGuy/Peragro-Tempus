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

#ifndef CMDSAY_H
#define CMDSAY_H

#include "commanddefault.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

namespace PT
{
  namespace Chat
  {
    //--------------------------------------------------------------------------
    class cmdSay : public CommandDefault
    {
    public:
      cmdSay (const char* cmd="say") : CommandDefault(cmd) { }
      virtual ~cmdSay () { }

      virtual const char* HelpUsage(const char*) const 
      { return "'/say <message>'"; }

      virtual const char* HelpSynopsis (const char*) const
      { return "Say something to the world."; }

      virtual const char* HelpFull (const char*) const
      { return "Use this command to enter a full message to say to others around you."; }

      virtual void Execute (const StringArray& args) { Say(2, args); }

    protected:
      void Say(unsigned char volume, const StringArray& args, const char* pre="")  
      {
        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;

        // determine start of text string, depending on whether the
        // command was invoked with a '/' or not
        size_t i = 0;
        if (args.size() > 0 && args[0].compare("/") == 0) i = 2;

        if (args.size() <= i) throw BadUsage();

        std::string text = pre;
        for(; i < args.size(); i++)
        {
          text += args[i];
          text += " ";
        }

        Report(PT::Debug, "Say: %s", text.c_str());
        ChatMessage msg;
        msg.setVolume(volume);
        msg.setMessage(text.c_str());
        network->send(&msg);

        return;
      }
    };
    //--------------------------------------------------------------------------
    class cmdShout : public cmdSay
    {
    public:
      cmdShout () : cmdSay("shout") { }
      virtual ~cmdShout () { }

      virtual const char* HelpSynopsis (const char*) const
      { return "Shout something to the world."; }
      virtual const char* HelpUsage (const char*) const
      { return "Usage: '/shout <message>'"; }

      virtual void Execute (const StringArray& args) { Say(20, args); }
    };
    //--------------------------------------------------------------------------
    class cmdSayMe : public cmdSay
    {
    public:
      cmdSayMe () : cmdSay("me") { }
      virtual ~cmdSayMe () { }

      virtual const char* HelpSynopsis (const char*) const 
      { return "Say something in third person to the world."; }
      virtual const char* HelpUsage (const char*) const 
      { return "Usage: '/me <message>'"; }
      virtual const char* HelpFull (const char*) const
      { 
        return "Use this command to enter a full message to say to those"
          " around you.  The /me at the beginning will be replaced by the"
          " name of the speaker.";
      }

      virtual void Execute (const StringArray& args) { Say(2, args, "/me"); }
    };
    //--------------------------------------------------------------------------
  } // Chat namespace
} // PT namespace

#endif // CMDSAY_H
