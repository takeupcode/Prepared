#ifndef BEHAVIOR_H
#define BEHAVIOR_H

class Character;
class Game;

class Behavior
{
public:
    virtual void onUpdate (
        Game * game,
        Character * character) = 0;
};

#endif // BEHAVIOR_H
