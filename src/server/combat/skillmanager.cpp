SkillManager::SkillManager()
{
}

SkillManager::~SkillManager()
{
}

void
SkillManager::CheckDepreciation(Character* lockedCharacter)
{
  float timeUnused = 0.0f;
  timeUnused = GetTimeSkillUnused(lockedCharacter, 
                                  SkillManager::Skill::Bard);
}

float
SkillManager::GetTimeSkillUnsed(Character* lockedCharacter, 
                                SkillManager::Skill skill)
{
  return GetTimeUnusedLastLogin(lockedCharacter, skill) + 
         GetTimeUnusedThisLogin(lockedCharacter, skill);
}

void
SkillManager::CharacterLogout(Character* lockedCharacter,
                              SkillManager::Skill skill)
{
  UpdateTimeUnused(lockedCharacter, skill);
}

void
SkillManager::ResetTimeSkillLastUsed(Character* lockedCharacter,
                                     SkillManager::Skill skill)
{
}

float
SkillManager::GetTimeUnusedLastLogin(Character *lockedCharacter,
                                     SkillManager::Skill skill)
{
  const char* strSkillLastUsed = GetSkillLastUsedStr(skill);

  return GetStatValue(lockedCharacter, strSkillLastUsed) +
    GetStatValueForAllEquipedItems(lockedCharacter, strSkillLastUsed);
}

const char*
SkillManager::GetSkillLastUsedStr(SkillManager::Skill skill)
{

}


