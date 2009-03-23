/*
    Copyright (C) 2007 Development Team of Peragro Tempus

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

#ifndef PT_DATACHAT_H
#define PT_DATACHAT_H

#include <string>

namespace PT
{
  namespace Data
  {
    /**
     * @ingroup data_handlers
     * Helper class that contains basic information about chanspace. Usually 
     * not created directly, but accessed via ChatDataManager instance. Use the
     * Set/Get methods to access the chanspace properties.
     * @see ChatDataManager
     */
    class ChanSpace
    {
    private:
      ///Name of the chanspace.
      std::string type;
      ///Regexp identifying chanspace members.
      std::string form;
      ///Max number of channels allowed in this channelspace.
      int maxchannels;

      ///Permissions for creating a channel.
      std::string create_type;
      int create_val;
      int create_max;
      bool create_perm;

      ///Permissions for joining a channel.
      std::string join_type;
      int join_val;
      bool join_perm;

      ///Permissions for sending invitations to a channel.
      std::string invite_type;
      int invite_val;

      ///External visibility of the channel.
      bool vis_channel, vis_members;

    public:
      ChanSpace() 
        : type(""), form(""), maxchannels(-1), 
        create_type("user"), create_val(0), create_max(-1), create_perm(false),
        join_type("user"), join_val(0), join_perm(false),
        invite_type("none"), invite_val(0),
        vis_channel(true), vis_members(true)
      {}

      void SetType(const std::string& t) { type = t; }
      const std::string& GetType() const { return type; }

      void SetForm(const std::string& f) { form = f; }
      const std::string& GetForm() const { return form; }

      void SetMaxChannels(int m) { maxchannels = m; }
      int GetMaxChannels() const { return maxchannels; }

      void SetCreateType(const std::string& t) { create_type = t; }
      const std::string& GetCreateType() const { return create_type; }
      
      void SetCreateVal(int v) { create_val = v; }
      int GetCreateVal() const { return create_val; }
      
      void SetCreateMax(int m) { create_max = m; }
      int GetCreateMax() const { return create_max; }

      void SetCreatePerm(bool p) { create_perm = p; }
      bool GetCreatePerm() const { return create_perm; }

      void SetJoinType(const std::string& t) { join_type = t; }
      const std::string& GetJoinType() const { return join_type; }

      void SetJoinVal(int v) { join_val = v; }
      int GetJoinVal() const { return join_val; }

      void SetJoinPerm(bool p) { join_perm = p; }
      bool GetJoinPerm() const { return join_perm; }

      void SetInviteType(const std::string& t) { invite_type = t; }
      const std::string& GetInviteType() const { return invite_type; }

      void SetInviteVal(int v) { invite_val = v; }
      int GetInviteVal() const { return invite_val; }

      void SetVisibilityChannel(bool v) { vis_channel = v; }
      bool GetVisibilityChannel() const { return vis_channel; }
 
      void SetVisibilityMembers(bool v) { vis_members = v; }
      bool GetVisibilityMembers() const { return vis_members; }

    };
  } // Data namespace
} // PT namespace

#endif // PT_DATADOOR_H
