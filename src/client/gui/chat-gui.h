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

#ifndef CHAT_GUI_H
#define CHAT_GUI_H

#include "base-gui.h"

#define CHATWINDOW "Chatlog/Frame"

namespace PT
{
  namespace GUI
  {
    namespace Windows
    {
      class ChatWindow : public GUIWindow
      {
      private:
        bool OnDropList(const CEGUI::EventArgs& e);
        bool OnSlider(const CEGUI::EventArgs& e);
        bool OnRootKeyDown(const CEGUI::EventArgs& e);
        void CreateDropList();
        bool HandleCommand(const char* texti);

        CEGUI::SlotFunctorBase* sumbitEventSubscriber;
        bool visible;

      public:
        ChatWindow(GUIManager* guimanager);
        virtual ~ChatWindow();
        bool Create();

        void AddChatMessage (const char* nick, const char* msg);
        void AddMessage (const char* msg);

        /// Set the behaviour for the load button.
        void SetSubmitEvent(CEGUI::SlotFunctorBase* subscriber);

        void HideWindow();
        void ShowWindow();
      };

    } // Windows namespace
  } // GUI namespace
} // PT namespace

#endif // CHAT_GUI_H
