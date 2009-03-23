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

#ifndef PT_DATACHATDATAMANAGER_H
#define PT_DATACHATDATAMANAGER_H

#include <vector>
#include <string>

#include "src/client/data/datamanager.h"

namespace PT
{
  namespace Data
  {
    //Forward declarations
    class ChanSpace;

    /**
     * @ingroup data_handlers
     * Handles the loading of chat data from XML definition file. The data
     * resides in $DATA/xml/chat/chat.xml file, where $DATA is the parent
     * directory where the Peragro Tempus data resides.
     */
    class ChatDataManager : public DataManager
    {
    private:
      std::vector<std::string> initgroups;
      std::vector< std::pair<std::string, bool> > defgroups;
      int listdef, listmax, maxchannels;
      std::vector<ChanSpace*> chanspaces;

    public:
      /**
       * Base constructor
       */
      ChatDataManager(PointerLibrary* ptrlib);
      ~ChatDataManager();

      /**
       * @param name Name of the wanted chanspace.
       * @return Pointer to chanspace with given Name, or 0 if none was found.
       */
      ChanSpace* GetChanSpaceByName(const std::string& name) const;

      void GetAllChanSpaces(std::vector<ChanSpace*>& list) 
      { list = chanspaces; }

      int GetMaxChannels() const { return maxchannels; }
      int GetListDefault() const { return listdef; }
      int GetListMax() const { return listmax; }

      const std::vector<std::string>& GetInitGroups() const 
      { return initgroups; }
      const std::vector< std::pair<std::string, bool> >& GetDefGroups() const 
      { return defgroups; }

      /// Implements the DataManager superclass
      bool parseElement(iDocumentNode* node);

      /// Root Node of the xml: NULL
      const char* getRootName() { return 0; }

      /// Element Node of the xml: &lt;chatgroups&gt;
      const char* getElementName() { return "chatgroups"; }
    };
  } // Data namespace
} // PT namespace

#endif // PT_DATACHATDATAMANAGER_H
