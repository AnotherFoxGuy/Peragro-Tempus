unsigned int
ProgressionManager::CalculateExperienceGain(
                            ProgressionManagerSendTo::SendTo attackType)
{
  float successChance = 0
  float failureChance = 0
  float randomNumber = 0;
  float penalty = 0;
  float skill = 0;
  float agility = 0;

  penalty = GetPenalty(attackType);
  skill = GetSkill(attackType);
  agility = GetAgility();

  successChance = skill * agility - penalty;

  failureChance = 100 - sucessChance;

  if (randomNumber < successChance) {
    if (randomNumber >= (successChance - (failureChance * 0.1))) {
      AddXP();
    } 
  } else if (randomNumber > successChance) {
    if (randomNumber <= successChance + (failureChance * 0.1)) {
      AddXP();
    }
  } else if (randomNumber == successChance) {
    AddXP();
  }
}

void
ProgressionManager::AddXP()
{
  if (skill > ability) {
    IncreaseExperience(ProgressionManager::ABILITY, 1);
  } else {
    IncreaseExperience(ProgressionManager::SKILL, 1);
  }

  IncreaseExperience(ProgressionManager::SPECIALITY, 1);
}

void
ProgressionManager::HPIncreased(Character* lockedCharacter,
                                unsigned int amount)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber <= 10) {
      progression++;
    }
  }
  if (progression) {
    IncreaseExperience(Progressionmanager::Endurance, progression);
  }
}

void
ProgressionManager::StaminaIncreased(Character* lockedCharacter)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber == 1) {
      progression++;
    }
  }
  if (progression) {
    IncreaseExperience(Progressionmanager::Endurance, progression);
  }
}

void
ProgressionManager::WillPowerIncreased(Character* lockedCharacter)
{
  unsigned int randomNumber = 0;
  unsigned int progression = 0;
  while (amount > 0) {
    randomNumber = RollDice(1,100);
    if (randomNumber == 1) {
      progression++;
    }
  }
  if (progression) {
    IncreaseExperience(Progressionmanager::Resolve, progression);
  }
}

unsigned int RollDice(unsigned int lower, unsigned int higher)
{
  return (rand() % (higher - lower + 1)) + lower;
}
