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

#ifndef CLIENT_H
#define CLIENT_H

#include <cssysdef.h>
#include <csutil/ref.h>
#include <csutil/refarr.h>
#include <csutil/parray.h>
#include <csutil/csbaseeventh.h>
#include <cstool/csapplicationframework.h>
#include <iengine/engine.h>
#include <iutil/vfs.h>
#include <iutil/pluginconfig.h>
#include <iutil/virtclk.h>
#include <ivideo/graph3d.h>

#include <isndsys/ss_renderer.h>
#include <isndsys/ss_loader.h>
#include <isndsys/ss_source.h>
#include <isndsys/ss_stream.h>
#include <isndsys/ss_data.h>
#include <isndsys/ss_listener.h>

#include <behaviourlayer/bl.h>
#include <physicallayer/pl.h>
#include <physicallayer/entity.h>

#include "common/network/netmessage.h"
#include "common/util/mutex.h"

#include "client/event/eventmanager.h"
#include "client/event/regionevent.h"

#include "client/pointer/pointer.h"

struct iPcDefaultCamera;
struct iPcActorMove;
struct iSector;

struct iCommandLineParser;
struct iPath;
struct iView;

class Network;
class GUIManager;
class EffectsManager;
class CombatMGR;
class ItemMGR;
class PtConsole;
class Cursor;

namespace PT
{
	namespace Entity
	{
		class EntityManager;
	} // Entity namespace 

	namespace Events
	{
		class EventManager;
	} // Events namespace 

	namespace Chat
	{
		class ChatManager;
	} // Chat namespace 

	class Reporter;

	class Client : public csApplicationFramework, public csBaseEventHandler
	{
	public:
		/// Game states
		enum eSTATE
		{
			STATE_INITIAL = 0,
			STATE_INTRO = 1,
			STATE_CONNECTED = 2,
			STATE_LOGGED_IN = 3,
			STATE_SELECTING_CHAR = 4,
			STATE_PLAY = 5,
			STATE_RECONNECTED = 6
		};

		///Game internal state
		eSTATE state;

	private:
		void PreProcessFrame();
		void ProcessFrame();
		void FinishFrame();

		bool OnKeyboard(iEvent&);
		bool OnMouseDown(iEvent& e);
		bool OnMouseMove(iEvent&);
		iPcActorMove* getPcActorMove();
		bool InitializeCEL();
		void checkConnection();
		void handleStates();

	private:
		PointerLibrary pointerlib;

		csRef<iEngine> engine;
		csRef<iGraphics3D> g3d;
		csRef<iVirtualClock> vc;
		//csRef<iKeyboardDriver> kbd;
		csRef<iVFS> vfs;
		csRef<iCommandLineParser> cmdline;
		csRef<iView> view;
		csRef<iCelPlLayer> pl;
		csRef<iCelBlLayer> bl;
		csRef<iCelEntity> zonemanager;

		// The sound renderer.
		csRef<iSndSysRenderer> sndrenderer;

		// The sound loader.
		csRef<iSndSysLoader> sndloader;

		// The sound stream.
		csRef<iSndSysStream> sndstream;

		// The sound source.
		csRef<iSndSysSource> sndsource;
		csRef<iSndSysSourceSoftware3D> sndsource3d;

		// The config manager.
		csRef<iConfigManager> app_cfg;

		Reporter* reporter;
		Network* network;
		GUIManager* guimanager;
		EffectsManager* effectsmanager;
		PT::Entity::EntityManager* entitymanager;
		CombatMGR* combatmanager;
		ItemMGR* itemmanager;
		Cursor* cursor;
		PtConsole* ptconsole;

		PT::Events::EventManager* eventmanager;
		PT::Chat::ChatManager* chatmanager;

	private:
		bool playing;
		char walk, turn;
		Mutex mutex;
		csTicks timer;
		iSector *room;
		float rotX, rotY;
		float cameradistance;
		bool world_loaded;
		int limitFPS;
		csTicks last_sleep;
		csTicks last_seen;

		// needed for relogin on disconnect
		csString user;
		csString pass;
		unsigned int char_id;

	public:
		bool loggedIn(PT::Events::Eventp ev);
		bool LoadRegion(PT::Events::Eventp ev);
		bool Connected(PT::Events::Eventp ev);

	public:
		Client();
		~Client();

		void OnExit();
		bool OnInitialize(int argc, char* argv[]);

		bool Application();
		void OnCommandLineHelp();

		void login(csString user, csString pass);

		void selectCharacter(unsigned int char_id);

		void sawServer();

		csTicks GetTicks() { return csGetTicks(); }
		void DrawFrame () 
		{
			PreProcessFrame();
			ProcessFrame();
			PostProcessFrame();
			FinishFrame();
		}
	};

} // PT namespace 

#endif // CLIENT_H
