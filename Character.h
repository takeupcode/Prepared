#ifndef CHARACTER_H
#define CHARACTER_H

#include "Direction.h"
#include "Display.h"
#include "Location.h"
#include "Resource.h"

#include <optional>
#include <vector>

class Behavior;
class Game;
class Level;

class Character
{
public:
    Character (
        int id,
        char symbol,
        Behavior * behavior);

    int id () const;

    char symbol () const;

    Location const & location () const;

    void setLocation (Location const & location);

    Direction direction () const;

    void setDirection (Direction direction);

    int maxHealth () const;

    void setMaxHealth (int value, bool delta = true);

    int currentHealth () const;

    void setCurrentHealth (int value, bool delta = true);

    bool isConscious () const;

    bool isDead () const;

    int attackDamage () const;

    void setAttackDamage (int value);

    std::optional<int> attackerId () const;

    void setAttackerId (std::optional<int> const & id);

    std::vector<Resource> & resources ();
    std::vector<Resource> const & resources () const;

    void draw (Display & display) const;

    bool move (Level * level, Direction direction);

    void update (Game * game);

private:
    int mId;
    char mSymbol;
    Behavior * mBehavior;
    int mMaxHealth;
    int mCurrentHealth;
    int mAttackDamage;
    std::optional<int> mAttackerId;
    Location mLocation;
    Direction mDirection;
    std::vector<Resource> mResources;
};

#endif // CHARACTER_H
