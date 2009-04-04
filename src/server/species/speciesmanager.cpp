/*
    Copyright (C) 2009 Development Team of Peragro Tempus

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


#include "speciesmanager.h"

#include <math.h>

#include "server/server.h"

#include "common/util/printhelper.h"

#include "server/entity/entitymanager.h"
#include "server/entity/entity.h"
#include "server/entity/character/character.h"

#include "server/database/tablemanager.h"

#include "server/database/table-species.h"
#include "server/database/table-speciesabilities.h"
#include "server/database/table-speciesskills.h"
#include "server/database/table-speciesreputations.h"
#include "server/database/table-speciesinventory.h"
#include "server/database/table-speciesequipment.h"

#include "server/database/table-itemtemplates.h"
#include "server/database/table-meshes.h"
#include "server/database/table-equiptypes.h"


SpeciesManager::SpeciesManager()
{
}

SpeciesManager::~SpeciesManager()
{
}

boost::shared_ptr<NpcEntity> SpeciesManager::CreateNPCFromSpecies(size_t speciesId)
{
  Server* server = Server::getServer();

  SpeciesTable* table = server->GetTableManager()->Get<SpeciesTable>();
  SpeciesTableVOp s = table->GetSingle(speciesId);
  if (!s) return boost::shared_ptr<NpcEntity>();

  Entityp ent = server->getEntityManager()->CreateNew(Common::Entity::PCEntityType);
  boost::shared_ptr<NpcEntity> npc = boost::shared_dynamic_cast<NpcEntity>(entity);
  if (!npc) return boost::shared_ptr<NpcEntity>();

  return npc;
}
