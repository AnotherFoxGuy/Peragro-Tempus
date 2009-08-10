/*
    Copyright (C) 2008 Development Team of Peragro Tempus

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

//  !! This file was automatically generated by a network code generator   !!
//  !! Do not change this file since all changes will be overwritten later !!
//  !! Instead please change the source files here: peragro/data/generate  !!

#include "client/network/userhandler.h"

#include "common/event/eventmanager.h"
#include "common/event/entityevent.h"

#include "client/pointer/pointer.h"

void UserHandler::handleLoginResponse(GenericMessage* msg)
{
  LoginResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.login", true);
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");
    pEvent->Add("isAdmin", pmsg.getIsAdmin());

    evmgr->AddEvent(pEvent);
  }

} // end handleLoginResponse

void UserHandler::handleRegisterResponse(GenericMessage* msg)
{
  RegisterResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.register", true);
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleRegisterResponse

void UserHandler::handleCharList(GenericMessage* msg)
{
  CharListMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.character.list", true);
    csRef<iEvent> characterList = evmgr->CreateEvent("characterList", true);
    for (unsigned char i = 0; i < pmsg.getCharacterCount(); i++)
    {
      std::stringstream itemName;
      itemName << "character" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("charId", pmsg.getCharId(i));
      item->Add("name", *pmsg.getName(i)?*pmsg.getName(i):"");
      item->Add("hairColour", *pmsg.getHairColour(i));
      item->Add("skinColour", *pmsg.getSkinColour(i));
      item->Add("decalColour", *pmsg.getDecalColour(i));
      characterList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("characterList", characterList);

    evmgr->AddEvent(pEvent);
  }

} // end handleCharList

void UserHandler::handleCharCreateResponse(GenericMessage* msg)
{
  CharCreateResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.character.create", true);
    pEvent->Add("charId", pmsg.getCharId());
    pEvent->Add("name", *pmsg.getName()?*pmsg.getName():"");
    pEvent->Add("hairColour", *pmsg.getHairColour());
    pEvent->Add("skinColour", *pmsg.getSkinColour());
    pEvent->Add("decalColour", *pmsg.getDecalColour());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleCharCreateResponse

void UserHandler::handleCharSelectResponse(GenericMessage* msg)
{
  CharSelectResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("state.play", true);
    pEvent->Add("entityId", pmsg.getEntityId());
    pEvent->Add("error", *pmsg.getError()?*pmsg.getError():"");

    evmgr->AddEvent(pEvent);
  }

} // end handleCharSelectResponse

void UserHandler::handleMeshListResponse(GenericMessage* msg)
{
  MeshListResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.mesh.list", true);
    csRef<iEvent> meshesList = evmgr->CreateEvent("meshesList", true);
    for (unsigned char i = 0; i < pmsg.getMeshesCount(); i++)
    {
      std::stringstream itemName;
      itemName << "meshes" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("meshId", pmsg.getMeshId(i));
      item->Add("meshName", *pmsg.getMeshName(i)?*pmsg.getMeshName(i):"");
      item->Add("fileName", *pmsg.getFileName(i)?*pmsg.getFileName(i):"");
      meshesList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("meshesList", meshesList);

    evmgr->AddEvent(pEvent);
  }

} // end handleMeshListResponse

void UserHandler::handleAvatarListResponse(GenericMessage* msg)
{
  AvatarListResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.avatar.list", true);
    csRef<iEvent> avatarList = evmgr->CreateEvent("avatarList", true);
    for (unsigned char i = 0; i < pmsg.getAvatarCount(); i++)
    {
      std::stringstream itemName;
      itemName << "avatar" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("avatarId", pmsg.getAvatarId(i));
      item->Add("avatarName", *pmsg.getAvatarName(i)?*pmsg.getAvatarName(i):"");
      avatarList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("avatarList", avatarList);

    evmgr->AddEvent(pEvent);
  }

} // end handleAvatarListResponse

void UserHandler::handleAvatarInfoResponse(GenericMessage* msg)
{
  AvatarInfoResponseMessage pmsg;
  pmsg.deserialise(msg->getByteStream());

  using namespace PT::Events;
  EventManager* evmgr = PointerLibrary::getInstance()->getEventManager();
  {
    csRef<iEvent> pEvent = evmgr->CreateEvent("user.avatar.info", true);
    pEvent->Add("avatarId", pmsg.getAvatarId());
    pEvent->Add("avatarName", *pmsg.getAvatarName()?*pmsg.getAvatarName():"");
    pEvent->Add("meshId", pmsg.getMeshId());
    pEvent->Add("speciesId", pmsg.getSpeciesId());
    pEvent->Add("factoryName", *pmsg.getfactoryName()?*pmsg.getfactoryName():"");
    pEvent->Add("fileName", *pmsg.getfileName()?*pmsg.getfileName():"");
    csRef<iEvent> inventoryList = evmgr->CreateEvent("inventoryList", true);
    for (unsigned char i = 0; i < pmsg.getInventoryCount(); i++)
    {
      std::stringstream itemName;
      itemName << "inventory" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("inventoryItemTemplateId", pmsg.getInventoryItemTemplateId(i));
      item->Add("inventoryChance", pmsg.getInventoryChance(i));
      inventoryList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("inventoryList", inventoryList);
    csRef<iEvent> equipmentList = evmgr->CreateEvent("equipmentList", true);
    for (unsigned char i = 0; i < pmsg.getEquipmentCount(); i++)
    {
      std::stringstream itemName;
      itemName << "equipment" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("equipmentItemTemplateId", pmsg.getEquipmentItemTemplateId(i));
      item->Add("equipmentChance", pmsg.getEquipmentChance(i));
      equipmentList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("equipmentList", equipmentList);
    csRef<iEvent> reputationsList = evmgr->CreateEvent("reputationsList", true);
    for (unsigned char i = 0; i < pmsg.getReputationsCount(); i++)
    {
      std::stringstream itemName;
      itemName << "reputations" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("reputationsName", *pmsg.getReputationsName(i)?*pmsg.getReputationsName(i):"");
      item->Add("reputationsMinLevel", pmsg.getReputationsMinLevel(i));
      item->Add("reputationsMaxLevel", pmsg.getReputationsMaxLevel(i));
      item->Add("reputationsLevel", pmsg.getReputationsLevel(i));
      reputationsList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("reputationsList", reputationsList);
    csRef<iEvent> skillsList = evmgr->CreateEvent("skillsList", true);
    for (unsigned char i = 0; i < pmsg.getSkillsCount(); i++)
    {
      std::stringstream itemName;
      itemName << "skills" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("skillsName", *pmsg.getSkillsName(i)?*pmsg.getSkillsName(i):"");
      item->Add("skillsMin", pmsg.getSkillsMin(i));
      item->Add("skillsMax", pmsg.getSkillsMax(i));
      item->Add("skillsXP", pmsg.getSkillsXP(i));
      skillsList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("skillsList", skillsList);
    csRef<iEvent> hobbiesList = evmgr->CreateEvent("hobbiesList", true);
    for (unsigned char i = 0; i < pmsg.getHobbiesCount(); i++)
    {
      std::stringstream itemName;
      itemName << "hobbies" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("hobbiesName", *pmsg.getHobbiesName(i)?*pmsg.getHobbiesName(i):"");
      item->Add("hobbiesOrder", pmsg.getHobbiesOrder(i));
      item->Add("hobbiesXP", pmsg.getHobbiesXP(i));
      hobbiesList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("hobbiesList", hobbiesList);
    csRef<iEvent> abilitiesList = evmgr->CreateEvent("abilitiesList", true);
    for (unsigned char i = 0; i < pmsg.getAbilitiesCount(); i++)
    {
      std::stringstream itemName;
      itemName << "abilities" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("abilitiesName", *pmsg.getAbilitiesName(i)?*pmsg.getAbilitiesName(i):"");
      item->Add("abilitiesMin", pmsg.getAbilitiesMin(i));
      item->Add("abilitiesMax", pmsg.getAbilitiesMax(i));
      item->Add("abilitiesXP", pmsg.getAbilitiesXP(i));
      abilitiesList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("abilitiesList", abilitiesList);
    csRef<iEvent> vulberabilitiesList = evmgr->CreateEvent("vulberabilitiesList", true);
    for (unsigned char i = 0; i < pmsg.getVulberabilitiesCount(); i++)
    {
      std::stringstream itemName;
      itemName << "vulberabilities" << "_" << i;
      csRef<iEvent> item = evmgr->CreateEvent(itemName.str().c_str(), true);
      item->Add("vulberabilitiesName", *pmsg.getVulberabilitiesName(i)?*pmsg.getVulberabilitiesName(i):"");
      item->Add("vulberabilitiesMin", pmsg.getVulberabilitiesMin(i));
      item->Add("vulberabilitiesMax", pmsg.getVulberabilitiesMax(i));
      item->Add("vulberabilitiesValue", pmsg.getVulberabilitiesValue(i));
      vulberabilitiesList->Add(itemName.str().c_str(), item);
    }
    pEvent->Add("vulberabilitiesList", vulberabilitiesList);

    evmgr->AddEvent(pEvent);
  }

} // end handleAvatarInfoResponse

