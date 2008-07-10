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

#include <cssysdef.h>
#include <imesh/objmodel.h>
#include <igeom/trimesh.h>
#include <csgeom/tri.h>
#include <iengine/mesh.h>
#include <imesh/object.h>
#include <iengine/sector.h>
#include <iengine/movable.h>

#include <physicallayer/pl.h>
#include <physicallayer/propfact.h>
#include <physicallayer/propclas.h>
#include <physicallayer/entity.h>
#include <physicallayer/persist.h>
#include <propclass/linmove.h>

#include "cmddbg.h"

#include "client/pointer/pointer.h"
#include "common/reporter/reporter.h"

#include "client/network/network.h"
#include "common/network/netmessage.h"

#include "client/gui/guimanager.h"
#include "client/gui/chat-gui.h"
#include "client/gui/whisper-gui.h"

#include "client/entity/entitymanager.h"

#include "client/data/sector/sector.h"
#include "client/data/sector/sectordatamanager.h"

namespace PT
{
  namespace Chat
  {
    cmdDbg::cmdDbg () : CommandDefault("dbg") { }

    cmdDbg::~cmdDbg () { }

    std::string cmdDbg::HelpSynopsis (const char*) const
    { return "Command used for developers to debug stuff. Read the code!"; }

    std::string cmdDbg::HelpUsage (const char*) const
    { 
      return "'/dbg [command] [args]'\n"
          "  - Write book: '/dbg write #itemid #bookid #name #text'\n"
          "  - Player Pos: '/dbg pos'\n"
          "  - Flash Step: '/dbg flashstep'\n"
          "  - Spawn Item: '/dbg spawn item #itemid #variation'\n"
          "  - Spawn Mount: '/dbg spawn mount meshname entityname'\n"
          "  - Sector: '/dbg sector sectorname [x y z]'\n"
          "  - Move: '/dbg move f|b|l|r|u|d [distance]'\n"
          "  - Sector: '/dbg rm entity #id'";
    }

