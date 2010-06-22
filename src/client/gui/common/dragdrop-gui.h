/*
    Copyright (C) 2005 Development Team of Peragro Tempus

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General public License for more details.

    You should have received a copy of the GNU General public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef DRAGDROP_GUI_H
#define DRAGDROP_GUI_H

#include "cssysdef.h"

#include "iutil/string.h"
#include "iutil/vfs.h"

#include <CEGUI.h>
#include "ivaria/icegui.h"

class Network;
class Slot;
class Object;

namespace PT
{
  namespace Data
  {
    class ItemDataManager;
  }

  namespace GUI
  {
    class GUIManager;

    namespace Windows
    {
      class DragDrop
      {
      private:
        PT::GUI::GUIManager* guimanager;
        Network* network;
        CEGUI::WindowManager* winMgr;
        PT::Data::ItemDataManager* itemDataManager;
        int counter;

      public:
        bool handleDragEnter(const CEGUI::EventArgs& args);
        bool handleDragLeave(const CEGUI::EventArgs& args);
        bool handleDragDropped(const CEGUI::EventArgs& args);
        bool handleDragDroppedIcon(const CEGUI::EventArgs& args);
        bool handleDragDroppedRoot(const CEGUI::EventArgs& args);
        bool handleDragDroppedTrade(const CEGUI::EventArgs& args);
        bool handleDragDroppedBuy(const CEGUI::EventArgs& args);
        bool handleRightClickedIcon(const CEGUI::EventArgs& args);

        enum Type
        {
          Item=0,
          Skill=1
        };

      public:
        DragDrop(PT::GUI::GUIManager* guimanager);
        virtual ~DragDrop();

        CEGUI::Window* createIcon(int icontype, int objectid,
          const char* name, const char* iconname, bool interactable = true);
        Object* CreateItem(uint itemid,
          const char* name, const char* iconname, bool interactable = true);
        void MoveObject(Slot* oldslot, Slot* newslot);

        CEGUI::String IntToStr(int number);
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // DRAGDROP_GUI_H
