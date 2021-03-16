#include "Character.h"

#include "Behavior.h"
#include "Game.h"

Character::Character (
    int id,
    char symbol,
    Behavior * behavior)
: mId(id),
  mSymbol(symbol),
  mBehavior(behavior),
  mMaxHealth(0),
  mCurrentHealth(0),
  mAttackDamage(0),
  mDirection(Direction::East)
{ }

int Character::id () const
{
    return mId;
}

char Character::symbol () const
{
    return mSymbol;
}

Location const & Character::location () const
{
    return mLocation;
}

void Character::setLocation (Location const & location)
{
    mLocation = location;
}

Direction Character::direction () const
{
    return mDirection;
}

void Character::setDirection (Direction direction)
{
    mDirection = direction;
}

int Character::maxHealth () const
{
    return mMaxHealth;
}

void Character::setMaxHealth (int value, bool delta)
{
    if (delta)
    {
        mMaxHealth += value;
    }
    else
    {
        mMaxHealth = value;
    }

    if (mMaxHealth < 0)
    {
        mMaxHealth = 0;
    }
}

int Character::currentHealth () const
{
    return mCurrentHealth;
}

void Character::setCurrentHealth (int value, bool delta)
{
    if (delta)
    {
        mCurrentHealth += value;
    }
    else
    {
        mCurrentHealth = value;
    }
}

bool Character::isConscious () const
{
    return mCurrentHealth > 0;
}

bool Character::isDead () const
{
    return mCurrentHealth < mMaxHealth / -5;
}

int Character::attackDamage () const
{
    return mAttackDamage;
}

void Character::setAttackDamage (int value)
{
    mAttackDamage = value;
    if (mAttackDamage < 0)
    {
        mAttackDamage = 0;
    }
}

std::optional<int> Character::attackerId () const
{
    return mAttackerId;
}

void Character::setAttackerId (std::optional<int> const & id)
{
    mAttackerId = id;
}

std::vector<Resource> & Character::resources ()
{
    return mResources;
}

std::vector<Resource> const & Character::resources () const
{
    return mResources;
}

void Character::draw (Display & display) const
{
    auto defaultCharacterId = display.game()->defaultCharacterId();
    bool isDefault = defaultCharacterId ?
        (defaultCharacterId.value() == mId) : false;

    display.setMapSymbol(
        isDefault ? '*' : mSymbol,
        mLocation,
        isDefault);
}

bool Character::move (Level * level, Direction direction)
{
    auto proposedLocation = calculateProposedLocation(
        mLocation,
        direction);

    auto moveLocation = level->calculateMoveLocation(
        mLocation,
        proposedLocation);

    if (mLocation != moveLocation)
    {
        setLocation(moveLocation);

        return true;
    }

    return false;
}

void Character::update (Game * game)
{
    if (mBehavior != nullptr)
    {
        mBehavior->onUpdate(game, this);
    }
}