    void cmdDbg::Execute (const StringArray& args)
    {
        using namespace PT::GUI;
        using namespace PT::GUI::Windows;

        Network* network = PointerLibrary::getInstance()->getNetwork();
        if(!network) return;

        GUIManager* guimanager = PointerLibrary::getInstance()->getGUIManager();
        if(!guimanager) return;
        ChatWindow* chatWindow = guimanager->GetWindow<ChatWindow>(CHATWINDOW);
        if(!chatWindow) return;

        PointerLibrary* ptr_lib = PointerLibrary::getInstance();
        Entity::EntityManager* ent_mgr = ptr_lib->getEntityManager();

        PT::Data::SectorDataManager* sectorDataMgr = ptr_lib->getSectorDataManager();

        // Element 0 is '/', 1 is 'relocate'
        if (args.size() < 3) throw BadUsage();
        else
        {
          if (args[2].compare("write") == 0)
          {
            if (args.size() < 7) throw BadUsage();

            BookWriteRequestMessage msg;
            msg.setItemId(atoi(args[3].c_str()));
            msg.setBookId(atoi(args[4].c_str()));
            msg.setBookName(ptString(args[5].c_str(), args[5].size()));

            std::string text;
            for(size_t i = 6; i < args.size(); i++){ text += args[i]; text += " ";}

            msg.setText(text.c_str());
            network->send(&msg);
          }
          else if (args[2].compare("pos") == 0)
          {
            iCelEntity* entity = ent_mgr->findCelEntById(ent_mgr->GetPlayerId());

            csRef<iPcLinearMovement> pclinmove =
              CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

            csVector3 pos = pclinmove->GetFullPosition();
            iSector* sector = pclinmove->GetSector();

            char buffer[1024];

            snprintf(buffer, 1024, "Position: %s <%f.2, %f.2, %f.2>",
              sector->QueryObject()->GetName(), pos.x, pos.y, pos.z);

            chatWindow->AddMessage (buffer);
          }
          else if (args[2].compare("flashstep") == 0)
          {
            ToggleFlashStepMessage msg;
            network->send(&msg);
          }
          else if (args[2].compare("spawn") == 0)
          {
            if (args.size() < 6) throw BadUsage();
            iCelEntity* entity = ent_mgr->findCelEntById(ent_mgr->GetPlayerId());

            csRef<iPcLinearMovement> pclinmove =
              CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

            csVector3 pos = pclinmove->GetFullPosition();
            float rotation = pclinmove->GetYRotation();
            iSector* sector = pclinmove->GetSector();

            int sectorId = sectorDataMgr->GetSectorByName(sector->QueryObject()->GetName())->GetId();

            if (args[3].compare("item") == 0)
            {
              SpawnItemMessage itemmsg;
              itemmsg.setItemId(atoi(args[4].c_str()));
              itemmsg.setVariation(atoi(args[5].c_str()));
              itemmsg.setPos(pos.x, pos.y, pos.z);
              itemmsg.setSectorId(sectorId);
              network->send(&itemmsg);
            }
            else if(args[3].compare("mount") == 0)
            {
              SpawnMountMessage mountmsg;
              mountmsg.setMesh(ptString(args[4].c_str(), args[4].length()));
              mountmsg.setName(ptString(args[5].c_str(), args[5].length()));
              mountmsg.setPos(pos.x, pos.y, pos.z);
              mountmsg.setRotation(rotation);
              mountmsg.setSectorId(sectorId);
              network->send(&mountmsg);
            }
          }
          else if (args[2].compare("sector") == 0)
          {
            Network* network = PointerLibrary::getInstance()->getNetwork();
            if(!network) return;

            if (args.size() < 4) throw BadUsage();

            iCelEntity* entity = ent_mgr->findCelEntById(ent_mgr->GetPlayerId());
            csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);

            PT::Data::Sector* sector;
            if (args[3].compare("here") == 0)
            {
              sector = sectorDataMgr->GetSectorByName(pclinmove->GetSector()->QueryObject()->GetName());
            }
            else
            {
              sector = sectorDataMgr->GetSectorByName(args[3]);
            }
            if(!sector)
            {
              chatWindow->AddMessage ("Invalid sector!");
              return;
            }

            csVector3 pos;
            if (args.size() == 4)
            {
              pos = pclinmove->GetFullPosition();
            }
            else if (args.size() < 7)
            {
              chatWindow->AddMessage ("Invalid position! Enter x, y, and z values.");
              return;
            }
            else
            {
              pos.x = atof(args[4].c_str());
              pos.y = atof(args[5].c_str());
              pos.z = atof(args[6].c_str());
            }

            TeleportRequestMessage msg;
            msg.setEntityId(ent_mgr->GetPlayerId());
            msg.setPos(pos.x, pos.y, pos.z);
            msg.setRotation(pclinmove->GetYRotation());
            msg.setSectorId(sector->GetId());
            network->send(&msg);
          }
          else if (args[2].compare("move") == 0)
          {
            Network* network = PointerLibrary::getInstance()->getNetwork();
            if(!network) return;

            if (args.size() < 4) throw BadUsage();

            PT::Data::Sector* sector = sectorDataMgr->GetSectorByName(ent_mgr->findPtEntById(ent_mgr->GetPlayerId())->GetSectorName());
            if(!sector)
            {
              chatWindow->AddMessage ("Invalid sector!");
              return;
            }

            iCelEntity* entity = ent_mgr->findCelEntById(ent_mgr->GetPlayerId());
            csRef<iPcLinearMovement> pclinmove = CEL_QUERY_PROPCLASS_ENT(entity, iPcLinearMovement);
            csVector3 pos = pclinmove->GetFullPosition();
            float rot = pclinmove->GetYRotation();

            char direction;
            float distance;
            // Loop over the arguments starting with the first direction
            int commandIndex = 3, commands = args.size();
            while (commandIndex < commands)
            {
              if (!args[commandIndex].empty())
              {
                direction = tolower(args[commandIndex].at(0));
              }
              else
              {
                commandIndex += 1;
                continue;
              }

              // Check if the distance was also specified
              if (commandIndex+1 < commands && !args[commandIndex+1].empty() &&
                  isdigit(args[commandIndex+1].at(0)))
              {
                distance = atof(args[commandIndex+1].c_str());
                if (distance == 0.0f || distance > 10000.0f || distance < -10000.0f)
                {
                  chatWindow->AddMessage("Invalid distance!");
                }
                commandIndex += 2;
              }
              else
              {
                distance = 1.0f;
                commandIndex += 1;
              }

              switch (direction)
              {
                case 'f':
                  pos.x -= distance * sinf(rot);
                  pos.z -= distance * cosf(rot);
                  break;
                case 'b':
                  pos.x += distance * sinf(rot);
                  pos.z += distance * cosf(rot);
                  break;
                case 'l':
                  pos.x += distance * cosf(rot);
                  pos.z -= distance * sinf(rot);
                  break;
                case 'r':
                  pos.x -= distance * cosf(rot);
                  pos.z += distance * sinf(rot);
                  break;
                case 'u':
                  pos.y += distance;
                  break;
                case 'd':
                  pos.y -= distance;
                  break;
                default:
                  chatWindow->AddMessage("Invalid direction! Enter f[orwards], b[ackwards], l[eft], r[ight], u[p], or d[own].");
                  break;
              }
            }

            TeleportRequestMessage msg;
            msg.setEntityId(ent_mgr->GetPlayerId());
            msg.setPos(pos.x, pos.y, pos.z);
            msg.setRotation(rot);
            msg.setSectorId(sector->GetId());
            network->send(&msg);
          }
          else if (args[2].compare("rm") == 0)
          {
            if (args.size() < 5) throw BadUsage();

            if (args[3].compare("entity") == 0)
            {
              RemoveSpawnedEntityMessage rmmsg;
              rmmsg.setEntityId(atoi(args[4].c_str()));
              network->send(&rmmsg);
            }
          }
          else if (args[2].compare("dump") == 0)
          {
            iObjectRegistry* objreg = ptr_lib->getObjectRegistry();
            csRef<iEngine> engine (csQueryRegistry<iEngine> (objreg));
            csRef<iVFS> vfs (csQueryRegistry<iVFS> (objreg));

            csRef<iStringSet> strings =
              csQueryRegistryTagInterface<iStringSet> (objreg,
              "crystalspace.shared.stringset");

            csStringID colldetid = strings->Request ("base");

            vfs->ChDir("/this");

            csRef<iFile> file (vfs->Open("dbcreate.sql", VFS_FILE_WRITE));

            std::string create;
            create += "drop table vertices;\n";
            create += "drop table triangles;\n";
            create += "drop table meshes;\n";
            create += "create table meshes (id INTEGER, sector INTEGER, name TEXT, PRIMARY KEY (id) );\n";
            create += "create table triangles (mesh INTEGER, num INTEGER, a INTEGER, b INTEGER, c INTEGER, PRIMARY KEY (mesh, num) );\n";
            create += "create table vertices (mesh INTEGER, num INTEGER, x FLOAT, y FLOAT, z FLOAT, PRIMARY KEY (mesh, num) );\n";

            file->Write(create.c_str(), create.length());
            file->Flush();

            iSectorList* sectors = engine->GetSectors();

            for (int i = 0; sectors && i < sectors->GetCount(); i++)
            {
              iSector* sector = sectors->Get(i);

              csString str ("db_");
              str += sector->QueryObject()->GetName();
              str += ".sql";

              csRef<iFile> file (vfs->Open(str.GetData(), VFS_FILE_WRITE));

              std::stringstream data;

              data << "\nBEGIN TRANSACTION;\n";

              iMeshList* meshes = sector->GetMeshes();
              for (int j = 0; meshes && j < meshes->GetCount(); j++)
              {
                iMeshWrapper* mesh = meshes->Get(j);

                unsigned int sector_id = ptr_lib->getSectorDataManager()
                  ->GetSectorByName(sector->QueryObject()->GetName())->GetId();

                data << "insert into meshes (id, sector, name) values ( "
                     << mesh->QueryObject()->GetID() << ", "
                     << sector_id << ", "
                     << "'" << mesh->QueryObject()->GetName() << "');\n";

                iObjectModel* model = mesh->GetMeshObject()->GetObjectModel();
                if (model == 0) continue;

                iTriangleMesh* trimesh = model->GetTriangleData(colldetid);
                if (trimesh == 0) continue;

                csTriangle* triangles = trimesh->GetTriangles();
                for (size_t k = 0; triangles && k < trimesh->GetTriangleCount(); k++)
                {
                  data << "insert into triangles (mesh, num, a, b, c) values ( "
                       << mesh->QueryObject()->GetID() << ", " << k << ", "
                       << triangles[k].a << ", " << triangles[k].b << ", "
                       << triangles[k].c << ");\n";
                }

                csReversibleTransform t = mesh->GetMovable()->GetFullTransform();

                csVector3* vertices = trimesh->GetVertices();
                for (size_t k = 0; vertices && k < trimesh->GetVertexCount(); k++)
                {
                  csVector3 v = t.This2Other(vertices[k]);
                  data << "insert into vertices (mesh, num, x, y, z) values ( "
                       << mesh->QueryObject()->GetID() << ", " << k << ", "
                       << v.x << ", " << v.y << ", " << v.z << ");\n";
                }

              }

              data << "\nCOMMIT;\n";

              file->Write(data.str().c_str(), data.str().length());
              file->Flush();
            }
          }
          return;
        }
    }


  } // Chat namespace
} // PT namespace
